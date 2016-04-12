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
    // Split edge to create new vert
    splitEdges(mesh);
    // Apply geometry changes
//    updateGeometries(mesh);
    // remove subdivision attributes
    teardownMeshProperties(mesh);
}

void Loop::setupMeshProperties(TriMesh *mesh)
{
    mesh->add_property(vertPoint, "vv_prop");
    mesh->add_property(edgePoint, "ev_prop");
    mesh->add_property(isEdgeVertex);
}

void Loop::teardownMeshProperties(TriMesh *mesh)
{
    mesh->remove_property(vertPoint);
    mesh->remove_property(edgePoint);
    mesh->remove_property(isEdgeVertex);
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
        mesh->property( isEdgeVertex, *vertIter ) = false;
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

void Loop::splitEdges(TriMesh *mesh)
{
    TriMesh::EdgeIter edgeIter,e_end;
    TriMesh::HalfedgeHandle halfEdge;
    TriMesh::HalfedgeHandle oppHalfEdge;
    TriMesh::VertexHandle newVertex, vh1;
    TriMesh::Point midPoint(0.0, 0.0, 0.0);
    TriMesh::HalfedgeHandle new_heh, opp_new_heh, t_heh;
    // Split each edge at mid-point and store calculated position in property.
    // Vertex location will be updated later.
    e_end = mesh->edges_end();
    for( edgeIter = mesh->edges_begin();
        edgeIter != e_end;
        ++edgeIter)
    {
        split_edge(mesh, *edgeIter);
    }
}

void Loop::splitEdge(TriMesh* mesh, const TriMesh::EdgeHandle& _eh)
{
    TriMesh::HalfedgeHandle
    heh1 = mesh->halfedge_handle(_eh, 0),
    heh2 = mesh->halfedge_handle(_eh, 1);

    TriMesh::HalfedgeHandle heh3, heh4, heh_ext;
    TriMesh::VertexHandle   evh;
    TriMesh::VertexHandle   vh1(mesh->to_vertex_handle(heh2));
    TriMesh::VertexHandle   vh2(mesh->to_vertex_handle(heh1));
    TriMesh::FaceHandle     f1h(mesh->face_handle(heh1));
    TriMesh::FaceHandle     f2h(mesh->face_handle(heh2));
    TriMesh::Point          midP(mesh->point(vh1));
    midP += mesh->point(vh2);
    midP *= 0.5;

    // new edge-vertex
    evh = mesh->new_vertex( midP );

    // retrieve position stored in edge and store in vertex for later
    mesh->property( vertPoint, evh ) = mesh->property( edgePoint, _eh );
    mesh->property( isEdgeVertex, evh ) = true;

    // get incomming half edge
    if(mesh->is_boundary(_eh))
    {
        //TODO: support boundary edges
        std::cout<<"boundary edge not supported!\n";
        return;
    }
    else
    {
        for(heh_ext = mesh->next_halfedge_handle(heh1);
             mesh->next_halfedge_handle(heh_ext) != heh1;
             heh_ext = mesh->next_halfedge_handle(heh_ext) )
        {}
    }
//-----------------------------------------------------------------
//             f1h
//            heh1                heh_ext (points to heh1 as next)
//       <--------------      <--------
//  v2h o---------------o v1h
//       -------------->
//            heh2
//             f2h
//
//-----------------------------------------------------------------
//      After edge split
//-----------------------------------------------------------------
//
//             f1h
//        heh1'     heh3         heh_ext (points to heh3 as next)
//        <------  <------       <--------
//    v2h o-------o-------o v1h
//                evh
//         ------> ------>
//         heh2'   heh4
//             f2h
//-----------------------------------------------------------------

    // Setup new heh4
    heh4 = mesh->new_edge(evh, vh1);  // from evh to vh1
    mesh->set_face_handle(heh4, f2h);
    mesh->set_halfedge_handle(f2h, heh4);
    mesh->set_next_halfedge_handle(heh4, mesh->next_halfedge_handle(heh2));

    // Setup new heh3
    heh3 = mesh->opposite_halfedge_handle(heh4);
    mesh->set_face_handle(heh3, f1h);
    mesh->set_halfedge_handle(f1h, heh3);
    mesh->set_next_halfedge_handle(heh_ext, heh3);
    mesh->set_next_halfedge_handle(heh3, heh1);

    // update what heh2 points to
    mesh->set_vertex_handle(heh2, evh);
    mesh->set_next_halfedge_handle(heh2, heh4);

    // Based on open mesh documentation we must update outgoing halfedges
    // whenever topology is changed.
    mesh->adjust_outgoing_halfedge( evh );
    mesh->adjust_outgoing_halfedge( vh1 );
    mesh->adjust_outgoing_halfedge( vh2 );
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