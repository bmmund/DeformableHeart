#include "loop_acm.hpp"

Loop::Loop()
{

}

Loop::~Loop()
{

}

void Loop::subdivide(ACM* acm)
{
    // prepare enough space
    acm->refine();
}

void Loop::decompose(ACM* acm)
{
    acm->decompose();
}