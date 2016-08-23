#ifndef __DEFO_HEART__HPP
#define __DEFO_HEART__HPP

#include "app.hpp"
#include "HeartMesh.hpp"
#include "loop_acm.hpp"
#include <glm/glm.hpp>
#include "trackball.hpp"
#include "rayselector.hpp"
#include "shader.hpp"

class DefoHeart : public App
{
public:
    DefoHeart();
    ~DefoHeart();
    void loop();
private:
    Shader shaderPhong;
    Shader shaderSolid;
    GLuint modelLoc;
    GLuint viewLoc;
    GLuint projLoc;
    GLuint colourLoc;
    HeartMesh mesh;
    Loop subDivider;
    Trackball trackball;
    RaySelector rayselector;
    glm::mat4 modelM;
    glm::mat4 viewM;
    glm::mat4 projM;
    glm::vec3 cameraOrigin;
    int selectedIdx;

    bool useNormal;
    bool useLighting;
    bool drawWireFrame;
    void initializeGL();
    void resetModelView();
    void resize();
    void keyCallbackImp(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mousePositionCallbackImp(GLFWwindow* window, double xpos, double ypos);
    void mouseClickCallbackImp(GLFWwindow* window, int button, int action, int mods);
	void scrollCallbackImp(GLFWwindow* window, double xoffset, double yoffset);
	void getAtterLocations(Shader &s);
};
#endif /* __DEFO_HEART__HPP */
