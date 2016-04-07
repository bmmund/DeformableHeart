#include "app.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>


using namespace std;
App* currentAppInstance = NULL;

App& App::getInstance()
{
    if (currentAppInstance)
        return *currentAppInstance;
    else
        throw std::runtime_error("There is now current Application");
}

void App::windowSizeCallback(GLFWwindow* window, int width, int height)
{
    getInstance().windowSizeCallbackImp(window, width, height);
}


void App::errorCallback(int error, const char* description)
{
    cerr<<description<<endl;;
}

void App::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    getInstance().keyCallbackImp(window, key, scancode, action, mods);
}

void App::mousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    getInstance().mousePositionCallbackImp(window, xpos, ypos);
}

void App::mouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
    getInstance().mouseClickCallbackImp(window, button, action, mods);
}

App::App( int _width, int _height, std::string _title ):
    width(_width),
    height(_height),
    title(_title)
{
    // Set as current app instance
    currentAppInstance = this;

    // set up error callback
    glfwSetErrorCallback(App::errorCallback);

    // initialize the GLFW
    if (!glfwInit())
    {
        throw std::runtime_error("Couldn't init GLFW");
    }

    #if defined(USE_OPENGL_LEGACY)
        glfwDefaultWindowHints();
    #else
        // set opengl to v 3.3 core
        int major = 3;
        int minor = 3;
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif
    // create the window
    window = glfwCreateWindow( width, height, title.c_str(), nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("GLFW init failed.");
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum error = glewInit();
    if( error != GLEW_OK)
    {
        glfwTerminate();
        throw std::runtime_error(string("GLEW init fialed:")
                                 + (const char*)glewGetErrorString(error));
    }

    // Setup GLFW callbacks
    glfwSetWindowSizeCallback(window, App::windowSizeCallback);
    glfwSetKeyCallback(window, App::keyCallback);
    glfwSetCursorPosCallback(window, App::mousePositionCallback);
    glfwSetMouseButtonCallback(window, App::mouseClickCallback);
    // opengl configuration
//    glEnable (GL_DEPTH_TEST); // enable depth-testing
//    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"

}

App::~App()
{

}

GLFWwindow* App::getWindow()
{
    return window;
}

void App::exit() {
    glfwSetWindowShouldClose(window, GL_TRUE);
}


float App::getFrameDeltaTime()
{
    return deltaTime;
}

float App::getTime()
{
    return time;
}

void App::run()
{

    //Make the window's context current
    glfwMakeContextCurrent(window);

    time = glfwGetTime();

    while(!glfwWindowShouldClose(window))
    {
        // compute new time and delta time
        float t = glfwGetTime();
        deltaTime = t - time;
        time = t;

        // Poll and process events
        glfwPollEvents();

        // rendering code
        loop();

        // Swap buffers
        glfwSwapBuffers(window);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}

void App::windowSizeCallbackImp(GLFWwindow* window, int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;
    glViewport(0, 0, width, height);
}

void App::keyCallbackImp(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void App::mousePositionCallbackImp(GLFWwindow* window, double xpos, double ypos)
{
}

void App::mouseClickCallbackImp(GLFWwindow* window, int button, int action, int mods)
{
}

void App::loop()
{
}

int App::getWidth()
{
    return width;
}

int App::getHeight()
{
    return height;
}

float App::getWindowRatio()
{
    return float(width)/float(height);
}
