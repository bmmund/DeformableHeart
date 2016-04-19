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
    glm::mat4 projM;
    bool useNormal;
    bool useLighting;
//    glm::vec4 point;
//    glm::vec4 vect(0,0,-1,0.f);
    int selectedIdx;
    void setDefaultMaterial(void);
    void setHeartMaterial();
    void setDefaultLight(void);
    void initializeGL();
    void updateGeometries();
    void drawMesh();
    void drawMeshNormals();
    void drawMeshPoints();
    void drawMeshPoint(int index);
    void keyCallbackImp(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mousePositionCallbackImp(GLFWwindow* window, double xpos, double ypos);
    void mouseClickCallbackImp(GLFWwindow* window, int button, int action, int mods);
	void scrollCallbackImp(GLFWwindow* window, double xoffset, double yoffset);
    bool map_to_sphere( const glm::vec2& _v2D, OpenMesh::Vec3f& _v3D );
    glm::vec2        last_point_2D_;
    OpenMesh::Vec3f  last_point_3D_;
    bool             last_point_ok_;
};
#endif /* __DEFO_HEART__HPP */
