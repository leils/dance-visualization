#include "Camera.h"

using namespace std;

Camera::Camera(Vector3f pos)
{
    position = pos;
    up = Vector3f(0, 1, 0);
    fwd = Vector3f(0, 0, -1); // necessary? Init to (0,0,0)?
    right = Vector3f(1, 0, 0);
    speed = 5.0f; // 1 unit per sec
    lookspeed = 0.05f;
    horizontal_angle = -3.14f;
    vertical_angle = -.5f;
    lo_clamp = -1.0f;
    hi_clamp = 1.0f;
    bound_sphere_radius = 1.0f;
}

Camera::~Camera()
{
}

void Camera::init(GLFWwindow *window)
{
    double xpos, ypos;
    glfwSetCursorPos(window, 320, 240);
    glfwGetCursorPos(window, &xpos, &ypos);
    calculateDirections();
}

Vector3f Camera::getPosition()
{
    return position;
}

void Camera::move(Vector3f shift)
{
    position += shift;
}

void Camera::moveFwd(float shift, float dt)
{
    position += shift * fwd * speed * dt;
}

void Camera::moveRight(float shift, float dt)
{
    position += shift * right * speed * dt;
}

Vector3f Camera::getLookatPt()
{
    return position + fwd;
}

Vector3f Camera::getUp()
{
    return up;
}

void Camera::calculateDirections()
{
    fwd = Vector3f(cos(vertical_angle) * sin(horizontal_angle),
        sin(vertical_angle),
        cos(vertical_angle) * cos(horizontal_angle));
    fwd.normalize();

    right = Vector3f(
        sin(horizontal_angle - 3.14f/2.0f),
        0,
        cos(horizontal_angle - 3.14f/2.0f));
    right.normalize();

    up = right.cross(fwd);
    up.normalize();
}

void Camera::mouseTracking(GLFWwindow *window, float dt)
{
    double xpos, ypos;
    int w_width, w_height;
    glfwGetWindowSize(window, &w_width, &w_height);
    glfwGetCursorPos(window, &xpos, &ypos);

    cout << "x: " << xpos << " y: " << ypos << endl;

    horizontal_angle += float(w_width/2 - xpos) * lookspeed * dt;
    vertical_angle += float(w_height/2 - ypos) * lookspeed * dt;
    // Clamp vertical so no rolling is possible
    if (vertical_angle > hi_clamp) {
        vertical_angle = hi_clamp;
    }
    if (vertical_angle < lo_clamp) {
        vertical_angle = lo_clamp;
    }
    // vertical_angle = clamp(vertical_angle, lo_clamp, hi_clamp);


    cout << "vert: " << vertical_angle << " hor: " << horizontal_angle << endl;
    // std::cout << horizontal_angle << " " << vertical_angle << std::endl;

    calculateDirections();
    glfwSetCursorPos(window, w_width/2, w_height/2);
}
