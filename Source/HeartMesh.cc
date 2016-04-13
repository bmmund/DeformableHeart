#include "HeartMesh.hpp"
#include <iostream>

HeartMesh::HeartMesh(std::string filename)
    : fname(filename),
      mesh()
{
    std::cout<<"filename:"<< fname<<std::endl;
    // read mesh from stdin
    if ( ! OpenMesh::IO::read_mesh(mesh, fname.c_str()) )
    {
        std::cerr << "Error: Cannot read mesh from " << fname << std::endl;
    }
    else
    {
        std::cout<<"verts:"<<mesh.n_vertices()<<std::endl;
    }
    // Make sure we have vertex normals
    mesh.request_face_normals();
    mesh.request_vertex_normals();
    TriMesh::VertexIter v_it(mesh.vertices_begin());
    for(; v_it != mesh.vertices_end(); ++v_it)
        mesh.calc_vertex_normal(*v_it);
}

HeartMesh::~HeartMesh()
{
}