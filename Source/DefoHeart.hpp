#ifndef __DEFO_HEART__HPP
#define __DEFO_HEART__HPP

#include "app.hpp"
#include "HeartMesh.hpp"
#include "loop.hpp"
#include <glm/glm.hpp>
#include "trackball.hpp"
#include "rayselector.hpp"
#ifndef USE_OPENGL_LEGACY
    #include "shader.hpp"
#endif

class DefoHeart : public App
{
public:
    DefoHeart();
    ~DefoHeart();
    void loop();
private:
    #ifndef USE_OPENGL_LEGACY
        Shader shaderPhong;
		Shader shaderSolid;
        GLuint modelLoc;
        GLuint viewLoc;
        GLuint projLoc;
		GLuint colourLoc;
    #endif
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

#ifdef USE_OPENGL_LEGACY
    void updateGeometriesLegacy();
    void drawMesh();
    void drawMeshNormals();
    void drawMeshPoints();
    void drawMeshPoint(int index);
    void drawEvenOddPoints();
    void drawModelGnomenPoints();
    void setDefaultMaterial(void);
    void setHeartMaterial();
    void setDefaultLight(void);
#endif
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
