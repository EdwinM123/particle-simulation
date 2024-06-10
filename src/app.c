#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "include/glew.h"
#include "include/glfw3.h"

/*
#include "graphics.h"
#include "shader.h"
#include "model.h"
#include "verlet.h"
#include "camera.h"
#include "peripheral.h"
*/
#define ANIMATION_TIME 90.0f 
#define ADDITION_SPEED 10
#define TARGET_FPS 60
#define NUM_SUBSTEPS 8

//functions 
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cursor_enter_callback(GLFWwindow* window, int entered);
void processInput(GLFWwindow* window);
void updateCamera(GLFWwindow* window, Mouse* mouse, Camera* camera);
void instantiateVerlets(VerletObject* objects, int size);

const unsigned int SCR_WIDTH=1280;
const unsigned int SCR_HEIGHT=720;

bool cursorEntered = false; 
Camera* camera; 
float cameraRadius = 24.0f;
int totalFrames = 0; 

int main(){
    GLFWwindow* window; 
    if(!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
#endif 

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Simulation", NULL, NULL);
    if(!window){
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);

    glfwSetCursorEnterCallback(window, cursor_enter_callback);

    glewInit();

    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClearStencil(0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); 
    glFrontFace(GL_CCW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPointSize(3.0);

    unsigned int phongShader = createShader("shaders/phong_vertext.glsl", "shaders/phong_fragment.glsl");
    unsigned int instanceShader=createShader("shaders/instance_vertex.glsl", "shaders/instance_fragment.glsl");
    unsigned int baseShader = createShader("shaders/base_vertex.glsl", "shaders/base_fragment.glsl");

    Mesh* mesh = createMesh("Models/icosphere.obj", true);
    Mesh* cubeMesh = createMesh("Models/Ball.obj", false);

    mfloat_t containerPosition[VEC3_SIZE] = {0, 0, 0};
    mfloat_t rotation[VEC3_SIZE]={0, 0, 0};

    VerletObject* verlets = malloc(sizeof(VerletObject)*MAX_INSTANCES);
    instantiateVerlets(verlets, MAX_INSTANCES);
    int numActive=0;

    mfloat_t view[MAT4_SIZE]; 
    camera = createCamera((mfloat_t[]) {0, 0, cameraRadius});

    Mouse* mouse = createMouse();

    float dt = 0.000001;
    float lastFrameTime = (float)glfwGetTime();

    char title[100]="";

    srand(time(NULL));

    while(!glfwWindowShouldClose(window)){
        updateMouse(window, mouse);
        processInput(window);

        if(glfwGetKey(window, GLFW_KEY_G)==GLFW_PRESS){
            addForce(verlets, numActive, (mfloat_t[]){0, 3, 0}, -30.0f*NUM_SUBStEPS);
        }

        updateCamera(window, mouse, camera);
        createViewMatrix(view, camera);

        glUseProgram(phongShader);
        glUniformMatrix4fv(glGetUniformLocation(phongShader, "view"), 
            1, GL_FALSE, view); 
    } 
}