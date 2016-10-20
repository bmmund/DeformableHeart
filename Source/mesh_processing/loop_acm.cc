#include "loop_acm.hpp"
#include "utilities.hpp"
Loop::Loop()
{
    preComputeWeights(6);
}

Loop::~Loop()
{

}

void Loop::subdivide(ACM* acm)
{
    // prepare enough space
    acm->refine();
    // find vertex positions (even and odd per cm)
	setNewVertexPositions(acm);
    acm->reduceVectorScale();
    addDetails(acm);
    updateValues(acm);
}

void Loop::decompose(ACM* acm)
{
    setCoarseVertexPositions(acm);
    updateValues(acm);
    acm->increaseVectorScale();
    setCoarseEdgesDetails(acm);
    updateValues(acm);
}

void Loop::setNewVertexPositions(ACM * acm)
{
    ACM::CMapIter c_it;
    int vs;
    // for each connectivity map
    for (c_it = acm->cm_begin(); c_it != acm->cm_end(); c_it++)
    {
        vs = c_it->vectorScale/2;
        // for each even vertex
        for (int i = 0; i < c_it->cm.size(); i+=vs)
        {
            for (int j = 0; j < c_it->cm.at(i).size(); j+=vs)
            {
                CMapIndex index(i, j);
                if (Utilities::isEven(i/vs) && Utilities::isEven(j/vs))
                {
                    setVertexVertexPosition(acm, &(*c_it), index);
                }
                else
                {
                    setEdgeVertexPosition(acm, &(*c_it), index);
                }
            }
        }
    }
}

void Loop::setVertexVertexPosition(ACM* acm, CMap* cm, CMapIndex index)
{
    // determine new location
    VertexHandle vh = cm->cm.at(index.x).at(index.y);
    std::vector<VertexHandle> neighbours;
    neighbours = acm->getNeighbourhood(vh);
    int valence = neighbours.size();
    float alpha = getWeight(valence);

    // sum up neighbours
    // alpha * SUM(Vj)
    Vertex sum;
    Vertex* vptr;
    for (auto& neighbour : neighbours)
    {
        vptr = acm->getVertex(neighbour);
        sum += *vptr;
    }
    sum = alpha * sum;

    // add vertex contribution
    sum += (1 - (valence*alpha)) * (*acm->getVertex(vh));
    changes[vh] = sum;
}

void Loop::setEdgeVertexPosition(ACM* acm, CMap* cm, CMapIndex index)
{
	// do remaining odd verts
    CMapIndex ev1, ev2;
    int vs = cm->vectorScale/2;
    VertexHandle vh = cm->cm.at(index.x).at(index.y);
    // (odd, even) - horizontal
    if(Utilities::isOdd(index.x/vs) && Utilities::isEven(index.y/vs))
    {
        ev1 = CMapIndex(index.x-vs, index.y);
        ev2 = CMapIndex(index.x+vs, index.y);
    }
    // (even, odd) - vert
    else if(Utilities::isEven(index.x/vs) && Utilities::isOdd(index.y/vs))
    {
        ev1 = CMapIndex(index.x, index.y-vs);
        ev2 = CMapIndex(index.x, index.y+vs);
    }
    // (odd, odd) - diag
    else if(Utilities::isOdd(index.x/vs) && Utilities::isOdd(index.y/vs))
    {
        ev1 = CMapIndex(index.x-vs, index.y+vs);
        ev2 = CMapIndex(index.x+vs, index.y-vs);
    }
    else
    {
        return;
    }
    std::array<VertexHandle, 4> edgeNeighbours = acm->getEdgeNeighbours(cm->idx, ev1, ev2);

    Vertex edgeVert;
    Vertex* v1;
    Vertex* v2;
    Vertex* v3;
    Vertex* v4;

    v1 = acm->getVertex(edgeNeighbours.at(0));
    v2 = acm->getVertex(edgeNeighbours.at(1));
    v3 = acm->getVertex(edgeNeighbours.at(2));
    v4 = acm->getVertex(edgeNeighbours.at(3));
    // E` = (3/8)* [V1 + V2] + (1/8)*[V3 + V4]
    //            pos = (3.0/8.0)*v1v2 + (1.0/8.0)*v3v4;
    edgeVert = (3.0/8.0)*(*v1 + *v2) + (1.0/8.0)*(*v3 + *v4);
    changes[vh] = edgeVert;
}

void Loop::setCoarseVertexPositions(ACM * acm)
{
    ACM::CMapIter c_it;
    int vs;
    // for each connectivity map
    for (c_it = acm->cm_begin(); c_it != acm->cm_end(); c_it++)
    {
        vs = c_it->vectorScale*2;
        // for each even vertex
        for (int i = 0; i < c_it->cm.size(); i+=vs)
        {
            for (int j = 0; j < c_it->cm.at(i).size(); j+=vs)
            {
                CMapIndex index(i, j);
                setCoarseVertexVertexPosition(acm, &(*c_it), index);
            }
        }
    }
}


void Loop::setCoarseVertexVertexPosition(ACM* acm, CMap* cm, CMapIndex index)
{
    // determine new location
    VertexHandle vh = cm->cm.at(index.x).at(index.y);
    std::vector<VertexHandle> neighbours;
    neighbours = acm->getNeighbourhood(vh);
    int valence = neighbours.size();
    // Based on L. Olsen's work, they use beta for getWeight()
    // leaving alpha for a reverse-subdivision weight.
    // alpha =  8*beta/5
    float alpha = (8.0f*getWeight(valence))/5.0f;

    // sum up neighbours
    // alpha * SUM(Vj)
    Vertex pos;
    Vertex* vptr;
    for (auto& neighbour : neighbours)
    {
        vptr = acm->getVertex(neighbour);
        pos += *vptr;
    }
    pos = (alpha/(1.0f-valence*alpha)) * pos;
    // c0 = (1/(1-na))f0 - (a/(1-na))sum(fi)
    pos = ( (1.0f/(1.0f - valence*alpha)) * (*acm->getVertex(vh)) ) - pos;// - pos;
    changes[vh] = pos;
}

void Loop::setCoarseEdgesDetails(ACM *acm)
{
    ACM::CMapIter c_it;
    int vs;
    // for each connectivity map
    for (c_it = acm->cm_begin(); c_it != acm->cm_end(); c_it++)
    {
        vs = c_it->vectorScale/2;
        // for each even vertex
        for (int i = 0; i < c_it->cm.size(); i+=vs)
        {
            for (int j = 0; j < c_it->cm.at(i).size(); j+=vs)
            {
                CMapIndex index(i, j);
                if (!(Utilities::isEven(i/vs) && Utilities::isEven(j/vs)))
                {
                    setCoarseEdgeDetails(acm, &(*c_it), index);
                }
            }
        }
    }
}

void Loop::setCoarseEdgeDetails(ACM *acm, CMap* cm, CMapIndex index)
{
    // do remaining odd verts
    CMapIndex ev1, ev2;
    int vs = cm->vectorScale/2;
    VertexHandle vh = cm->cm.at(index.x).at(index.y);
    // (odd, even) - horizontal
    if(Utilities::isOdd(index.x/vs) && Utilities::isEven(index.y/vs))
    {
        ev1 = CMapIndex(index.x-vs, index.y);
        ev2 = CMapIndex(index.x+vs, index.y);
    }
    // (even, odd) - vert
    else if(Utilities::isEven(index.x/vs) && Utilities::isOdd(index.y/vs))
    {
        ev1 = CMapIndex(index.x, index.y-vs);
        ev2 = CMapIndex(index.x, index.y+vs);
    }
    // (odd, odd) - diag
    else if(Utilities::isOdd(index.x/vs) && Utilities::isOdd(index.y/vs))
    {
        ev1 = CMapIndex(index.x-vs, index.y+vs);
        ev2 = CMapIndex(index.x+vs, index.y-vs);
    }
    else
    {
        return;
    }
    std::array<VertexHandle, 4> edgeNeighbours = acm->getEdgeNeighbours(cm->idx, ev1, ev2);

    Vertex edgeVert;
    Vertex* v1;
    Vertex* v2;
    Vertex* v3;
    Vertex* v4;
    Vertex fi = *(acm->getVertex(vh));
    v1 = acm->getVertex(edgeNeighbours.at(0));
    v2 = acm->getVertex(edgeNeighbours.at(1));
    v3 = acm->getVertex(edgeNeighbours.at(2));
    v4 = acm->getVertex(edgeNeighbours.at(3));
    // E` = (3/8)* [V1 + V2] + (1/8)*[V3 + V4]
    //            pos = (3.0/8.0)*v1v2 + (1.0/8.0)*v3v4;
    edgeVert = (3.0/8.0)*(*v1 + *v2) + (1.0/8.0)*(*v3 + *v4);
    changes[vh] = fi - edgeVert;
    // di = fi - fi'
    //                    TriMesh::Point fi, fi_hat;
    //                    fi = mesh->point(*vertIter);
    //                    fi_hat = (3.0f / 8.0f)*v1v2 + (1.0f / 8.0f)*v3v4;
    //                    pos = fi - fi_hat;
}

float Loop::getWeight(int valence)
{
    // this may be unecessary
    if(weights.find(valence) == weights.end()) // not found
    {
        // weight = (1/n) * ( (5/8) - ((3/8) + ((1/4)*cos(2*pi/n))^2)
        float weight;
        weight = cos(2.0f*M_PI/valence);
        weight = (1.0f/4.0f) * weight;
        weight = (3.0f/8.0f) + weight;
        weight = weight * weight;
        weight = (5.0f/8.0f) - weight;
        weight = (1.0f/valence) * weight;
        //create new entry
        weights[valence] = weight;
    }
    return weights[valence];
}

void Loop::preComputeWeights(int maxValence)
{
    for(int i = 1; i <= maxValence; i++)
    {
        getWeight(i);
    }
}

void Loop::updateValues(ACM* acm)
{
    std::map<VertexHandle, Vertex>::iterator m_it;
    for(m_it = changes.begin(); m_it != changes.end(); m_it++)
    {
        (*acm->getVertex(m_it->first)) = m_it->second;
    }
    changes.clear();
}

void Loop::addDetails(ACM* acm)
{
    ACM::CMapIter c_it;
    int vs;
    // for each connectivity map
    for (c_it = acm->cm_begin(); c_it != acm->cm_end(); c_it++)
    {
        vs = c_it->vectorScale;
        // for each even vertex
        for (int i = 0; i < c_it->cm.size(); i+=vs)
        {
            for (int j = 0; j < c_it->cm.at(i).size(); j+=vs)
            {
                CMapIndex index(i, j);
                VertexHandle vh = c_it->cm.at(index.x).at(index.y);
                if (Utilities::isEven(i/vs) && Utilities::isEven(j/vs))
                {
                    // determine new location
                    std::vector<VertexHandle> neighbours;
                    neighbours = acm->getNeighbourhood(vh);
                    int valence = neighbours.size();
                    float alpha = getWeight(valence);

                    // sum up neighbours
                    // alpha * SUM(Vj)
                    Vertex sumDetails;
                    Vertex* vptr;
                    for (auto& neighbour : neighbours)
                    {
                        vptr = acm->getVertex(neighbour);
                        sumDetails += *vptr;
                    }
                    sumDetails = ((8.0f*alpha) / 5.0f) * sumDetails;
                    changes[vh] += sumDetails;
                }
                else
                {
                    // details are stored in the odd verts
                    changes[vh] += *(acm->getVertex(vh));
                }
            }
        }
    }
}
