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
    mesh.request_face_colors();
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
    initFaceColours(TriMesh::Color(102, 0, 0));
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
    int index;

    points.clear();
    normals.clear();
    colours.clear();
    faceIndeces.clear();

    points.reserve(mesh.n_vertices()*3);
    normals.reserve(mesh.n_vertices()*3);
    colours.reserve(mesh.n_vertices()*3);
    faceIndeces.reserve(mesh.n_faces()*3);

    for (; f_it!=f_end; ++f_it)
    {
        fv_it=mesh.cfv_iter(*f_it);

        for(int i = 0; i < 3; i++)
        {
            TriMesh::Color c(mesh.color(*f_it));
            OpenMesh::Vec3f c_float(c[0]/255.0f, c[1]/255.0f, c[2]/255.0f);

            points.push_back(mesh.point(*fv_it));
            colours.push_back(c);
            normals.push_back(mesh.normal(*fv_it));
            index = points.size() - 1;
            faceIndeces.push_back(index);
            fv_it++;
        }
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

    int points_size = points.size() * sizeof(TriMesh::Point);
    int normals_size = normals.size() * sizeof(TriMesh::Normal);
    int colours_size = colours.size() * sizeof(TriMesh::Color);
    int vertex_size = points_size + normals_size + colours_size;

    glBufferData(GL_ARRAY_BUFFER, vertex_size, NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, points_size, &points[0]);
    glBufferSubData(GL_ARRAY_BUFFER, points_size, normals_size, &normals[0]);
    glBufferSubData(GL_ARRAY_BUFFER, points_size + normals_size, colours_size, &colours[0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceIndeces.size() * sizeof(GLuint),
                 &faceIndeces[0], GL_STATIC_DRAW);

    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    // Vertex Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)points_size);
    // Vertex Colours
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_UNSIGNED_BYTE, GL_FALSE, 0, (GLvoid*)(points_size+normals_size));

    glBindVertexArray(0);
}

void HeartMesh::initFaceColours(TriMesh::Color c)
{
    TriMesh::ConstFaceIter        f_it(mesh.faces_sbegin()),
    f_end(mesh.faces_end());
    for (; f_it!=f_end; ++f_it)
    {
        mesh.set_color(*f_it, c);
    }
}
#endif