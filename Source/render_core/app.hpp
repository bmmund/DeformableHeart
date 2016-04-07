#ifndef __APP__HPP
#define __APP__HPP

#include <string>

class GLFWwindow;

class App
{
public:

    App(int _width = 640, int _height = 480, std::string _title = "Application");
    virtual ~App();

    // Get current applicaiton instance
    static App& getInstance();

    // get the window id
    GLFWwindow* getWindow();

    // window control
    void exit();

    // delta time between frame and time from beginning
    float getFrameDeltaTime();
    float getTime();

    // application run
    void run();

    // Application informations
    int getWidth();
    int getHeight();
    float getWindowRatio();
    bool windowDimensionChange();
    static void windowSizeCallback(GLFWwindow* window, int width, int height);
    static void errorCallback(int error, const char* description);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mousePositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseClickCallback(GLFWwindow* window, int button, int action, int mods);

private:

    App& operator=(const App&) {return *this;}

    GLFWwindow* window;
    float time;
    float deltaTime;

    int width;
    int height;

protected:

    App(const App&) {};

    std::string title;

    virtual void loop();
    virtual void windowSizeCallbackImp(GLFWwindow* window, int width, int height);
    virtual void keyCallbackImp(GLFWwindow* window, int key, int scancode, int action, int mods);
    virtual void mousePositionCallbackImp(GLFWwindow* window, double xpos, double ypos);
    virtual void mouseClickCallbackImp(GLFWwindow* window, int button, int action, int mods);


};

#endif /* __APP__HPP */
