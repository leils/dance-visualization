/* P3 materials example - see SetMaterial and fragment shader
CPE 471 Cal Poly Z. Wood + S. Sueda
*/
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "shape.h"

/* Should I be using shapes? 
   Like ... we've only ever imported shapes from files. 
   The "shape" is there to interpret the mesh. 
   What about what I have now? I just want to draw 4 vectors. 
   And I need to do calculations on them. 

   Do I know how to do the projection & perspective matrices
   on pure vector lists? 

   Lab4 would have non-shape rendering. 
   */

using namespace std;
using namespace Eigen;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from
shared_ptr<Program> prog;

GLuint VertexArrayID;
static const GLfloat g_vertex_buffer_data[] = {
  -1.0f, -1.0f, 0.0f,
  1.0f, -1.0f, 0.0f,
  0.0f, 1.0f, 0.0f
};
GLuint vertexbuffer; 

shared_ptr<Shape> cube;

int g_width, g_height;
float sTheta;
int gMat = 0;
int light_x = -2;
int t = 0;
Vector3f eye = Vector3f();
Vector3f lookAtPt = Vector3f();
Vector3f up = Vector3f(0, 1, 0);
Vector3f eye_forward = Vector3f();
Vector3f eye_right = Vector3f();

GLboolean lock_view = false;
GLboolean lock_y = true;

float phi = 0;
float theta = -M_PI / 2;

static void error_callback(int error, const char *description)
{
  cerr << description << endl;
}

static void calculate_directions()
{
  eye_forward = lookAtPt - eye;
  if (lock_y) {
    eye_forward[1] = 0.0;
  }
  eye_forward.normalize();
  eye_right = Vector3f(-eye_forward.z(), eye_forward.y(), eye_forward.x());
  if (lock_y) {
    eye_right[1] = 0.0;
  }
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  } else if (key == GLFW_KEY_A) {
    eye -= eye_right / 5;
  } else if (key == GLFW_KEY_D) {
    eye += eye_right / 5;
  } else if (key == GLFW_KEY_W) {
    eye += eye_forward / 5;
  } else if (key == GLFW_KEY_S) {
    eye -= eye_forward / 5;
  } else if (key == GLFW_KEY_L && action == GLFW_PRESS) {
    lock_view = !lock_view;
  } else if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
    lock_y = !lock_y;
  }

}

static void mouse_track(GLFWwindow *window) {
  /* I should update this to use keypress, since mouse callback 
    is really hard to make right. 
    */ 
  double posX, posY;
  glfwGetCursorPos(window, &posX, &posY);

  float dy = -(posY - (g_height / 2)) / (g_height / 2);
  float dx = (posX - (g_width / 2)) / (g_width / 2);

  if (!lock_view) {
    theta += M_PI * (dx / 400);
    phi += M_PI * (dy / 400);
  }

  if (phi < -1.4) {
    phi = -1.4;
  }
  else if (phi > 1.4) {
    phi = 1.4;
  }
}


static void resize_callback(GLFWwindow *window, int width, int height) {
  g_width = width;
  g_height = height;
  glViewport(0, 0, width, height);
}

//helper function to set materials
void SetMaterial(int i) 
{

  switch (i) {
  case 0: // grass
    glUniform3f(prog->getUniform("MatAmb"), 0.02, 0.54, 0.2);
    glUniform3f(prog->getUniform("MatDif"), 0.0, 0.76, 0.9);
    glUniform3f(prog->getUniform("MatSpec"), 0.0, 0.0, 0.0);
    glUniform1f(prog->getUniform("shine"), 00.0);
    //set specular to: (0.14, 0.2, 0.8);
    //set shine to: (120.0);
    break;
  case 1: // pink fur
    glUniform3f(prog->getUniform("MatAmb"), 0.53, 0.13, 0.14);
    glUniform3f(prog->getUniform("MatDif"), 0.5, 0.2, 0.2);
    glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
    glUniform1f(prog->getUniform("shine"), 4.0);
    //set specular to: (0.3, 0.3, 0.4);
    //set shine to: (4.0);
    break;
  }
}

static void initGeom() {
  //generate the VAO
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  //generate vertex buffer to hand off to OGL
  glGenBuffers(1, &vertexbuffer);
  //set the current state to focus on our vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  //actually memcopy the data - only do this once
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);
}

static void init()
{
  GLSL::checkVersion();

  sTheta = 0;
  // Set background color.
  glClearColor(.12f, .34f, .56f, 1.0f);
  // Enable z-buffer test.
  glEnable(GL_DEPTH_TEST);


  cube = make_shared<Shape>();
  cube->loadMesh(RESOURCE_DIR + "cube.obj");
  cube->resize();
  cube->compute_normals(RESOURCE_DIR + "cube.obj");
  cube->init();

  // Initialize the GLSL program.
  prog = make_shared<Program>();
  prog->setVerbose(true);
  prog->setShaderNames(RESOURCE_DIR + "simple_vert.glsl", RESOURCE_DIR + "simple_frag.glsl");
  prog->init();
  prog->addUniform("P");
  prog->addUniform("M");
  prog->addUniform("V");
  prog->addUniform("MatAmb");
  prog->addUniform("MatDif");
  prog->addUniform("MatSpec");
  prog->addUniform("shine");
  prog->addAttribute("vertPos");
  prog->addAttribute("vertNor");
  prog->addUniform("light_x");
}

static void render()
{
  // Get current frame buffer size.
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  // Clear framebuffer.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Use the matrix stack for Lab 6
  float aspect = width / (float)height;

  // Create the matrix stacks - please leave these alone for now
  auto P = make_shared<MatrixStack>();
  auto M = make_shared<MatrixStack>();
  auto V = make_shared<MatrixStack>();
  // Apply perspective projection.
  P->pushMatrix();
  P->perspective(45.0f, aspect, 0.01f, 100.0f);

  // bind this program, start drawing perspective
  prog->bind();
  glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
  glUniform1i(prog->getUniform("light_x"), light_x);

  mouse_track(window);
  lookAtPt = Vector3f(cos(phi) * cos (theta), sin(phi), cos(phi) * cos((M_PI / 2) - theta)) + eye;
  calculate_directions();

  V->lookAt(eye, lookAtPt, up);
  glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());

  SetMaterial(0);
  M->pushMatrix();
  M->loadIdentity();

  //draw the cube with these 'global transforms'
  SetMaterial(1);
  M->translate(Vector3f(0, 0, -5));
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
    cube->draw(prog);
  M->popMatrix();

  // Pop matrix stacks.
  P->popMatrix();
  //V->popMatrix();

  prog->unbind();

  if (sTheta > -70) {
    sTheta -= 0.5;
  }
  t++;
}

int main(int argc, char **argv)
{
  if (argc < 2) {
    cout << "Please specify the resource directory." << endl;
    return 0;
  }
  RESOURCE_DIR = argv[1] + string("/");

  // Set error callback.
  glfwSetErrorCallback(error_callback);
  // Initialize the library.
  if (!glfwInit()) {
    return -1;
  }
  //request the highest possible version of OGL - important for mac
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  // Create a windowed mode window and its OpenGL context.
  g_width = 640;
  g_height = 480;
  window = glfwCreateWindow(640, 480, "Draw The Thing", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  // Make the window's context current.
  glfwMakeContextCurrent(window);
  // Initialize GLEW.
  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    cerr << "Failed to initialize GLEW" << endl;
    return -1;
  }
  //weird bootstrap of glGetError
  glGetError();
  cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
  cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

  // Set vsync.
  glfwSwapInterval(1);
  // Set keyboard callback.
  glfwSetKeyCallback(window, key_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  //set the window resize call back
  glfwSetFramebufferSizeCallback(window, resize_callback);

  // Initialize scene. Note geometry initialized in init now
  init();

  // Loop until the user closes the window.
  while (!glfwWindowShouldClose(window)) {
    // Render scene.
    render();
    // Swap front and back buffers.
    glfwSwapBuffers(window);
    // Poll for and process events.
    glfwPollEvents();
  }
  // Quit program.
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
