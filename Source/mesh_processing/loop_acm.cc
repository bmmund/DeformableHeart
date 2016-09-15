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

    //setEdgeVertexPositions(acm);

    updateValues(acm);
    acm->reduceVectorScale();
//    ACM::VertexIter v_it = acm->v_begin();
//    for(; v_it !=acm->v_end(); v_it++)
//    {
//        std::vector<VertexHandle> verts = acm->getNeighbourhood(v_it->idx);
//        printf("vert %d has neighbours:\n", v_it->idx);
//        for(const auto& vert : verts)
//        {
//            printf("\tvert %d\n", vert);
//        }
//    }
}

void Loop::decompose(ACM* acm)
{
    acm->decompose();
}

void Loop::setNewVertexPositions(ACM * acm)
{
    ACM::CMapIter c_it;
    // for each connectivity map
    for (c_it = acm->cm_begin(); c_it != acm->cm_end(); c_it++)
    {
        // for each even vertex
        for (int i = 0; i < c_it->cm.size(); i++)
        {
            for (int j = 0; j < c_it->cm.at(i).size(); j++)
            {
                CMapIndex index(i, j);
                if (Utilities::isEven(i) && Utilities::isEven(j))
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
    for (auto& neighbour : neighbours)
    {
        sum += *(acm->getVertex(neighbour));
    }
    sum = alpha * sum;

    // add vertex contribution
    sum += (1 - (valence*alpha)) * (*acm->getVertex(vh));
    changes[vh] = sum;
    //                pos += (1 - (valence*alpha) ) * mesh->point(*vertIter);
    //                for (TriMesh::VertexEdgeIter ve_it = mesh->ve_iter(*vertIter);
    //                     ve_it.is_valid(); ++ve_it)
    //                {
    //                    details += mesh->property(detailsProp, *ve_it);
    //                }
    //                details = ((8.0f*alpha) / 5.0f) * details;
}

void Loop::setEdgeVertexPosition(ACM* acm, CMap* cm, CMapIndex index)
{
	// do all odd verts on boundaries

	// do remaining odd verts
    CMapIndex ev1, ev2;
    VertexHandle vh = cm->cm.at(index.x).at(index.y);
    // (odd, even) - horizontal
    if(Utilities::isOdd(index.x) && Utilities::isEven(index.y))
    {
        ev1 = CMapIndex(index.x-1, index.y);
        ev2 = CMapIndex(index.x+1, index.y);
    }
    // (even, odd) - vert
    else if(Utilities::isEven(index.x) && Utilities::isOdd(index.y))
    {
        ev1 = CMapIndex(index.x, index.y-1);
        ev2 = CMapIndex(index.x, index.y+1);
    }
    // (odd, odd) - diag
    else if(Utilities::isOdd(index.x) && Utilities::isOdd(index.y))
    {
        ev1 = CMapIndex(index.x-1, index.y+1);
        ev2 = CMapIndex(index.x+1, index.y-1);
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
//    TriMesh::EdgeIter edgeIter;
//    TriMesh::VertexHandle vertex;
//    TriMesh::HalfedgeHandle halfEdge;
//    TriMesh::HalfedgeHandle oppHalfEdge;
//
//    for( edgeIter = mesh->edges_begin();
//        edgeIter != mesh->edges_end();
//        ++edgeIter)
//    {
//        halfEdge = mesh->halfedge_handle( *edgeIter, 0);
//        oppHalfEdge = mesh->halfedge_handle( *edgeIter, 1);
//
//        TriMesh::Point pos(0.0, 0.0, 0.0);
//        if(mesh->is_boundary(*edgeIter))
//        {
//            //TODO: support boundary edges
//            std::cout<<"boundary edge!\n";
//            return;
//        }
//        else
//        {
//            TriMesh::Point v1v2(0.0, 0.0, 0.0);
//            TriMesh::Point v3v4(0.0, 0.0, 0.0);
//            // E` = (3/8)* [V1 + V2] + (1/8)*[V3 + V4]
//            vertex = mesh->to_vertex_handle(halfEdge);
//            v1v2 = mesh->point(vertex); // V1
//
//            vertex = mesh->to_vertex_handle(oppHalfEdge);
//            v1v2 += mesh->point(vertex); // V1 + V2
//
//            vertex = mesh->to_vertex_handle(mesh->next_halfedge_handle(halfEdge));
//            v3v4 = mesh->point(vertex); // V3
//
//            vertex = mesh->to_vertex_handle(mesh->next_halfedge_handle(oppHalfEdge));
//            v3v4 += mesh->point(vertex); // V3 + V4
//            pos = (3.0/8.0)*v1v2 + (1.0/8.0)*v3v4;
//        }
//        mesh->property(edgePoint, *edgeIter) = pos;
//        // add details
//        mesh->property(edgePoint, *edgeIter) += mesh->property(detailsProp, *edgeIter);
//    }
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
    ACM::VertexIter v_it;
    for(v_it = acm->v_begin(); v_it != acm->v_end(); v_it++)
    {
        *v_it = changes[v_it->idx];
    }
}