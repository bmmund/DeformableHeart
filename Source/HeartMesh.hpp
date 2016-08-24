#ifndef __HEART_MESH__HPP
#define __HEART_MESH__HPP

#include <string>
#include <OpenMesh/Core/IO/MeshIO.hh>   // must be included before a mesh type
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "acm.hpp"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <random>
#include <vector>

typedef OpenMesh::PolyMesh_ArrayKernelT<>  QuadMesh;


class HeartMesh
{
public:
    HeartMesh(std::string filename);
    ~HeartMesh();
    QuadMesh* getQuadMesh(){return &mesh;}
    ACM* getACM(){return &acm;}
    void updateFaceIndeces();
    void Draw();
private:
    std::mt19937 rng;
    std::uniform_int_distribution<int> gen;
    std::string fname;
    QuadMesh mesh;
    std::vector<int> faceIndeces;
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> colours;
    std::vector<int> pairedTrisFH;
    ACM acm;

    GLuint VAO, VBO, EBO;
    void createBuffers();
    void updateBuffers();
    void initFaceColours(QuadMesh::Color c);
    void initializeACM();

    void initializeACMVerts();
    int createACMVertex(const QuadMesh::VertexHandle triMeshVert);
    int createACMVertex(const QuadMesh::VertexHandle triMeshVert, glm::vec3 colour);

    inline glm::vec3 pointToVec3(OpenMesh::Vec3f p){return glm::vec3(p[0], p[1], p[2]);}
    void getRandomRGB(int& r, int& g, int& b);
    glm::vec3 getRandomRGBF();
};
#endif /* __HEART_MESH__HPP */
