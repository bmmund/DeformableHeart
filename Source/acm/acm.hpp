#ifndef __ACM__HPP
#define __ACM__HPP

#include <OpenMesh/Core/IO/MeshIO.hh>   // must be included before a mesh type
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include <vector>

typedef OpenMesh::TriMesh_ArrayKernelT<>  TriMesh;

typedef struct VertexAttributes
{
    TriMesh::Point p;
    TriMesh::Color c;
    TriMesh::Normal n;
} Vertex;

typedef struct connectivityMap {
    TriMesh::EdgeHandle edgeKey;
    std::vector<std::vector<Vertex>> cm;
    float vectorScale;
    bool isPhantom;
} CMap;

#endif /* __ACM__HPP */
