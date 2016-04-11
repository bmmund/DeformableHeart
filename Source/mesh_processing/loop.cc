#include "loop.hpp"
#include <cmath>
#include <iostream>
Loop::Loop()
{

}

Loop::~Loop()
{

}

void Loop::subdivide(TriMesh *mesh)
{
    std::cout<<"applying subdivision\n";
    // setup mesh with required attributes
    setupMeshProperties(mesh);
    // Find vertex-vertex positions
    setVertexVertexPositions(mesh);
    // Find edge-vertex positions
    setEdgeVertexPositions(mesh);
    // Apply geometry changes
    updateGeometries(mesh);
    // remove subdivision attributes
    teardownMeshProperties(mesh);
}

void Loop::setupMeshProperties(TriMesh *mesh)
{
    mesh->add_property(vertPoint, "vv_prop");
    mesh->add_property(edgePoint, "ev_prop");
}

void Loop::teardownMeshProperties(TriMesh *mesh)
{
    mesh->remove_property(vertPoint);
    mesh->remove_property(edgePoint);
}

void Loop::setVertexVertexPositions(TriMesh* mesh)
{
    TriMesh::VertexIter vertIter;

    for(vertIter = mesh->vertices_begin(); vertIter != mesh->vertices_end(); ++vertIter)
    {
        TriMesh::Point pos(0.0, 0.0, 0.0);
        // Two cases: boundary or non-boundary vertex
        if(mesh->is_boundary(*vertIter))
        {
            //TODO: support boundary verts
            std::cout<<"boundary verts!\n";
            return;
        }
        else
        {
            int valence = mesh->valence(*vertIter);
            float alpha = getWeight(valence);
            TriMesh::VertexVertexIter vertVertIter;
            // alpha * SUM(Vj)
            for(vertVertIter = mesh->vv_iter(*vertIter);
                vertVertIter.is_valid();
                ++vertVertIter)
            {
                pos += mesh->point(*vertVertIter);
            }
            pos = alpha * pos;
            // add vertex contribution
            pos += (1 - (valence*alpha) ) * mesh->point(*vertIter);
        }
        // add calculated value to vertex-vertex
        mesh->property(vertPoint, *vertIter) = pos;
    }
}

void Loop::setEdgeVertexPositions(TriMesh* mesh)
{
    TriMesh::EdgeIter edgeIter;
    TriMesh::VertexHandle vertex;
    TriMesh::HalfedgeHandle halfEdge;
    TriMesh::HalfedgeHandle oppHalfEdge;

    for( edgeIter = mesh->edges_begin();
        edgeIter != mesh->edges_end();
        ++edgeIter)
    {
        halfEdge = mesh->halfedge_handle( *edgeIter, 0);
        oppHalfEdge = mesh->halfedge_handle( *edgeIter, 1);

        TriMesh::Point pos(0.0, 0.0, 0.0);
        if(mesh->is_boundary(*edgeIter))
        {
            //TODO: support boundary edges
            std::cout<<"boundary edge!\n";
            return;
        }
        else
        {
            TriMesh::Point v1v2(0.0, 0.0, 0.0);
            TriMesh::Point v3v4(0.0, 0.0, 0.0);
            // E` = (3/8)* [V1 + V2] + (1/8)*[V3 + V4]
            vertex = mesh->to_vertex_handle(halfEdge);
            v1v2 = mesh->point(vertex); // V1

            vertex = mesh->to_vertex_handle(oppHalfEdge);
            v1v2 += mesh->point(vertex); // V1 + V2

            vertex = mesh->to_vertex_handle(mesh->next_halfedge_handle(halfEdge));
            v3v4 = mesh->point(vertex); // V3

            vertex = mesh->to_vertex_handle(mesh->next_halfedge_handle(oppHalfEdge));
            v3v4 += mesh->point(vertex); // V3 + V4
            pos = (3.0/8.0)*v1v2 + (1.0/8.0)*v3v4;
        }
        mesh->property(edgePoint, *edgeIter) = pos;
    }
}

float Loop::getWeight(int valence)
{
    //TODO: Replace with pre-calculated values
    // weight = (1/n) * ( (5/8) - ((3/8) + ((1/4)*cos(2*pi/n))^2)
    float weight;
    weight = cos(2.0*M_PI/valence);
    weight = (1.0/4.0) * weight;
    weight = (3.0/8.0) + weight;
    weight = weight * weight;
    weight = (5.0/8.0) - weight;
    weight = (1.0/6.0) * weight;
    return weight;
}

void Loop::updateGeometries(TriMesh* mesh)
{
    // Apply vertex positions contained in attributes
    TriMesh::VertexIter vertIter;
    for(vertIter = mesh->vertices_begin();
        vertIter != mesh->vertices_end();
        ++vertIter)
    {
        mesh->set_point(*vertIter, mesh->property(vertPoint, *vertIter));
    }
}