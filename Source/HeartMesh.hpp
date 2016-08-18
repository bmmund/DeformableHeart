#ifndef __HEART_MESH__HPP
#define __HEART_MESH__HPP

#include <string>
#include <OpenMesh/Core/IO/MeshIO.hh>   // must be included before a mesh type
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include "acm.hpp"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <random>
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
    std::mt19937 rng;
    std::uniform_int_distribution<int> gen;
    std::string fname;
    TriMesh mesh;
    std::vector<int> faceIndeces;
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> colours;
    std::vector<int> pairedTrisFH;
    ACM acm;

    GLuint VAO, VBO, EBO;
    void createBuffers();
    void updateBuffers();
    void initFaceColours(TriMesh::Color c);
    void initializeACM();
    void initializeACMVerts();
    void createCMFromEdge(const TriMesh::EdgeHandle& edge);
    void createPhantomCMFromEdge(const TriMesh::HalfedgeHandle& heh);
    int createACMVertex(const TriMesh::VertexHandle triMeshVert);
    int createACMVertex(const TriMesh::VertexHandle triMeshVert, glm::vec3 colour);
    inline glm::vec3 pointToVec3(OpenMesh::Vec3f p){return glm::vec3(p[0], p[1], p[2]);}
    void getRandomRGB(int& r, int& g, int& b);
    glm::vec3 getRandomRGBF();
};
#endif /* __HEART_MESH__HPP */
