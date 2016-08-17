#include "acm.hpp"

ACM::ACM()
{

}

void ACM::add(const CMap &cm)
{
    cm_list.push_back(cm);
}

std::vector<CMap>& ACM::connectivityMaps()
{
    return cm_list;
}

void ACM::clear()
{
    cm_list.clear();
    v_list.clear();
}

CMap& ACM::add_cmap()
{
    CMap cm;
    cm_list.push_back(cm);
    return cm_list.back();
}

int ACM::add_vertex()
{
    Vertex vert;
    return add_vertex(vert);
}

int ACM::add_vertex(Vertex new_vertex)
{
    new_vertex.idx = v_list.size();
    v_list.push_back(new_vertex);
    return v_list.back().idx;
}

Vertex* ACM::getVertex(int idx)
{
    return &v_list.at(idx);
}
