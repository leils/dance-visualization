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
#include "Camera.h"
#include "util.h"
#include "Texture.h"
#include "data.h"
#include "buffer_structs.h"
// #include "Ribbon.h"


using namespace std;
using namespace Eigen;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from
shared_ptr<Program> ribbon_prog, line_prog;

Camera *cam = new Camera();
// Ribbon *testRibbon = new Ribbon(right_knee_buffer);

// Array to fill with the converted vertices (Only for Ankle Ribbons)
static GLfloat g_vertex_right_ankle_buffer[NUM_ALL];
static GLfloat g_vertex_right_ankle_normal_buffer[NUM_ALL];
static GLfloat right_ankle_color_buffer[NUM_ALL/ 3];
static GLfloat right_ankle_tex_buffer[(NUM_ALL/ 3) * 2]; // Texture is 2d, and 1 set of texture coords per actual coordinate

static GLfloat g_vertex_left_ankle_buffer[NUM_ALL];
static GLfloat g_vertex_left_ankle_normal_buffer[NUM_ALL];
static GLfloat left_ankle_color_buffer[NUM_ALL/ 3];
static GLfloat left_ankle_tex_buffer[(NUM_ALL/ 3) * 2]; // Texture is 2d, and 1 set of texture coords per actual coordinate

static GLfloat g_vertex_waist_buffer[NUM_COORDS * 2]; // All lines will have 2 points: 6 coordinates
static GLfloat g_vertex_shoulder_buffer[NUM_COORDS * 2]; // All lines will have 2 points: 6 coordinates

Texture texture0;
GLint h_texture_0;

int g_width, g_height;
int t;

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

// TODO: Timestep this
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    } else if (key == GLFW_KEY_A) {
        eye -= eye_right / 5; // Move eye left
        cam->moveRight(-1, TIMESTEP);
    } else if (key == GLFW_KEY_D) {
        eye += eye_right / 5; // Move eye right
        cam->moveRight(1, TIMESTEP);
    } else if (key == GLFW_KEY_W) {
        eye += eye_forward / 5; // Move eye forward
        cam->moveFwd(1, TIMESTEP);
    } else if (key == GLFW_KEY_S) {
        eye -= eye_forward / 5; // Move eye backward
        cam->moveFwd(-1, TIMESTEP);
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

static void resize_callback(GLFWwindow *window, int width, int height) {
    g_width = width;
    g_height = height;
    glViewport(0, 0, width, height);
}

static void initGeom()
{
    //generate vertex buffer to hand off to OGL
    convertVertices(right_ankle_buffer, g_vertex_right_ankle_buffer);
    compute_normals(g_vertex_right_ankle_buffer, g_vertex_right_ankle_normal_buffer);
    walkTriangles(g_vertex_right_ankle_buffer, right_ankle_color_buffer);
    textureWalk(g_vertex_right_ankle_buffer, right_ankle_tex_buffer);

    convertVertices(left_ankle_buffer, g_vertex_left_ankle_buffer);
    compute_normals(g_vertex_left_ankle_buffer, g_vertex_left_ankle_normal_buffer);
    walkTriangles(g_vertex_left_ankle_buffer, left_ankle_color_buffer);
    textureWalk(g_vertex_left_ankle_buffer, left_ankle_tex_buffer);


    /* -------------------ANKLE----------------- */
    //generate the VAO
    glGenVertexArrays(1, &Right_AnkleArrayID);
    glBindVertexArray(Right_AnkleArrayID);

    // Right_Ankle Vertex Buffer
    glGenBuffers(1, &right_ankle_vertexbuffer);
    //set the current state to focus on our vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, right_ankle_vertexbuffer);
    //actually memcopy the data - only do this once
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_right_ankle_buffer), g_vertex_right_ankle_buffer, GL_DYNAMIC_DRAW);

    // Right_Ankle Normal Buffer
    glGenBuffers(1, &Right_AnkleNormalID);
    glBindBuffer(GL_ARRAY_BUFFER, Right_AnkleNormalID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_right_ankle_normal_buffer), g_vertex_right_ankle_normal_buffer, GL_STATIC_DRAW);

    glGenBuffers(1, &Right_AnkleColorID);
    glBindBuffer(GL_ARRAY_BUFFER, Right_AnkleColorID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(right_ankle_color_buffer), right_ankle_color_buffer, GL_STATIC_DRAW);

    glGenBuffers(1, &Right_AnkleTextureID);
    glBindBuffer(GL_ARRAY_BUFFER, Right_AnkleTextureID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(right_ankle_tex_buffer), right_ankle_tex_buffer, GL_STATIC_DRAW);

    /* -------------------ANKLE----------------- */
    //generate the VAO
    glGenVertexArrays(1, &Left_AnkleArrayID);
    glBindVertexArray(Left_AnkleArrayID);

    // Left_Ankle Vertex Buffer
    glGenBuffers(1, &left_ankle_vertexbuffer);
    //set the current state to focus on our vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, left_ankle_vertexbuffer);
    //actually memcopy the data - only do this once
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_left_ankle_buffer), g_vertex_left_ankle_buffer, GL_DYNAMIC_DRAW);

    // Left_Ankle Normal Buffer
    glGenBuffers(1, &Left_AnkleNormalID);
    glBindBuffer(GL_ARRAY_BUFFER, Left_AnkleNormalID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_left_ankle_normal_buffer), g_vertex_left_ankle_normal_buffer, GL_STATIC_DRAW);

    glGenBuffers(1, &Left_AnkleColorID);
    glBindBuffer(GL_ARRAY_BUFFER, Left_AnkleColorID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(left_ankle_color_buffer), left_ankle_color_buffer, GL_STATIC_DRAW);

    glGenBuffers(1, &Left_AnkleTextureID);
    glBindBuffer(GL_ARRAY_BUFFER, Left_AnkleTextureID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(left_ankle_tex_buffer), left_ankle_tex_buffer, GL_STATIC_DRAW);

    /* ----------------- Full Waist ---------------*/
    stackCoordinates(right_front_waist, left_front_waist, g_vertex_waist_buffer);

    glGenVertexArrays(1, &Full_WaistArrayID);
    glBindVertexArray(Full_WaistArrayID);

    glGenBuffers(1, &full_waist_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, full_waist_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_waist_buffer), g_vertex_waist_buffer, GL_DYNAMIC_DRAW);

    /* ----------------- Full Waist ---------------*/
    stackCoordinates(right_shoulder, left_shoulder, g_vertex_shoulder_buffer);

    glGenVertexArrays(1, &ShouldersArrayID);
    glBindVertexArray(ShouldersArrayID);

    glGenBuffers(1, &shoulders_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, shoulders_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_shoulder_buffer), g_vertex_shoulder_buffer, GL_DYNAMIC_DRAW);


    cout << glGetError() << endl;
    //clear
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //clear
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void init()
{
    GLSL::checkVersion();
    t = 0;
    // Set background color.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    // Enable z-buffer test.
    glEnable(GL_DEPTH_TEST);
    // glPointSize(10.0f);
    // glLineWidth(100.0f); NOT WORKING: ???

    initGeom();

    texture0.setFilename(RESOURCE_DIR + "brush_texture4.bmp");
    // Initialize textures
    texture0.setUnit(0);
    texture0.setName("Texture0");
    texture0.init();

    // Initialize the Ribbon GLSL program.
    ribbon_prog = make_shared<Program>();
    ribbon_prog->setVerbose(true);
    ribbon_prog->setShaderNames(RESOURCE_DIR + "ribbon_vert.glsl", RESOURCE_DIR + "ribbon_frag.glsl");
    ribbon_prog->init();
    ribbon_prog->addUniform("P");
    ribbon_prog->addUniform("M");
    ribbon_prog->addUniform("V");

    ribbon_prog->addAttribute("vertPos");
    ribbon_prog->addAttribute("vertNor");
    ribbon_prog->addAttribute("vertColor");
    ribbon_prog->addAttribute("vertTex");
    ribbon_prog->addUniform("lightDir");

    ribbon_prog->addUniform("Texture0");
    ribbon_prog->addTexture(&texture0);

    // Initialize the Ribbon GLSL program.
    line_prog = make_shared<Program>();
    line_prog->setVerbose(true);
    line_prog->setShaderNames(RESOURCE_DIR + "line_vert.glsl", RESOURCE_DIR + "line_frag.glsl");
    line_prog->init();
    line_prog->addUniform("P");
    line_prog->addUniform("M");
    line_prog->addUniform("V");
    line_prog->addAttribute("vertPos");

    cam->init(window);
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
    lookAtPt = Vector3f(cos(phi) * cos (theta), sin(phi), cos(phi) * cos((M_PI / 2) - theta)) + eye;
    calculate_directions();

    if (t > 100){
        cam->mouseTracking(window, TIMESTEP);
    } else {
        glfwSetCursorPos(window, 320, 240);
    }

    // bind this ribbon_program, start drawing perspective
    ribbon_prog->bind();
    glUniformMatrix4fv(ribbon_prog->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
    glUniform3f(ribbon_prog->getUniform("lightDir"), -5, -3, 5);

    V->lookAt(cam->getPosition(), cam->getLookatPt(), cam->getUp());
    // V->lookAt(eye, lookAtPt, up);
    glUniformMatrix4fv(ribbon_prog->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());

    M->pushMatrix();
    M->loadIdentity();

    M->translate(Vector3f(0, -10, -20));
    M->rotate(-90, Vector3f(1, 0, 0)); //Rotate by 90 degrees for correct orientation
    M->scale(.5);
        glUniformMatrix4fv(ribbon_prog->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
        //set up pulling of vertices
        int num_to_draw = t * 9;
        int h_pos, h_nor, v, tex;
        h_pos = h_nor = v = -1;

        /*-------------------------Draw Right ankle--------------------*/

        h_pos = ribbon_prog->getAttribute("vertPos");
        glEnableVertexAttribArray(h_pos);
        glBindBuffer(GL_ARRAY_BUFFER, right_ankle_vertexbuffer);
        glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0); //function to get # of elements at a time

        //ankle normals
        h_nor = ribbon_prog->getAttribute("vertNor");
        glEnableVertexAttribArray(h_nor);
        glBindBuffer(GL_ARRAY_BUFFER, Right_AnkleNormalID);
        glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        v = ribbon_prog->getAttribute("vertColor");
        glEnableVertexAttribArray(v);
        glBindBuffer(GL_ARRAY_BUFFER, Right_AnkleColorID);
        glVertexAttribPointer(v, 1, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        tex = ribbon_prog->getAttribute("vertTex");
        glEnableVertexAttribArray(tex);
        glBindBuffer(GL_ARRAY_BUFFER, Right_AnkleTextureID);
        glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        glDrawArrays(GL_TRIANGLES, 0, num_to_draw); // TODO: adding a time based amt here
        glDisableVertexAttribArray(h_pos);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(h_nor);
        glDisableVertexAttribArray(v);
        glDisableVertexAttribArray(tex);

        /*-------------------------Draw Left ankle--------------------*/

        h_pos = ribbon_prog->getAttribute("vertPos");
        glEnableVertexAttribArray(h_pos);
        glBindBuffer(GL_ARRAY_BUFFER, left_ankle_vertexbuffer);
        glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0); //function to get # of elements at a time

        //ankle normals
        h_nor = ribbon_prog->getAttribute("vertNor");
        glEnableVertexAttribArray(h_nor);
        glBindBuffer(GL_ARRAY_BUFFER, Left_AnkleNormalID);
        glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        v = ribbon_prog->getAttribute("vertColor");
        glEnableVertexAttribArray(v);
        glBindBuffer(GL_ARRAY_BUFFER, Left_AnkleColorID);
        glVertexAttribPointer(v, 1, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        tex = ribbon_prog->getAttribute("vertTex");
        glEnableVertexAttribArray(tex);
        glBindBuffer(GL_ARRAY_BUFFER, Left_AnkleTextureID);
        glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        glDrawArrays(GL_TRIANGLES, 0, num_to_draw); // TODO: adding a time based amt here
        glDisableVertexAttribArray(h_pos);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(h_nor);
        glDisableVertexAttribArray(v);
        glDisableVertexAttribArray(tex);

    ribbon_prog->unbind();

    line_prog->bind();
    glUniformMatrix4fv(line_prog->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
    glUniformMatrix4fv(line_prog->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
    glUniformMatrix4fv(line_prog->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());

    //Draw waist lines
    h_pos = line_prog->getAttribute("vertPos");
    glEnableVertexAttribArray(h_pos);
    glBindBuffer(GL_ARRAY_BUFFER, full_waist_vertexbuffer);
    glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0); //function to get # of elements at a time

    glDrawArrays(GL_LINES, t * 2, 2); // TODO: adding a time based amt here
    glDisableVertexAttribArray(h_pos);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Draw shoulder lines
    h_pos = line_prog->getAttribute("vertPos");
    glEnableVertexAttribArray(h_pos);
    glBindBuffer(GL_ARRAY_BUFFER, shoulders_vertexbuffer);
    glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0); //function to get # of elements at a time

    glDrawArrays(GL_LINES, t * 2, 2); // TODO: adding a time based amt here
    glDisableVertexAttribArray(h_pos);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    line_prog->unbind();

    // Pop matrix stacks.
    M->popMatrix();
    P->popMatrix();

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

    glfwSetCursorPos(window, g_width/2, g_height/2);
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
