#ifndef __DEFORMATION__HPP
#define __DEFORMATION__HPP

#include <OpenMesh/Core/IO/MeshIO.hh>   // must be included before a mesh type
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

typedef OpenMesh::PolyMesh_ArrayKernelT<>  QuadMesh;

namespace Deformation {
    void pushVertsOut(QuadMesh* mesh, int index, float amount);
    void pushVertsIn(QuadMesh* mesh, int index, float amount);
}
#endif /* __DEFORMATION__HPP */
