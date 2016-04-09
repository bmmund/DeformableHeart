#ifndef __HEART_MESH__HPP
#define __HEART_MESH__HPP

#include <string>
#include <OpenMesh/Core/IO/MeshIO.hh>   // must be included before a mesh type
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

typedef OpenMesh::TriMesh_ArrayKernelT<>  TriMesh;

class HeartMesh
{
public:
    HeartMesh(std::string filename);
    ~HeartMesh();
private:
    std::string fname;
    TriMesh mesh;
};
#endif /* __HEART_MESH__HPP */
