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

#define NUM_COORDS 3 * 3

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

GLuint VertexArrayID2;
static const GLfloat g_vertex_buffer_data2[] = {
  -1.0f, -1.0f, 0.0f,
  1.0f, -1.0f, 0.0f,
  0.0f, 1.0f, 0.0f,

  1.0f, -1.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 
  2.0f, 1.0f, 0.0f
};
GLuint vertexbuffer2; 

static GLfloat orig_vertex_buffer[] = {
  -1.0f, -1.0f, 0.0f,
  0.0f, 0.0f, 0.0f,
  1.0f, 1.0f, 0.0f
};

GLuint VertexArrayID;
// Array to fill with the converted vertices
static GLfloat g_vertex_buffer_data[NUM_COORDS * 3];
GLuint vertexbuffer; 

int g_width, g_height;
float sTheta;
// TODO (leia): remove
int gMat = 0;
int light_x = -2;
int t = 0;

// Eye vectors for viewpoint moving 
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
    eye -= eye_right / 5; // Move eye left
  } else if (key == GLFW_KEY_D) {
    eye += eye_right / 5; // Move eye right
  } else if (key == GLFW_KEY_W) {
    eye += eye_forward / 5; // Move eye forward
  } else if (key == GLFW_KEY_S) {
    eye -= eye_forward / 5; // Move eye backward
  } else if (key == GLFW_KEY_L && action == GLFW_PRESS) {
    lock_view = !lock_view; // lock the viewpoint
  } else if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
    lock_y = !lock_y; // lock the eye in the y axis
  } else if (key == GLFW_KEY_RIGHT) {
    // theta defines l/r of the view. Positive to the right.
    theta += .1; // turn eye to the right
  } else if (key == GLFW_KEY_LEFT) {
    theta -= .1; // turn the eye to the left
  } else if (key == GLFW_KEY_UP) {
    // phi defines u/d of the view. Positive up.
    phi += .1; // turn the eye upward
  } else if (key == GLFW_KEY_DOWN) {
    phi -= .1; // turn the eye downward
  } 
}

static void mouse_track(GLFWwindow *window) {
  /* Removed viewpoint mechanics here */
}


static void resize_callback(GLFWwindow *window, int width, int height) {
  g_width = width;
  g_height = height;
  glViewport(0, 0, width, height);
}

//helper function to set materials
void SetMaterial(int i) 
{
  // TODO (leia): remove this when I change the shader 
  switch (i) {
  case 0: // grass
    glUniform3f(prog->getUniform("MatAmb"), 0.02, 0.54, 0.2);
    glUniform3f(prog->getUniform("MatDif"), 0.0, 0.76, 0.9);
    glUniform3f(prog->getUniform("MatSpec"), 0.0, 0.0, 0.0);
    glUniform1f(prog->getUniform("shine"), 00.0);
    break;
  case 1: // pink fur
    glUniform3f(prog->getUniform("MatAmb"), 0.53, 0.13, 0.14);
    glUniform3f(prog->getUniform("MatDif"), 0.5, 0.2, 0.2);
    glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
    glUniform1f(prog->getUniform("shine"), 4.0);
    break;
  }
}

static void printVertices() {
  printf("Original vertices: \n");
  for (int i = 0; i < NUM_COORDS; i++) {
    printf("OG_VERTEX: %f\n", (float)(orig_vertex_buffer[i]));
  }
}

static void printNewVertices() {
  printf("New vertices: \n");
  for (int i = 0; i < NUM_COORDS * 3; i++) {
    printf("%f\n", (float)(g_vertex_buffer_data[i]));
  }
}

static void convertVertices() {
  GLfloat ax, ay, az, prime_ay, bx, by, bz, prime_by;
  int j = 0;
  for (int i = 0; i < NUM_COORDS - 3; i = i + 3) {
    ax = orig_vertex_buffer[i];
    ay = orig_vertex_buffer[i + 1];
    az = orig_vertex_buffer[i + 2];
    bx = orig_vertex_buffer[i + 3];
    by = orig_vertex_buffer[i + 4];
    bz = orig_vertex_buffer[i + 5];
    prime_ay = ay - .5;
    prime_by = by - .5;

    // Push A
    g_vertex_buffer_data[j++] = ax;
    g_vertex_buffer_data[j++] = ay;
    g_vertex_buffer_data[j++] = az;

    // Push A' 
    g_vertex_buffer_data[j++] = ax;
    g_vertex_buffer_data[j++] = prime_ay;
    g_vertex_buffer_data[j++] = az;

    // Push B 
    g_vertex_buffer_data[j++] = bx;
    g_vertex_buffer_data[j++] = by;
    g_vertex_buffer_data[j++] = bz;

    // Push B 
    g_vertex_buffer_data[j++] = bx;
    g_vertex_buffer_data[j++] = by;
    g_vertex_buffer_data[j++] = bz;

    // Push A' 
    g_vertex_buffer_data[j++] = ax;
    g_vertex_buffer_data[j++] = prime_ay;
    g_vertex_buffer_data[j++] = az;

    // Push B'
    g_vertex_buffer_data[j++] = bx;
    g_vertex_buffer_data[j++] = prime_by;
    g_vertex_buffer_data[j++] = bz;

  }
  printf("Converted vertices\n");
}

static void initGeom() {
  //generate the VAO
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  //generate vertex buffer to hand off to OGL
  /* TODO (leia): do the vertex buffer change here. Since each vertex is passed in via
      three floats, I'll have to deal with them in triplets. 
      Do I need to write some extra helper functions to do that? 
      I might want to. 

      Remember that cpp doesn't allow arrays to be mutated. 
      dynamic arrays? 
      I shouldn't even need dynamic: we know the fixed size. 
      */

  printVertices();
  convertVertices();
  printNewVertices();

  glGenBuffers(1, &vertexbuffer);
  //set the current state to focus on our vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  //actually memcopy the data - only do this once
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);
  //glBufferData(GL_ARRAY_BUFFER, sizeof(orig_vertex_buffer), orig_vertex_buffer, GL_DYNAMIC_DRAW);
}

static void init()
{
  GLSL::checkVersion();

  sTheta = 0;
  // Set background color.
  glClearColor(.12f, .34f, .56f, 1.0f);
  // Enable z-buffer test.
  glEnable(GL_DEPTH_TEST);

  initGeom();

  // Initialize the GLSL program.
  prog = make_shared<Program>();
  prog->setVerbose(true);
  prog->setShaderNames(RESOURCE_DIR + "simple_vert.glsl", RESOURCE_DIR + "simple_frag.glsl");
  prog->init();
  prog->addUniform("P");
  prog->addUniform("M");
  prog->addUniform("V");

  /* TODO(leia): Eventually just take this out */
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

  float aspect = width / (float)height;

  // Create the matrix stacks 
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

  // mouse_track(window);
  lookAtPt = Vector3f(cos(phi) * cos (theta), sin(phi), cos(phi) * cos((M_PI / 2) - theta)) + eye;
  calculate_directions();

  V->lookAt(eye, lookAtPt, up);
  glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());

  M->pushMatrix();
  M->loadIdentity();

  //draw the triangles
  SetMaterial(1);
  //set up pulling of vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  //function to get # of elements at a time
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
  glDrawArrays(GL_TRIANGLES, 0, 9);
  glDisableVertexAttribArray(0);

  //draw the cube with these 'global transforms'
  SetMaterial(1);
  M->translate(Vector3f(0, 0, -5));
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
    //cube->draw(prog);
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

  // Initialize scene. Note that geometry initialized in init now
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
