#include "HeartMesh.hpp"
#include <iostream>

HeartMesh::HeartMesh(std::string filename)
    : fname(filename),
      mesh()
{
    mesh.request_vertex_normals();
    mesh.request_face_normals();
    mesh.request_edge_status();
    mesh.request_face_status();
    mesh.request_vertex_status();
    mesh.request_halfedge_status();
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
        std::cout << "faces:" << mesh.n_faces() << std::endl;
    }
    mesh.update_normals();

    #ifndef USE_OPENGL_LEGACY
        createBuffers();
    #endif

        updateFaceIndeces();
}

HeartMesh::~HeartMesh()
{
}

void HeartMesh::updateFaceIndeces()
{
    TriMesh::ConstFaceIter        f_it(mesh.faces_sbegin()),
    f_end(mesh.faces_end());
    TriMesh::ConstFaceVertexIter  fv_it;

    faceIndeces.clear();
    faceIndeces.reserve(mesh.n_faces()*3);

    for (; f_it!=f_end; ++f_it)
    {
        fv_it=mesh.cfv_iter(*f_it);
        faceIndeces.push_back((*fv_it).idx());
        faceIndeces.push_back((*(++fv_it)).idx());
        faceIndeces.push_back((*(++fv_it)).idx());
    }
    #ifndef USE_OPENGL_LEGACY
        updateBuffers();
    #endif
}

#ifndef USE_OPENGL_LEGACY
void HeartMesh::Draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, faceIndeces.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void HeartMesh::createBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

void HeartMesh::updateBuffers()
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    int points_size = mesh.n_vertices() * sizeof(TriMesh::Point);
    int normals_size = mesh.n_vertices() * sizeof(TriMesh::Normal);
    int vertext_size = points_size + normals_size;

    glBufferData(GL_ARRAY_BUFFER, vertext_size, NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, points_size, mesh.points());
    glBufferSubData(GL_ARRAY_BUFFER, points_size, normals_size, mesh.vertex_normals());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceIndeces.size() * sizeof(GLuint),
                 &faceIndeces[0], GL_STATIC_DRAW);

    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    // Vertex Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)points_size);

    glBindVertexArray(0);
}

#endif