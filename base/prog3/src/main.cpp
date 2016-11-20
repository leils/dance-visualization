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


using namespace std;
using namespace Eigen;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from
shared_ptr<Program> prog;
shared_ptr<Shape> sphere;
shared_ptr<Shape> bunny;

int g_width, g_height;
float sTheta;
int gMat =0;
int color_set = 1;
int light_x = -2;
int y_rotate = 0;
int draw_setting = 0;

static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

static void mouse_callback(GLFWwindow *window, int button, int action, int mods)
{

   double posX, posY;
   if (action == GLFW_PRESS) {
      glfwGetCursorPos(window, &posX, &posY);
      cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
	}
}

static void resize_callback(GLFWwindow *window, int width, int height) {
   g_width = width;
   g_height = height;
   glViewport(0, 0, width, height);
}

//helper function to set materials
void SetMaterial(int i) {

  switch (i) {
    case 0: //shiny blue plastic
 		glUniform3f(prog->getUniform("MatAmb"), 0.02, 0.04, 0.2);
 		glUniform3f(prog->getUniform("MatDif"), 0.0, 0.16, 0.9);
      glUniform3f(prog->getUniform("MatSpec"), 0.14, 0.2, 0.8);
      glUniform1f(prog->getUniform("shine"), 120.0);
      //set specular to: (0.14, 0.2, 0.8);
      //set shine to: (120.0);
        break;
    case 1: // flat grey
 		glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
 		glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
      glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
      glUniform1f(prog->getUniform("shine"), 4.0);
      //set specular to: (0.3, 0.3, 0.4);
      //set shine to: (4.0);
      break;
    case 2: //brass
 		glUniform3f(prog->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
 		glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
      glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
      glUniform1f(prog->getUniform("shine"), 27.9);
      //set specualr to: (0.9922, 0.941176, 0.80784);
      //set shine to: (27.9);
        break;
	 case 3: //copper
 		glUniform3f(prog->getUniform("MatAmb"), 0.1913, 0.0735, 0.0225);
 		glUniform3f(prog->getUniform("MatDif"), 0.7038, 0.27048, 0.0828);
      glUniform3f(prog->getUniform("MatSpec"), 0.257, 0.1376, 0.08601);
      glUniform1f(prog->getUniform("shine"), 12.8);
      //set specualr to: (0.257, 0.1376, 0.08601);
      //set shine to: (12.8);
        break;
    case 4: //slate
      glUniform3f(prog->getUniform("MatAmb"), 0.1, 0.1, 0.1);
      glUniform3f(prog->getUniform("MatDif"), 0.5, 0.5, 0.5);
      glUniform3f(prog->getUniform("MatSpec"), 0.9, 0.9, 0.9);
      glUniform1f(prog->getUniform("shine"), 300.0);
      //set specualr to: (0.257, 0.1376, 0.08601);
      //set shine to: (12.8);
        break;
	}
}

static void init()
{
	GLSL::checkVersion();

	sTheta = 0;
	// Set background color.
	glClearColor(.12f, .34f, .56f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

	// Initialize mesh.
	sphere = make_shared<Shape>();
	sphere->loadMesh(RESOURCE_DIR + "icoNoNormals.obj");
	sphere->resize();
   sphere->compute_normals(RESOURCE_DIR + "icoNoNormals.obj");
	sphere->init();

   bunny = make_shared<Shape>();
   bunny->loadMesh(RESOURCE_DIR + "bunnyNoNorm.obj");
   bunny->resize();
   bunny->compute_normals(RESOURCE_DIR + "bunnyNoNorm.obj");
   bunny->init();

	// Initialize the GLSL program.
	prog = make_shared<Program>();
	prog->setVerbose(true);
	prog->setShaderNames(RESOURCE_DIR + "simple_vert.glsl", RESOURCE_DIR + "simple_frag.glsl");
	prog->init();
	prog->addUniform("P");
	prog->addUniform("MV");
	prog->addUniform("MatAmb");
	prog->addUniform("MatDif");
   prog->addUniform("MatSpec");
   prog->addUniform("shine");
	prog->addAttribute("vertPos");
	prog->addAttribute("vertNor");
   prog->addUniform("color_setting");
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
   float aspect = width/(float)height;

   // Create the matrix stacks - please leave these alone for now
   auto P = make_shared<MatrixStack>();
   auto MV = make_shared<MatrixStack>();
   // Apply perspective projection.
   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, 100.0f);

	// Draw a stack of cubes with indiviudal transforms 
	prog->bind();
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
   glUniform1i(prog->getUniform("color_setting"), color_set);
   glUniform1i(prog->getUniform("light_x"), light_x);

	SetMaterial(gMat);
	/* draw my sphere!*/	
   MV->pushMatrix();
     MV->loadIdentity();
		MV->translate(Vector3f(1, 0, -5));
	  	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
      sphere->draw(prog);
   MV->popMatrix();

	prog->unbind();

   // Pop matrix stacks.
   P->popMatrix();

	if (sTheta > -70) {
		sTheta -= 0.5;
	}
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
   //request the highest possible version of OGL - important for mac
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "Leia", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	//weird bootstrap of glGetError
   glGetError();
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
   cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	// Set vsync.
	glfwSwapInterval(1);
   //set the mouse call back
   glfwSetMouseButtonCallback(window, mouse_callback);
   //set the window resize call back
   glfwSetFramebufferSizeCallback(window, resize_callback);

	// Initialize scene. Note geometry initialized in init now
	init();

	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
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
