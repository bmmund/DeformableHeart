#ifndef __HEART_MESH__HPP
#define __HEART_MESH__HPP

#include <string>
#include <OpenMesh/Core/IO/MeshIO.hh>   // must be included before a mesh type
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <vector>

typedef OpenMesh::TriMesh_ArrayKernelT<>  TriMesh;

class HeartMesh
{
public:
    HeartMesh(std::string filename);
    ~HeartMesh();
    TriMesh* getTriMesh(){return &mesh;}
    void updateFaceIndeces();
private:
    std::string fname;
    TriMesh mesh;
    std::vector<int> faceIndeces;
};
#endif /* __HEART_MESH__HPP */
