#ifndef _UTIL_H_
#define _UTIL_H_

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

#endif
