#include "HeartMesh.hpp"
#include <iostream>
#include <random>

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
    initializeACM();

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

    points.reserve(mesh.n_faces()*3);
    normals.reserve(mesh.n_faces()*3);
    colours.reserve(mesh.n_faces()*3);
    faceIndeces.reserve(mesh.n_faces()*3);

    index = 0;
    for(const auto& cm : acm)
    {
        Vertex vert;

        vert = cm.cm[0][1];
        points.push_back(vert.p);
        colours.push_back(vert.c);
        normals.push_back(vert.n);
        faceIndeces.push_back(index);

        vert = cm.cm[1][1];
        points.push_back(vert.p);
        colours.push_back(vert.c);
        normals.push_back(vert.n);
        index++;
        faceIndeces.push_back(index);

        vert = cm.cm[0][0];
        points.push_back(vert.p);
        colours.push_back(vert.c);
        normals.push_back(vert.n);
        index++;
        faceIndeces.push_back(index);

        faceIndeces.push_back(index);
        faceIndeces.push_back(index-1);

        vert = cm.cm[1][0];
        points.push_back(vert.p);
        colours.push_back(vert.c);
        normals.push_back(vert.n);
        index++;
        faceIndeces.push_back(index);
        index++;
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

void HeartMesh::initializeACM()
{
    std::mt19937 rng(std::mt19937::default_seed);
    std::uniform_int_distribution<int> gen(0, 255);

    acm.clear();
    pairedTrisEH.clear();
    pairedTrisFH.clear();
    pairedTrisFH.resize(mesh.n_faces(),-1);

    TriMesh::ConstEdgeIter edgeIter;
    for(edgeIter = mesh.edges_begin();
        edgeIter != mesh.edges_end();
        edgeIter++)
    {
        TriMesh::HalfedgeHandle heh1(mesh.halfedge_handle(*edgeIter, 0));
        TriMesh::HalfedgeHandle heh2(mesh.halfedge_handle(*edgeIter, 1));
        TriMesh::FaceHandle f1(mesh.face_handle(heh1));
        TriMesh::FaceHandle f2(mesh.face_handle(heh2));

        // if either are already paired, skip this set
        if( ( pairedTrisFH[f1.idx()] != -1 )
           || (pairedTrisFH[f2.idx()] != -1)
           )
        {
            continue;
        }
        else
        {
            CMap temp;
            // heh1 is left, heh2 is right
            createCMFromEdge(*edgeIter, temp);

            // Pair these triangles and add edge to list
            pairedTrisFH[f1.idx()] = f2.idx();
            pairedTrisFH[f2.idx()] = f1.idx();
            // Pair these triangles and add edge to list
            pairedTrisEH.push_back(*edgeIter);
            // set the face colour
            int r = gen(rng);
            int g = gen(rng);
            int b = gen(rng);
            TriMesh::Color c(r,g,b);
            mesh.set_color(f1, c);
            mesh.set_color(f2, c);
            temp.cm[0][0].c = c;
            temp.cm[1][1].c = c;
            temp.cm[0][1].c = c;
            temp.cm[1][0].c = c;
            acm.push_back(temp);
        }
    }
    for(int i = 0; i < pairedTrisFH.size(); i++)
    {
        if(pairedTrisFH[i] == -1)
        {
            CMap temp;
            TriMesh::HalfedgeHandle heh(mesh.halfedge_handle(mesh.face_handle(i)));
            createPhantomCMFromEdge(heh, temp);
            // Pair these triangles and add edge to list
            pairedTrisEH.push_back(mesh.edge_handle(mesh.halfedge_handle(mesh.face_handle(i))));
            // set the face colour
            int r = gen(rng);
            int g = gen(rng);
            int b = gen(rng);
            TriMesh::Color c(r,g,b);
            mesh.set_color(mesh.face_handle(i), c);
            temp.cm[0][0].c = c;
            temp.cm[1][1].c = c;
            temp.cm[0][1].c = c;
            temp.cm[1][0].c = c;
            acm.push_back(temp);
        }
    }
}

void HeartMesh::createCMFromEdge(const TriMesh::EdgeHandle& edge, CMap& output)
{
    TriMesh::HalfedgeHandle heh1(mesh.halfedge_handle(edge, 0));
    TriMesh::HalfedgeHandle heh2(mesh.halfedge_handle(edge, 1));
    output.edgeKey = edge;
    output.vectorScale = 1;
    output.cm.resize(2);
    for(auto& i : output.cm)
    {
        i.resize(2);
    }
    output.cm[0][0].p = mesh.point(mesh.to_vertex_handle(heh1));
    output.cm[1][1].p = mesh.point(mesh.to_vertex_handle(heh2));
    output.cm[0][1].p = mesh.point(
                                 mesh.to_vertex_handle(
                                                       mesh.next_halfedge_handle(heh1)
                                                       )
                                 );

    output.cm[1][0].p = mesh.point(
                                 mesh.to_vertex_handle(
                                                       mesh.next_halfedge_handle(heh2)
                                                       )
                                 );

    output.cm[0][0].n = mesh.normal(mesh.to_vertex_handle(heh1));
    output.cm[1][1].n = mesh.normal(mesh.to_vertex_handle(heh2));
    output.cm[0][1].n = mesh.normal(
                                  mesh.to_vertex_handle(
                                                        mesh.next_halfedge_handle(heh1)
                                                        )
                                  );

    output.cm[1][0].n = mesh.normal(
                                  mesh.to_vertex_handle(
                                                        mesh.next_halfedge_handle(heh2)
                                                        )
                                  );
    output.isPhantom = false;
}

void HeartMesh::createPhantomCMFromEdge(const TriMesh::HalfedgeHandle& heh, CMap& output)
{
    output.edgeKey = mesh.edge_handle(heh);
    output.vectorScale = 1;
    output.cm.resize(2);
    for(auto& i : output.cm)
    {
        i.resize(2);
    }
    output.cm[0][0].p = mesh.point(mesh.to_vertex_handle(heh));
    output.cm[1][1].p = mesh.point(mesh.from_vertex_handle(heh));
    output.cm[0][1].p = mesh.point(
                                   mesh.to_vertex_handle(
                                                         mesh.next_halfedge_handle(heh)
                                                         )
                                   );

    output.cm[1][0].p = output.cm[0][0].p;

    output.cm[0][0].n = mesh.normal(mesh.to_vertex_handle(heh));
    output.cm[1][1].n = mesh.normal(mesh.from_vertex_handle(heh));
    output.cm[0][1].n = mesh.normal(
                                   mesh.to_vertex_handle(
                                                         mesh.next_halfedge_handle(heh)
                                                         )
                                   );

    output.cm[1][0].n = output.cm[0][0].n;

    output.isPhantom = true;
}

#endif