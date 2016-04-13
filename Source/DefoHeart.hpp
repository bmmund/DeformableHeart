#ifndef __DEFO_HEART__HPP
#define __DEFO_HEART__HPP

#include "app.hpp"
#include "HeartMesh.hpp"
#include "loop.hpp"
#include <glm/glm.hpp>

class DefoHeart : public App
{
public:
    DefoHeart();
    ~DefoHeart();
    void loop();
private:
    HeartMesh mesh;
    Loop subDivider;
    glm::mat4 modelM;
    void initializeGL();
    void updateGeometries();
    void drawMesh();
    void drawMeshPoints();
    void drawMeshPoint(int index);
    void keyCallbackImp(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mousePositionCallbackImp(GLFWwindow* window, double xpos, double ypos);
    void mouseClickCallbackImp(GLFWwindow* window, int button, int action, int mods);
	void scrollCallbackImp(GLFWwindow* window, double xoffset, double yoffset);
};
#endif /* __DEFO_HEART__HPP */
