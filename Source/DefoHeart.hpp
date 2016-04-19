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
    glm::mat4 viewM;
    glm::mat4 projM;
    glm::vec3 cameraOrigin;
    bool useNormal;
    bool useLighting;

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
    void drawModelGnomenPoints();
    bool intersect(glm::vec4 &ray_dir, glm::vec4 &ray_origin, glm::vec4 &center, float &t_out);
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
