#include "acm.hpp"


ACM::ACM()
{

}

void ACM::add(const CMap &cm)
{
    cm_list.push_back(cm);
    for(auto& vertices : cm_list.back().cm)
    {
        for(auto& vertex : vertices)
        {
            v_list.push_back(&vertex);
        }
    }
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
