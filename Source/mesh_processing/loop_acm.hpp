#ifndef __LOOP_ACM__HPP
#define __LOOP_ACM__HPP

#include "acm.hpp"
#include <map>

class Loop
{
public:
    Loop();
    ~Loop();
    void subdivide(ACM* acm);
    void decompose(ACM* acm);

private:
    std::map<int, float> weights; // valence, weight
    std::map<VertexHandle, Vertex> changes;
    void setNewVertexPositions(ACM* acm);
    void setVertexVertexPosition(ACM* acm, CMap* cm, CMapIndex index);
    void setEdgeVertexPosition(ACM* acm, CMap* cm, CMapIndex index);

    void setCoarseVertexPositions(ACM* acm);
    void setCoarseVertexVertexPosition(ACM* acm, CMap* cm, CMapIndex index);
    void setCoarseEdgesDetails(ACM* acm);
    void setCoarseEdgeDetails(ACM *acm, CMap* cm, CMapIndex index);

    float getWeight(int valence);
    void preComputeWeights(int maxValence);
    void updateValues(ACM* acm);
    void addDetails(ACM* acm);
};

#endif /* __LOOP_ACM__HPP */
