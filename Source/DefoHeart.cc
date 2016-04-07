#include "DefoHeart.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

int const windowWidth = 640;
int const windowHeight = 480;
std::string const application_name = "DefoHeart";

DefoHeart::DefoHeart()
    : App(windowWidth, windowHeight, application_name)
{
    
}

DefoHeart::~DefoHeart()
{

}

void DefoHeart::loop()
{
    // Clear the frame buffer
    glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    #if defined(USE_OPENGL_LEGACY)
        updateGeometries();
    #else
    #endif
}

void DefoHeart::keyCallbackImp(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void DefoHeart::mousePositionCallbackImp(GLFWwindow* window, double xpos, double ypos)
{
}

void DefoHeart::mouseClickCallbackImp(GLFWwindow* window, int button, int action, int mods)
{
}

void DefoHeart::updateGeometries()
{
    float ratio;
    int width, height;
    width = getWidth();
    height = getHeight();
    ratio = width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);

    glBegin(GL_TRIANGLES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(-0.6f, -0.4f, 0.f);
    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(0.6f, -0.4f, 0.f);
    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(0.f, 0.6f, 0.f);
    glEnd();
    glFlush();
}