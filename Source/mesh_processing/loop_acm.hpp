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
    void setVertexVertexPositions(ACM* acm);
    void setEdgeVertexPositions(ACM* acm);

    float getWeight(int valence);
    void preComputeWeights(int maxValence);
    void updateValues(ACM* acm);
};

#endif /* __LOOP_ACM__HPP */
