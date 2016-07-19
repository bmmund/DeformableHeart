#include "app.hpp"

#ifndef USE_OPENGL_LEGACY
    #include "glad/glad.h"
#endif // !USE_OPENGL_LEGACY

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

void App::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	getInstance().scrollCallbackImp(window, xoffset, yoffset);
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

    // Setup for opengl
    glfwDefaultWindowHints();

    #ifndef USE_OPENGL_LEGACY
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    #endif
        // create the window
    window = glfwCreateWindow( width, height, title.c_str(), nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("GLFW init failed.");
    }
    glfwGetFramebufferSize(window, &pixelsX, &pixelsY);    // update width/height
    glfwMakeContextCurrent(window);

#ifndef USE_OPENGL_LEGACY
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
#endif


    // Setup GLFW callbacks
    glfwSetWindowSizeCallback(window, App::windowSizeCallback);
    glfwSetKeyCallback(window, App::keyCallback);
    glfwSetCursorPosCallback(window, App::mousePositionCallback);
    glfwSetMouseButtonCallback(window, App::mouseClickCallback);
	glfwSetScrollCallback(window, App::scrollCallback);
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
    glfwGetFramebufferSize(window, &pixelsX, &pixelsY); // update sizes with framebuffer
    width = newWidth;
    height = newHeight;
    resize();
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

void App::scrollCallbackImp(GLFWwindow* window, double xoffset, double yoffset)
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

int App::getPixelWidth()
{
    return pixelsX;
}

int App::getPixelHeight()
{
    return pixelsY;
}

float App::getWindowRatio()
{
    return float(width)/float(height);
}
