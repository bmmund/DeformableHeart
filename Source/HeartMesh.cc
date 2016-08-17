#include "HeartMesh.hpp"
#include <iostream>


HeartMesh::HeartMesh(std::string filename)
    : rng(std::mt19937::default_seed),
      gen(0, 255),
      fname(filename),
      mesh(),
      acm()
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
    createBuffers();
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
    for(const auto& cm : acm.connectivityMaps())
    {
        Vertex vert;

        vert = cm.cm[0][1];
        points.push_back(vert.point);
        colours.push_back(vert.colour);
        normals.push_back(vert.normal);
        faceIndeces.push_back(index);

        vert = cm.cm[1][1];
        points.push_back(vert.point);
        colours.push_back(vert.colour);
        normals.push_back(vert.normal);
        index++;
        faceIndeces.push_back(index);

        vert = cm.cm[0][0];
        points.push_back(vert.point);
        colours.push_back(vert.colour);
        normals.push_back(vert.normal);
        index++;
        faceIndeces.push_back(index);

        faceIndeces.push_back(index);
        faceIndeces.push_back(index-1);

        vert = cm.cm[1][0];
        points.push_back(vert.point);
        colours.push_back(vert.colour);
        normals.push_back(vert.normal);
        index++;
        faceIndeces.push_back(index);
        index++;
    }
    updateBuffers();
}

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

    int points_size = points.size() * sizeof(glm::vec3);
    int normals_size = normals.size() * sizeof(glm::vec3);
    int colours_size = colours.size() * sizeof(glm::vec3);
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
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(points_size+normals_size));

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
    acm.clear();
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
            // heh1 is left, heh2 is right
            createCMFromEdge(*edgeIter);

            // Pair these triangles and add edge to list
            pairedTrisFH[f1.idx()] = f2.idx();
            pairedTrisFH[f2.idx()] = f1.idx();
        }
    }
    for(int i = 0; i < pairedTrisFH.size(); i++)
    {
        if(pairedTrisFH[i] == -1)
        {
            TriMesh::HalfedgeHandle heh(mesh.halfedge_handle(mesh.face_handle(i)));
            createPhantomCMFromEdge(heh);
            pairedTrisFH[i] = i;
        }
    }
}

void HeartMesh::createCMFromEdge(const TriMesh::EdgeHandle& edge)
{
    CMap output;
    TriMesh::HalfedgeHandle heh1(mesh.halfedge_handle(edge, 0));
    TriMesh::HalfedgeHandle heh2(mesh.halfedge_handle(edge, 1));
    glm::vec3 v00, v01, v10, v11;
    glm::vec3 n00, n01, n10, n11;
    output.vectorScale = 1;
    output.cm.resize(2);
    for(auto& i : output.cm)
    {
        i.resize(2);
    }
    v00 = pointToVec3(mesh.point(mesh.to_vertex_handle(heh1)));
    v01 = pointToVec3(mesh.point(
                                 mesh.to_vertex_handle(
                                                       mesh.next_halfedge_handle(heh1)
                                                       )
                                 ));
    v10 = pointToVec3(mesh.point(
                                 mesh.to_vertex_handle(
                                                       mesh.next_halfedge_handle(heh2)
                                                       )
                                 ));

    v11 = pointToVec3(mesh.point(mesh.to_vertex_handle(heh2)));
    output.cm[0][0].point = v00;
    output.cm[1][1].point = v11;
    output.cm[0][1].point = v01;
    output.cm[1][0].point = v10;

    n00 = pointToVec3(mesh.normal(mesh.to_vertex_handle(heh1)));
    n01 = pointToVec3(mesh.normal(
                                  mesh.to_vertex_handle(
                                                        mesh.next_halfedge_handle(heh1)
                                                        )
                                  ));
    n10 = pointToVec3(mesh.normal(
                                 mesh.to_vertex_handle(
                                                       mesh.next_halfedge_handle(heh2)
                                                       )
                                 ));
    n11 = pointToVec3(mesh.normal(mesh.to_vertex_handle(heh2)));

    output.cm[0][0].normal = n00;
    output.cm[1][1].normal = n11;
    output.cm[0][1].normal = n01;
    output.cm[1][0].normal = n10;

    // set the face colour
    int r, g, b;
    getRandomRGB(r, g, b);
    glm::vec3 colourf(r/255.0f, g/255.0f, b/255.0f);
    output.colour = colourf;
    output.cm[0][0].colour = colourf;
    output.cm[1][1].colour = colourf;
    output.cm[0][1].colour = colourf;
    output.cm[1][0].colour = colourf;

    output.isPhantom = false;
    acm.add(output);
}

void HeartMesh::createPhantomCMFromEdge(const TriMesh::HalfedgeHandle& heh)
{
    CMap output;
    glm::vec3 v00, v01, v10, v11;
    glm::vec3 n00, n01, n10, n11;
    output.vectorScale = 1;
    output.cm.resize(2);
    for(auto& i : output.cm)
    {
        i.resize(2);
    }
    v00 = pointToVec3(mesh.point(mesh.to_vertex_handle(heh)));
    v01 = pointToVec3(mesh.point(
                                 mesh.to_vertex_handle(
                                                       mesh.next_halfedge_handle(heh)
                                                       )
                                 ));

    v10 = v00;
    v11 = pointToVec3(mesh.point(mesh.from_vertex_handle(heh)));

    output.cm[0][0].point = v00;
    output.cm[1][1].point = v11;
    output.cm[0][1].point = v01;
    output.cm[1][0].point = v10;

    n00 = pointToVec3(mesh.normal(mesh.to_vertex_handle(heh)));
    n01 = pointToVec3(mesh.normal(
                                 mesh.to_vertex_handle(
                                                       mesh.next_halfedge_handle(heh)
                                                       )
                                 ));

    n10 = n00;
    n11 = pointToVec3(mesh.normal(mesh.from_vertex_handle(heh)));

    output.cm[0][0].normal = n00;
    output.cm[1][1].normal = n11;
    output.cm[0][1].normal = n01;
    output.cm[1][0].normal = n10;

    // set the face colour
    int r, g, b;
    getRandomRGB(r, g, b);
    glm::vec3 colourf(r/255.0f, g/255.0f, b/255.0f);
    output.colour = colourf;
    output.cm[0][0].colour = colourf;
    output.cm[1][1].colour = colourf;
    output.cm[0][1].colour = colourf;
    output.cm[1][0].colour = colourf;

    output.isPhantom = true;
    acm.add(output);
}

void HeartMesh::getRandomRGB(int &r, int &g, int &b)
{
    r = gen(rng);
    g = gen(rng);
    b = gen(rng);
}
