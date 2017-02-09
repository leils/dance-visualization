#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <GLFW/glfw3.h>
#include <iostream>

#include <Eigen/Dense>
using namespace Eigen;

class Camera
{
public:
    Camera(Vector3f position = Vector3f(0,0,0));   //Constructor
    ~Camera();  //Destructor
    void init(GLFWwindow *window);
    Vector3f getPosition();
    void move(Vector3f shift);
    void moveFwd(float shift, float dt);
    void moveRight(float shift, float dt);
    Vector3f getLookatPt();
    Vector3f getUp();
    void mouseTracking(GLFWwindow *window, float dt);
    float bound_sphere_radius;

private:
    Vector3f position;
    Vector3f up;
    Vector3f fwd;
    Vector3f right;
    float speed;
    float lookspeed;
    void calculateDirections();
    float horizontal_angle;
    float vertical_angle;
    float lo_clamp;
    float hi_clamp;
};

#endif
