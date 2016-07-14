#include "loop.hpp"
#include <cmath>
#include <iostream>
#include <vector>
#include <assert.h>

#ifndef NDEBUG
    #define ENABLE_CHECK_MESH
#endif

#ifdef ENABLE_CHECK_MESH
    #include <OpenMesh/Tools/Utils/MeshCheckerT.hh>
    #define CHECK_MESH(mesh) {OpenMesh::Utils::MeshCheckerT<TriMesh> mt(mesh);\
                        assert(mt.check());}
#else
    // do nothing if not enabled
    #warning mesh is no longer being checked using CHECK_MESH
    #define CHECK_MESH(mesh)
#endif

Loop::Loop()
{

}

Loop::~Loop()
{

}

void Loop::subdivide(TriMesh *mesh)
{
    setupMeshProperties(mesh);
    // setup mesh with required attributes
    setupMeshProcessingProperties(mesh);
    // Find vertex-vertex positions
    setVertexVertexPositions(mesh);
    // Find edge-vertex positions
    setEdgeVertexPositions(mesh);
    // Split edge to create new vert
    splitEdges(mesh);
    // Split faces
    splitFaces(mesh);
    // Apply geometry changes
    updateGeometries(mesh);
    // remove subdivision attributes
    teardownMeshProcessingProperties(mesh);
    // Subdivision complete, increase subdivision depth
    mesh->property(subdevDepth)++;

    CHECK_MESH(*mesh);
}

void Loop::setupMeshProperties(TriMesh * mesh)
{
    // TODO store these in the mesh model object code
    // make sure we don't redefine even odd property
    OpenMesh::VPropHandleT<bool> evenOddCheck;
    if (!mesh->get_property_handle(evenOddCheck, v_prop_even_odd_name))
    {
        mesh->add_property(isEvenVertex, v_prop_even_odd_name);
    }

    // Test to see if there is no subdev property, if there isn't initialize it
    OpenMesh::MPropHandleT<int> subdevDepthCheck;
    if (!mesh->get_property_handle(subdevDepthCheck, m_prop_subdev_depth_name))
    {
        mesh->add_property(subdevDepth, m_prop_subdev_depth_name);
        mesh->property(subdevDepth) = 0;
        std::cout << "Mesh was not previously subdivided.\n";
    }
    else
    {
        std::cout << "Mesh was at subdivision level " <<
            mesh->property(subdevDepth) << " when called.\n";
    }
    // When we subdivid, all points are now considered even
    for (TriMesh::VertexIter vertIter = mesh->vertices_begin();
        vertIter != mesh->vertices_end(); ++vertIter)
    {
        mesh->property(isEvenVertex, *vertIter) = true;
    }
}

void Loop::decompose(TriMesh * mesh)
{
    // setup mesh with required attributes
    setupMeshProcessingProperties(mesh);
    // initialize all vertex positions to current position
    for (TriMesh::VertexIter vertIter = mesh->vertices_begin();
        vertIter != mesh->vertices_end();
        ++vertIter)
    {
        mesh->property(vertPoint, *vertIter) = mesh->point(*vertIter);
    }

    unsplitFaces(mesh);

    // All even verts have no coarse locations
    // Find vertex-vertex (coarse even) positions
    findCoarseEvenPositions(mesh);

    //remove the edge-vertex (odd) vertices
    removeOddVertices(mesh);

    // Apply geometry changes
    updateGeometries(mesh);

    // remove subdivision attributes
    teardownMeshProcessingProperties(mesh);
    // Subdivision complete, increase subdivision depth
    mesh->property(subdevDepth)--;
    CHECK_MESH(*mesh);
}


int Loop::getSubDivisionDepth(TriMesh * mesh)
{
    int subDevDepth = 0;
    OpenMesh::MPropHandleT<int> subdevDepthCheck;
    if (mesh->get_property_handle(subdevDepthCheck, m_prop_subdev_depth_name))
    {
        subDevDepth = mesh->property(subdevDepth);
    }
    return subDevDepth;
}

bool Loop::isVertEven(TriMesh * mesh, TriMesh::VertexHandle vh)
{
    OpenMesh::VPropHandleT<bool> isEvenCheck;
    if (mesh->get_property_handle(isEvenCheck, v_prop_even_odd_name))
    {
        return mesh->property(isEvenVertex, vh);
    }
    return true;
}

void Loop::setupMeshProcessingProperties(TriMesh *mesh)
{
    mesh->add_property(vertPoint, vv_prop_name);
    mesh->add_property(edgePoint, ev_prop_name);
}

void Loop::teardownMeshProcessingProperties(TriMesh *mesh)
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
        mesh->property( isEvenVertex, *vertIter ) = true;
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
        splitEdge(mesh, *edgeIter);
    }
}

void Loop::splitEdge(TriMesh* mesh, const TriMesh::EdgeHandle& _eh)
{
    // original half edge of edge to split
    TriMesh::HalfedgeHandle heh1, heh2;
    heh1 = mesh->halfedge_handle(_eh, 0);
    heh2 = mesh->halfedge_handle(_eh, 1);

    TriMesh::VertexHandle   evh;
    TriMesh::VertexHandle   vh1(mesh->to_vertex_handle(heh2));
    TriMesh::VertexHandle   vh2(mesh->to_vertex_handle(heh1));

    // Face handles for updating half-edge face reference
    TriMesh::FaceHandle     f1h(mesh->face_handle(heh1));
    TriMesh::FaceHandle     f2h(mesh->face_handle(heh2));

    // additional half edge handles for splitting
    TriMesh::HalfedgeHandle heh3, heh4, heh_ext;

    // find midpoint of edge
    TriMesh::Point          midP(mesh->point(vh1));
    midP += mesh->point(vh2);
    midP *= 0.5;

    // new edge-vertex at midpoint
    evh = mesh->new_vertex( midP );
    mesh->set_normal(evh, mesh->normal(vh1));

    // retrieve position stored in edge and store in vertex for later
    mesh->property( vertPoint, evh ) = mesh->property( edgePoint, _eh );
    mesh->property( isEvenVertex, evh ) = false;

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
    mesh->set_halfedge_handle(evh, heh4);
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
    mesh->adjust_outgoing_halfedge(evh);
    mesh->adjust_outgoing_halfedge(vh1);
    mesh->adjust_outgoing_halfedge(vh2);
}

void Loop::splitFaces(TriMesh* mesh)
{
    TriMesh::FaceIter fit, f_end;
    f_end   = mesh->faces_end();
    for (fit = mesh->faces_begin(); fit != f_end; ++fit)
    {
        TriMesh::FaceVertexIter vit;
        std::vector<TriMesh::HalfedgeHandle> hehHandles;
        TriMesh::FaceHalfedgeIter hehIter;
        // Find the 3 half edges we care about splitting
        for(hehIter = mesh->fh_iter(*fit); hehIter.is_valid(); ++hehIter)
        {
            TriMesh::HalfedgeHandle heh(*hehIter);
            if(mesh->property(isEvenVertex, mesh->from_vertex_handle(heh)))
               {
                   hehHandles.push_back(heh);
               }
        }
        if(hehHandles.size() != 3)
        {
            std::cerr<<"Edges not properly split!\n";
            return;
        }
        else
        {
            for(int i = 0; i < hehHandles.size(); i++)
            {
                TriMesh::HalfedgeHandle heh(hehHandles[i]);
                splitCorner( mesh, &heh );
            }
        }
    }
}

void Loop::splitCorner(TriMesh* mesh, TriMesh::HalfedgeHandle* he)
{

    TriMesh::HalfedgeHandle heh1, heh2, heh3, heh4, heh5, heh6;

    // heh1 = input half edge, belongs to new face
    heh1 = *he;
    // cycle around half edges until we have last half edge (belongs to new face)
    for(heh3 = heh1;
        mesh->next_halfedge_handle(heh3) != heh1;
        heh3 = mesh->next_halfedge_handle(heh3)
        )
    {}
    heh5 = mesh->next_halfedge_handle(heh1);
    // cycle around half edges until we have second last half edge
    // (belongs to old face)
    for(heh6 = heh5;
        mesh->next_halfedge_handle(heh6) != heh3;
        heh6 = mesh->next_halfedge_handle(heh6)
        )
    {}

    // Create old (f1) and new(f2) face handles
    TriMesh::FaceHandle f1h(mesh->face_handle(heh1));
    TriMesh::FaceHandle f2h(mesh->new_face());

    // Edge vertices for new edge
    TriMesh::VertexHandle ev1h(mesh->to_vertex_handle(heh1));
    TriMesh::VertexHandle ev2h(mesh->to_vertex_handle(mesh->opposite_halfedge_handle(heh3)));

    // create new edge
    heh2 = mesh->new_edge(ev1h, ev2h);
    heh4 = mesh->opposite_halfedge_handle(heh2);

    // update F1 half edges
    mesh->set_next_halfedge_handle(heh4, heh5);
    mesh->set_next_halfedge_handle(heh6, heh4);
    mesh->set_face_handle(heh4, f1h);
    mesh->set_halfedge_handle(f1h, heh4); // heh4 is now the f1 reference

    // update F2 half edges
    mesh->set_next_halfedge_handle(heh1, heh2);
    mesh->set_next_halfedge_handle(heh2, heh3);
    mesh->set_face_handle(heh1, f2h);
    mesh->set_face_handle(heh2, f2h);
    mesh->set_face_handle(heh3, f2h);
    mesh->set_halfedge_handle(f2h, heh2); // heh4 is now the f1 reference

    // Based on open mesh documentation we must update outgoing halfedges
    // whenever topology is changed.
    mesh->adjust_outgoing_halfedge(ev1h);
    mesh->adjust_outgoing_halfedge(ev2h);
}

float Loop::getWeight(int valence)
{
    //TODO: Replace with pre-calculated values
    // weight = (1/n) * ( (5/8) - ((3/8) + ((1/4)*cos(2*pi/n))^2)
    float weight;
    weight = cos(2.0f*M_PI/valence);
    weight = (1.0f/4.0f) * weight;
    weight = (3.0f/8.0f) + weight;
    weight = weight * weight;
    weight = (5.0f/8.0f) - weight;
    weight = (1.0f/valence) * weight;
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
    mesh->update_normals();
}

void Loop::findCoarseEvenPositions(TriMesh * mesh)
{
    for (TriMesh::VertexIter vertIter = mesh->vertices_begin(); vertIter != mesh->vertices_end(); ++vertIter)
    {
        TriMesh::Point pos(0.0, 0.0, 0.0);

        // only do for even verts
        if (!mesh->property(isEvenVertex,*vertIter))
        {
            continue;
        }

        // Two cases: boundary or non-boundary vertex
        if (mesh->is_boundary(*vertIter))
        {
            //TODO: support boundary verts
            std::cout << "boundary verts!\n";
            return;
        }
        else
        {
            int valence = mesh->valence(*vertIter);
            // Based on L. Olsen's work, they use beta for getWeight()
            // leaving alpha for a reverse-subdivision weight.
            // alpha =  8*beta/5
            float alpha = (8.0f*getWeight(valence))/5.0f;
            TriMesh::VertexVertexIter vertVertIter;
            // alpha * SUM(Vj)
            for (vertVertIter = mesh->vv_iter(*vertIter);
                vertVertIter.is_valid();
                ++vertVertIter)
            {
                pos += mesh->point(*vertVertIter);
            }
            // c0 = (1/(1-na))f0 - (a/(1-na))sum(fi)
            pos = ( (1.0f/(1.0f - valence*alpha)) * mesh->point(*vertIter) ) -
                    ( (alpha/(1.0f-valence*alpha)) * (pos) );
        }
        // add calculated value to vertex-vertex
        mesh->property(vertPoint, *vertIter) = pos;
    }
}

void Loop::removeOddVertices(TriMesh * mesh)
{
    for (TriMesh::VertexIter vertIter = mesh->vertices_begin(); vertIter != mesh->vertices_end(); ++vertIter)
    {
        TriMesh::Point pos(0.0, 0.0, 0.0);

        // only do for odd verts
        if (mesh->property(isEvenVertex, *vertIter))
        {
            continue;
        }

        // Two cases: boundary or non-boundary vertex
        if (mesh->is_boundary(*vertIter))
        {
            //TODO: support boundary verts
            std::cout << "boundary verts!\n";
            return;
        }
        else
        {
            // remove the vertex and fix the faces
            TriMesh::HalfedgeHandle heh(mesh->halfedge_handle(*vertIter));
            assert((mesh->is_collapse_ok(heh)) == true);
            mesh->collapse(heh);
        }
    }
    mesh->garbage_collection();
}

void Loop::unsplitFaces(TriMesh * mesh)
{
    TriMesh::FaceIter fit;
    std::vector<TriMesh::FaceHandle> fhandles;
    // Find faces with only odd vertices
    for (fit = mesh->faces_begin(); fit != mesh->faces_end(); ++fit)
    {
        TriMesh::FaceVertexIter vit;
        bool isOddTri = true;
        for (vit = mesh->fv_iter(*fit); vit.is_valid(); ++vit)
        {
            if (mesh->property(isEvenVertex, *vit))
            {
                isOddTri = false;
                break;
            }
        }
        if (isOddTri)
        {
            fhandles.push_back(*fit);
        }
        else
        {
            // continue
            continue;
        }
    }
    for (auto& f : fhandles)
    {
        std::vector<TriMesh::HalfedgeHandle> hehHandles;
        // Find the 3 half edges we care about splitting
        for (TriMesh::FaceHalfedgeIter hehIter = mesh->fh_iter(f); hehIter.is_valid(); ++hehIter)
        {
            // find each even vert within the opposite face
            TriMesh::HalfedgeHandle heh(*hehIter);
            hehHandles.push_back(heh);
        }
        //for(auto& heh : hehHandles)
        while (!hehHandles.empty())
        {
            mesh->remove_edge(mesh->edge_handle(hehHandles.back()));
            hehHandles.pop_back();
        }
    }
    mesh->garbage_collection();
}

bool Loop::doesHalfEdgeContainOddOnly(TriMesh * mesh, const OpenMesh::HalfedgeHandle &heh)
{
    TriMesh::VertexHandle v1(mesh->to_vertex_handle(heh));
    TriMesh::VertexHandle v2(mesh->from_vertex_handle(heh));
    return (!mesh->property(isEvenVertex, v1) && !mesh->property(isEvenVertex, v2));
}
