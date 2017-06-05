#ifndef _UTIL_H_
#define _UTIL_H_

#define NUM_COORDS (401 * 3) // Number of coordinates in each section of the swing dancing mocap
#define NUM_MULT 6 // Each coordinate generates 5 other coordinates (vertices of the triangle)
#define NUM_ALL (NUM_COORDS * NUM_MULT)
#define TIMESTEP .05
#define MAX_HEIGHT 2.0 
#define MAX_DIST .7

void normalize_vector(float v[3])
{
   float length = sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
   if (length > 0){
       v[0] = v[0] / length;
       v[1] = v[1] / length;
       v[2] = v[2] / length;
   }
}

// Stolen from Zoe's shape code
void calc_normal(float v[3][3], float out[3])
{
   float v1[3],v2[3];
   static const int x = 0;
   static const int y = 1;
   static const int z = 2;
   // Calculate two vectors from the three points
   v1[x] = v[0][x] - v[1][x];
   v1[y] = v[0][y] - v[1][y];
   v1[z] = v[0][z] - v[1][z];
   v2[x] = v[1][x] - v[2][x];
   v2[y] = v[1][y] - v[2][y];
   v2[z] = v[1][z] - v[2][z];
   // Take the cross product of the two vectors to get
   // the normal vector which will be stored in out
   out[x] = v1[y]*v2[z] - v1[z]*v2[y];
   out[y] = v1[z]*v2[x] - v1[x]*v2[z];
   out[z] = v1[x]*v2[y] - v1[y]*v2[x];
   // Normalize the vector
   normalize_vector(out);
}

/*
 * Normals are stored in a normal buffer, 1:1 with the vertices.
 * That is, each one of the vertices stored here (in the triangles) gets a copy of
 * the normal for that triangle. So for a given triangle, its normal n will get stored
 * three times.
 */
void compute_normals(GLfloat vert_buffer[], GLfloat norm_buffer[])
{
    int idx1, idx2, idx3;
    float v[3][3];
    float norm[3];

    // cout << "full length: " << NUM_MULT * NUM_COORDS << endl;

    for (int i = 0; i < (NUM_ALL)/3; i+=3) {
        //All the vertices should be in order in the first place ...
        idx1 = 3 * (i + 0);
        idx2 = 3 * (i + 1);
        idx3 = 3 * (i + 2);


        //The below should be all of the vertices for 1 triangle
        v[0][0] = vert_buffer[idx1+0];
        v[0][1] = vert_buffer[idx1+1];
        v[0][2] = vert_buffer[idx1+2];

        v[1][0]= vert_buffer[idx2+0];
        v[1][1]= vert_buffer[idx2+1];
        v[1][2]= vert_buffer[idx2+2];

        v[2][0]= vert_buffer[idx3+0];
        v[2][1]= vert_buffer[idx3+1];
        v[2][2]= vert_buffer[idx3+2];


        calc_normal(v, norm);

        norm_buffer[idx1+0] = norm[0];
        norm_buffer[idx2+0] = norm[0];
        norm_buffer[idx3+0] = norm[0];

        norm_buffer[idx1+1] = norm[1];
        norm_buffer[idx2+1] = norm[1];
        norm_buffer[idx3+1] = norm[1];

        norm_buffer[idx1+2] = norm[2];
        norm_buffer[idx2+2] = norm[2];
        norm_buffer[idx3+2] = norm[2];
    }
}

static float calculate_height(float ax, float ay, float az, float bx, float by, float bz){
    /* Calculate the distance between a anb b,
        The smaller the distance, the larger the height should be */

    float dist = sqrt(pow((bx - ax), 2) + pow((by - ay), 2) + pow((bz - az), 2));
    float displacement = (1 -(dist / MAX_DIST)) * MAX_HEIGHT;

    return(displacement);
}

/* Convert a single list of vertices into a buffer of triangle vertices */
static void convertVertices(GLfloat in_buffer[], GLfloat out_buffer[])
{
    GLfloat ax, ay, az, prime_ay, bx, by, bz, prime_by;
    int j = 0;
    int i;
    float displacement = 0.5;
    for (i = 0; i < NUM_COORDS - 3; i = i + 3) {
        ax = in_buffer[i];
        ay = in_buffer[i + 1];
        az = in_buffer[i + 2];
        bx = in_buffer[i + 3];
        by = in_buffer[i + 4];
        bz = in_buffer[i + 5];
        displacement = calculate_height(ax, ay, az, bx, by, bz);
        prime_ay = ay - displacement;
        prime_by = by - displacement;

        // TRIANGLE 1
        // Push A
        out_buffer[j++] = ax;
        out_buffer[j++] = ay;
        out_buffer[j++] = az;

        // Push A'
        out_buffer[j++] = ax;
        out_buffer[j++] = prime_ay;
        out_buffer[j++] = az;

        // Push B
        out_buffer[j++] = bx;
        out_buffer[j++] = by;
        out_buffer[j++] = bz;

        // TRIANGLE 2
        // Push B
        out_buffer[j++] = bx;
        out_buffer[j++] = by;
        out_buffer[j++] = bz;

        // Push A'
        out_buffer[j++] = ax;
        out_buffer[j++] = prime_ay;
        out_buffer[j++] = az;

        // Push B'
        out_buffer[j++] = bx;
        out_buffer[j++] = prime_by;
        out_buffer[j++] = bz;

    }
    // printf("Converted vertices, i = %d\n", i);
}

void walkTriangles(GLfloat in_buffer[], GLfloat out_buffer[])
{
    int i;
    float j = 0.0;
    for (i = 0; i * 3 < NUM_COORDS * NUM_MULT; i++){ //Walking by vertex (3 floats) in in_buffer
        if (i % 100 == 0) {//Changes colors every 100 listed vertices
            j += .2;
            if (j >= 1.0){
                j = 0.0;
            }
        }
        out_buffer[i] = j;
    }
}

void textureWalk(GLfloat in_buffer[], GLfloat out_buffer[])
{
    int i, j;
    j = 0;
    // essentially i want to input per 6 listed vertices
    // a, b, c, c, b, d
    // (0, 1), (0, 0), (1, 1), (1, 1), (0, 0), (1, 0)
    // that makes 12 floats
    // each vertex in the in_buffer is 3 floats
    // each coordinate in the out_buffer is 2 floats

    // Stretched out: 18
    for(i = 0; i < NUM_ALL; i+=(6 * 3 * 3)){ // 6*3 is 6 vertices * 3 floats each * 3 squares
        //Square 1
        out_buffer[j++] = 0;
        out_buffer[j++] = 1;

        out_buffer[j++] = 0;
        out_buffer[j++] = 0;

        out_buffer[j++] = .3;
        out_buffer[j++] = 1;

        out_buffer[j++] = .3;
        out_buffer[j++] = 1;

        out_buffer[j++] = 0;
        out_buffer[j++] = 0;

        out_buffer[j++] = .3;
        out_buffer[j++] = 0;

        //Square 2
        out_buffer[j++] = .3;
        out_buffer[j++] = 1;

        out_buffer[j++] = .3;
        out_buffer[j++] = 0;

        out_buffer[j++] = .6;
        out_buffer[j++] = 1;

        out_buffer[j++] = .6;
        out_buffer[j++] = 1;

        out_buffer[j++] = .3;
        out_buffer[j++] = 0;

        out_buffer[j++] = .6;
        out_buffer[j++] = 0;

        //Square 3
        out_buffer[j++] = .6;
        out_buffer[j++] = 1;

        out_buffer[j++] = .6;
        out_buffer[j++] = 0;

        out_buffer[j++] = 1;
        out_buffer[j++] = 1;

        out_buffer[j++] = 1;
        out_buffer[j++] = 1;

        out_buffer[j++] = .6;
        out_buffer[j++] = 0;

        out_buffer[j++] = 1;
        out_buffer[j++] = 0;

    }
}




void stackCoordinates(GLfloat a[], GLfloat b[], GLfloat out_buffer[])
{
    int j = 0;
    for (int i = 0; i < NUM_COORDS; i += 3){
        out_buffer[j++] = a[i];
        out_buffer[j++] = a[i + 1];
        out_buffer[j++] = a[i + 2];

        out_buffer[j++] = b[i];
        out_buffer[j++] = b[i + 1];
        out_buffer[j++] = b[i + 2];
    }
}

#endif
