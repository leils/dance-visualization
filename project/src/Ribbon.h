#ifndef _RIBBON_H_
#define _RIBBON_H_

#include <GLFW/glfw3.h>

#define NUM_COORDS (401 * 3) // Number of coordinates in each section of the swing dancing mocap
#define NUM_MULT 6 // Each coordinate generates 5 other coordinates (vertices of the triangle)
#define NUM_ALL (NUM_COORDS * NUM_MULT)

class Ribbon
{
public:
    Ribbon();
    // static GLfloat g_vertex_buffer[NUM_ALL];
    // static GLfloat g_normal_buffer[NUM_ALL];
    // static GLfloat color_buffer[NUM_ALL / 3];
    // static GLfloat tex_buffer[(NUM_ALL / 3) * 2];
private:
    int test;

};

#endif
