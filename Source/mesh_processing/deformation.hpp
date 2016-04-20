#ifndef __DEFORMATION__HPP
#define __DEFORMATION__HPP

#include <OpenMesh/Core/IO/MeshIO.hh>   // must be included before a mesh type
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

typedef OpenMesh::TriMesh_ArrayKernelT<>  TriMesh;

namespace Deformation {
    void pushVertsOut(TriMesh* mesh, int index, float amount);
    void pushVertsIn(TriMesh* mesh, int index, float amount);
}
#endif /* __DEFORMATION__HPP */
