#ifndef __HEART_MESH__HPP
#define __HEART_MESH__HPP

#include <string>
#include <OpenMesh/Core/IO/MeshIO.hh>   // must be included before a mesh type
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include "acm.hpp"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>

typedef OpenMesh::TriMesh_ArrayKernelT<>  TriMesh;


class HeartMesh
{
public:
    HeartMesh(std::string filename);
    ~HeartMesh();
    TriMesh* getTriMesh(){return &mesh;}
    void updateFaceIndeces();
    void Draw();
private:
    std::string fname;
    TriMesh mesh;
    std::vector<int> faceIndeces;
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> colours;
    std::vector<TriMesh::EdgeHandle> pairedTrisEH;
    std::vector<int> pairedTrisFH;
    ACM acm;

    GLuint VAO, VBO, EBO;
    void createBuffers();
    void updateBuffers();
    void initFaceColours(TriMesh::Color c);
    void initializeACM();
    void createCMFromEdge(const TriMesh::EdgeHandle& edge, CMap& output);
    void createPhantomCMFromEdge(const TriMesh::HalfedgeHandle& heh, CMap& output);
    inline glm::vec3 pointToVec3(OpenMesh::Vec3f p){return glm::vec3(p[0], p[1], p[2]);}
};
#endif /* __HEART_MESH__HPP */
