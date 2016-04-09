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

}

HeartMesh::~HeartMesh()
{
}