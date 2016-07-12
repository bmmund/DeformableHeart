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
<<<<<<< Updated upstream
=======
    void setupMeshProperties(TriMesh * mesh);
    void decompose(TriMesh* mesh);
    int getSubDivisionDepth(TriMesh* mesh);
    bool isVertEven(TriMesh* mesh, TriMesh::VertexHandle vh);
>>>>>>> Stashed changes

private:
    OpenMesh::VPropHandleT<TriMesh::Point> vertPoint;
    OpenMesh::EPropHandleT<TriMesh::Point> edgePoint;
    OpenMesh::VPropHandleT<bool> isEvenVertex;
	OpenMesh::MPropHandleT<int> subdevDepth;    // Refers to how many iterations of subdivision performed

    const char* vv_prop_name = "vv_prop";
    const char* ev_prop_name = "ev_prop";
    const char* v_prop_even_odd_name = "v_prop_even_odd";
    const char* m_prop_subdev_depth_name = "m_prop_subdev_depth";

    void setupMeshProperties(TriMesh* mesh);
    void teardownMeshProperties(TriMesh* mesh);
    void setVertexVertexPositions(TriMesh* mesh);
    void setEdgeVertexPositions(TriMesh* mesh);
    void splitEdges(TriMesh* mesh);
    void splitEdge(TriMesh* mesh, const TriMesh::EdgeHandle& _eh);
    void splitFaces(TriMesh* mesh);
    void splitCorner(TriMesh* mesh, TriMesh::HalfedgeHandle* he);
    float getWeight(int valence);
    void updateGeometries(TriMesh* mesh);

<<<<<<< Updated upstream
=======
    // decomposition
    void findCoarseEvenPositions(TriMesh* mesh);
    void removeOddVertices(TriMesh* mesh);
    void unsplitFaces(TriMesh* mesh);
    bool doesHalfEdgeContainOddOnly(TriMesh * mesh, const OpenMesh::HalfedgeHandle &heh);
    void unsplitCorner(TriMesh* mesh, TriMesh::HalfedgeHandle heh);
>>>>>>> Stashed changes
};
#endif /* __LOOP__HPP */
