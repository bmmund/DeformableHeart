#include "HeartMesh.hpp"
#include <iostream>

HeartMesh::HeartMesh(std::string filename)
    : fname(filename),
      mesh()
{
    mesh.request_vertex_normals();
    mesh.request_face_normals();
    OpenMesh::IO::Options opt;
    opt += OpenMesh::IO::Options::VertexColor;
    opt += OpenMesh::IO::Options::VertexNormal;
    opt += OpenMesh::IO::Options::FaceColor;
    opt += OpenMesh::IO::Options::FaceNormal;
    std::cout<<"filename:"<< fname<<std::endl;
    // read mesh from stdin
    if ( ! OpenMesh::IO::read_mesh(mesh, fname.c_str(), opt) )
    {
        std::cerr << "Error: Cannot read mesh from " << fname << std::endl;
    }
    else
    {
        std::cout<<"verts:"<<mesh.n_vertices()<<std::endl;
    }
    mesh.update_normals();

}

HeartMesh::~HeartMesh()
{
}