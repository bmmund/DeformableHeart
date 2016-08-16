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

class ACM {
public:
    ACM();
    std::vector<CMap>& connectivityMaps();
    void add(const CMap& cm);
    void clear();
    std::vector<Vertex*>::iterator v_begin(){return v_list.begin();}
    std::vector<Vertex*>::iterator v_end(){return v_list.end();}

private:
    std::vector<CMap> cm_list;
    std::vector<Vertex*> v_list;
};


#endif /* __ACM__HPP */
