#ifndef __HEART_MESH__HPP
#define __HEART_MESH__HPP

#include <string>
#include <OpenMesh/Core/IO/MeshIO.hh>   // must be included before a mesh type
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#ifndef USE_OPENGL_LEGACY
    #include "glad/glad.h"
    #include <GLFW/glfw3.h>
#endif // !USE_OPENGL_LEGACY

#include <vector>

typedef OpenMesh::TriMesh_ArrayKernelT<>  TriMesh;

class HeartMesh
{
public:
    HeartMesh(std::string filename);
    ~HeartMesh();
    TriMesh* getTriMesh(){return &mesh;}
    void updateFaceIndeces();
    #ifndef USE_OPENGL_LEGACY
        void Draw();
    #endif
private:
    #ifndef USE_OPENGL_LEGACY
        GLuint VAO, VBO, EBO;
        void createBuffers();
        void updateBuffers();
    #endif
    std::string fname;
    TriMesh mesh;
    std::vector<int> faceIndeces;
    std::vector<TriMesh::Point> points;
    std::vector<TriMesh::Normal> normals;
    std::vector<TriMesh::Color> colours;
    std::vector<TriMesh::EdgeHandle> pairedTrisEH;
    std::vector<int> pairedTrisFH;

    void initFaceColours(TriMesh::Color c);
    void initializeACM();
};
#endif /* __HEART_MESH__HPP */
