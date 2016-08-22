#ifndef __ACM__HPP
#define __ACM__HPP

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <tuple>
#include <array>

typedef int VertexHandle;
typedef int CMapHandle;

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

struct CMap {
    std::vector<std::vector<VertexHandle>> cm;
    glm::vec3 colour;
    CMapHandle idx;
    int vectorScale;
    bool isPhantom;
    CMap()
    : idx(-1), vectorScale(1), isPhantom(false)
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
                        VertexHandle vh11,
                        bool isPhantom = false);
    void setFaceColour(CMapHandle cm_idx, glm::vec3 colour);
    std::vector<std::array<VertexHandle, 3>> getCMapFaces(CMapHandle cm_idx);
    void refine();
    void decompose();

private:
    std::vector<CMap> cm_list;
    std::vector<Vertex> v_list;
    std::map<int, std::vector<CMapHandle>> v_cm_map;
    void updateVertexCMapMap(const std::vector<VertexHandle>& verts, const CMapHandle cm_idx);
    void updateVertexCMapMap(const VertexHandle& verts, const CMapHandle cm_idx);
};


#endif /* __ACM__HPP */
