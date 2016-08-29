#ifndef __ACM__HPP
#define __ACM__HPP

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <tuple>
#include <array>

typedef int VertexHandle;
typedef int CMapHandle;
typedef int NeighbourNumber;
typedef glm::uvec2 CMapIndex;

// what does i,j translate to
enum class CMapOrientation{
    posi_posj, // same
    posj_negi,
    negi_negj,
    negj_posi,
    invalid = 1
};

struct Vertex {
    glm::vec3 point;
    glm::vec3 colour;
    glm::vec3 normal;
    VertexHandle idx;
    Vertex()
        : point(0), colour(0), normal(0), idx(-1){}
    Vertex(glm::vec3 p, glm::vec3 c, glm::vec3 n)
        : point(p), colour(c), normal(c), idx(-1){}
    Vertex(const Vertex& v)
        : point(v.point), colour(v.colour), normal(v.normal), idx(v.idx){}
};

struct CMapNeighbour {
    CMapHandle cmh_pair;
    NeighbourNumber n;
    CMapOrientation o;
    CMapNeighbour()
    : cmh_pair(-1), n(-1), o(CMapOrientation::invalid) {}
};

struct CMap {
    std::vector<std::vector<VertexHandle>> cm;
    std::array<CMapNeighbour, 4> boundaryCMs;
    glm::vec3 colour;
    CMapHandle idx;
    int vectorScale;
    CMap()
    : idx(-1), vectorScale(1)
    {
    }
};

class ACM {
public:
    ACM();
    std::vector<CMap>& connectivityMaps();
    void add(const CMap& cm);
    void clear();
    std::vector<Vertex>::iterator v_begin(){return v_list.begin();}
    std::vector<Vertex>::iterator v_end(){return v_list.end();}
    CMapHandle addCmap();
    VertexHandle addVertex();
    VertexHandle addVertex(Vertex new_vertex);
    Vertex* getVertex(VertexHandle idx);
    CMap* getCMap(CMapHandle idx);
    void addVertsToCMap(CMapHandle cm_idx,
                        VertexHandle vh00,
                        VertexHandle vh01,
                        VertexHandle vh10,
                        VertexHandle vh11);
    void setFaceColour(CMapHandle cm_idx, glm::vec3 colour);
    std::vector<std::array<VertexHandle, 3>> getCMapFaces(CMapHandle cm_idx);
    void updateCMapNeighbours();
    CMapHandle getCMapForVertex(VertexHandle vh);
    void refine();
    void decompose();

private:
    std::vector<CMap> cm_list;
    std::vector<Vertex> v_list;
    std::map<VertexHandle, CMapHandle> v_cm_map;
    void updateVertexCMapMap(const std::vector<VertexHandle>& verts, const CMapHandle cm_idx);
    void updateVertexCMapMap(VertexHandle vert, const CMapHandle cm_idx);
    CMapNeighbour getCommanCMap(CMapHandle cmh,
                                  CMapIndex vh1_coords,
                                  CMapIndex vh2_coords);
    bool areEdgePointsEqual(const CMapIndex& e1v1,
                            const CMapIndex& e1v2,
                            const CMapIndex& e2v1,
                            const CMapIndex& e2v2);
    NeighbourNumber getNeighbourNumber(const CMapIndex& v1, const CMapIndex& v2);
    CMapOrientation getNeighbourCase(
                                     int neighbourNum,
                                     const CMapIndex& v1,
                                     const CMapIndex& v2,
                                     const CMapIndex& v1_pair,
                                     const CMapIndex& v2_pair
                                     );
    CMapOrientation getNeighbourCaseFor0(const CMapIndex& v1_pair,
                             const CMapIndex& v2_pair);
    CMapOrientation getNeighbourCaseFor1(const CMapIndex& v1_pair,
                             const CMapIndex& v2_pair);
    CMapOrientation getNeighbourCaseFor2(const CMapIndex& v1_pair,
                             const CMapIndex& v2_pair);
    CMapOrientation getNeighbourCaseFor3(const CMapIndex& v1_pair,
                             const CMapIndex& v2_pair);
};


#endif /* __ACM__HPP */
