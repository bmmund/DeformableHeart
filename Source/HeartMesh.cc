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
        Vertex *vert;

        vert = acm.getVertex(cm.cm[0][1]);
        points.push_back(vert->point);
        colours.push_back(vert->colour);
        normals.push_back(vert->normal);
        faceIndeces.push_back(index);

        vert = acm.getVertex(cm.cm[1][1]);
        points.push_back(vert->point);
        colours.push_back(vert->colour);
        normals.push_back(vert->normal);
        index++;
        faceIndeces.push_back(index);

        vert = acm.getVertex(cm.cm[0][0]);
        points.push_back(vert->point);
        colours.push_back(vert->colour);
        normals.push_back(vert->normal);
        index++;
        faceIndeces.push_back(index);

        faceIndeces.push_back(index);
        faceIndeces.push_back(index-1);

        vert = acm.getVertex(cm.cm[1][0]);
        points.push_back(vert->point);
        colours.push_back(vert->colour);
        normals.push_back(vert->normal);
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
    int r, g, b;
    getRandomRGB(r, g, b);
    glm::vec3 colourf(r/255.0f, g/255.0f, b/255.0f);

    // set the face colour
    output.colour = colourf;
    output.vectorScale = 1;

    // allocate space
    output.cm.resize(2);
    for(auto& i : output.cm)
    {
        i.resize(2);
    }

    TriMesh::VertexHandle vh00, vh01, vh10, vh11;
    vh00 = mesh.to_vertex_handle(heh1);
    vh01 = mesh.to_vertex_handle(mesh.next_halfedge_handle(heh1));
    vh10 = mesh.to_vertex_handle(mesh.next_halfedge_handle(heh2));
    vh11 = mesh.to_vertex_handle(heh2);

    int v00(createACMVertex(vh00, colourf));
    int v01(createACMVertex(vh01, colourf));
    int v10(createACMVertex(vh10, colourf));
    int v11(createACMVertex(vh11, colourf));

    output.cm[0][0] = v00;
    output.cm[0][1] = v01;
    output.cm[1][0] = v10;
    output.cm[1][1] = v11;

    output.isPhantom = false;
    acm.add(output);
}

void HeartMesh::createPhantomCMFromEdge(const TriMesh::HalfedgeHandle& heh)
{
    CMap output;
    int r, g, b;
    getRandomRGB(r, g, b);
    glm::vec3 colourf(r/255.0f, g/255.0f, b/255.0f);

    // set the face colour
    output.colour = colourf;
    output.vectorScale = 1;

    // allocate space
    output.cm.resize(2);
    for(auto& i : output.cm)
    {
        i.resize(2);
    }

    TriMesh::VertexHandle vh00, vh01, vh10, vh11;
    vh00 = mesh.to_vertex_handle(heh);
    vh01 = mesh.to_vertex_handle(mesh.next_halfedge_handle(heh));
    vh10 = vh00;
    vh11 = mesh.from_vertex_handle(heh);

    int v00(createACMVertex(vh00, colourf));
    int v01(createACMVertex(vh01, colourf));
    int v10(createACMVertex(vh10, colourf));
    int v11(createACMVertex(vh11, colourf));

    output.cm[0][0] = v00;
    output.cm[0][1] = v01;
    output.cm[1][0] = v10;
    output.cm[1][1] = v11;

    output.isPhantom = true;
    acm.add(output);
}

int HeartMesh::createACMVertex(const TriMesh::VertexHandle triMeshVert)
{
    Vertex vert;
    vert.point = pointToVec3(mesh.point(triMeshVert));
    vert.normal = pointToVec3(mesh.normal(triMeshVert));
    return acm.add_vertex(vert);
}

int HeartMesh::createACMVertex(const TriMesh::VertexHandle triMeshVert, glm::vec3 colour)
{
    int idx(createACMVertex(triMeshVert));
    acm.getVertex(idx)->colour = colour;
    return idx;
}
void HeartMesh::getRandomRGB(int &r, int &g, int &b)
{
    r = gen(rng);
    g = gen(rng);
    b = gen(rng);
}
