#ifndef __RAY_SELECTOR__HPP
#define __RAY_SELECTOR__HPP

#include <OpenMesh/Core/IO/MeshIO.hh>   // must be included before a mesh type
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <glm/glm.hpp>

typedef OpenMesh::TriMesh_ArrayKernelT<>  TriMesh;

class RaySelector
{
public:
    RaySelector(
                int width,
                int height,
                glm::mat4* projection,
                glm::mat4* view,
                glm::mat4* model,
                glm::vec3* eye);
    void setWindow(int width, int height);
    int getVertexIndex(TriMesh* mesh, glm::vec2 pixelCords);
private:
    int screenWidth;
    int screenHeight;
    glm::mat4* projM;
    glm::mat4* viewM;
    glm::mat4* modelM;
    glm::vec3* cameraOrigin;
};
#endif /* __RAY_SELECTOR__HPP */
