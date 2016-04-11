#ifndef __LOOP__HPP
#define __LOOP__HPP

#include <OpenMesh/Core/IO/MeshIO.hh>   // must be included before a mesh type
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

typedef OpenMesh::TriMesh_ArrayKernelT<>  TriMesh;

class Loop
{
public:
    Loop();
    ~Loop();
    void subdivide(TriMesh* mesh);

private:
    OpenMesh::VPropHandleT<TriMesh::Point> vertPoint;
    OpenMesh::EPropHandleT<TriMesh::Point> edgePoint;

    void setupMeshProperties(TriMesh* mesh);
    void teardownMeshProperties(TriMesh* mesh);
    void setVertexVertexPositions(TriMesh* mesh);
    float getWeight(int valence);
    void updateGeometries(TriMesh* mesh);

};
#endif /* __LOOP__HPP */
