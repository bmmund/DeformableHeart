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