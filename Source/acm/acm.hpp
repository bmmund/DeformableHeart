#ifndef __ACM__HPP
#define __ACM__HPP

#include <glm/glm.hpp>
#include <vector>

typedef struct VertexAttributes
{
    glm::vec3 point;
    glm::vec3 colour;
    glm::vec3 normal;
} Vertex;

typedef struct connectivityMap {
    std::vector<std::vector<Vertex>> cm;
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
    std::vector<Vertex*>::iterator v_begin(){return v_list.begin();}
    std::vector<Vertex*>::iterator v_end(){return v_list.end();}

private:
    std::vector<CMap> cm_list;
    std::vector<Vertex*> v_list;
};


#endif /* __ACM__HPP */
