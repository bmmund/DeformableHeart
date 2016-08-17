#ifndef __ACM__HPP
#define __ACM__HPP

#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 point;
    glm::vec3 colour;
    glm::vec3 normal;
    int idx;
    Vertex()
    : point(0), colour(0), normal(0), idx(-1)
    {
    }
    Vertex(glm::vec3 p, glm::vec3 c, glm::vec3 n)
    : point(p), colour(c), normal(c), idx(-1)
    {
    }
};

typedef struct connectivityMap {
    std::vector<std::vector<int>> cm;
    glm::vec3 colour;
    float vectorScale;
    bool isPhantom;
} CMap;

class ACM {
public:
    ACM();
    std::vector<CMap>& connectivityMaps();
    void add(const CMap& cm);
    void clear();
    std::vector<Vertex>::iterator v_begin(){return v_list.begin();}
    std::vector<Vertex>::iterator v_end(){return v_list.end();}
    CMap& add_cmap();
    int add_vertex();
    int add_vertex(Vertex new_vertex);
    Vertex* getVertex(int idx);

private:
    std::vector<CMap> cm_list;
    std::vector<Vertex> v_list;
};


#endif /* __ACM__HPP */
