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
    initFaceColours(QuadMesh::Color(102, 0, 0));
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
        glm::vec3 face_colour = cm.colour;
        std::vector<std::array<VertexHandle,3>> faces;
        faces = acm.getCMapFaces(cm.idx);
        for(const auto& face : faces)
        {
            for(const auto& vertHandle : face)
            {
                vert = acm.getVertex(vertHandle);
                points.push_back(vert->point);
                colours.push_back(face_colour);
                normals.push_back(vert->normal);
                faceIndeces.push_back(index);
                index++;
            }
        }
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

void HeartMesh::initFaceColours(QuadMesh::Color c)
{
    QuadMesh::ConstFaceIter        f_it(mesh.faces_sbegin()),
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

    initializeACMVerts();

    QuadMesh::FaceIter f_iter;
    for(f_iter = mesh.faces_begin() ; f_iter != mesh.faces_end(); f_iter++)
    {
        QuadMesh::FaceVertexIter fv_iter;
        std::vector<VertexHandle> verts;
        for(fv_iter = mesh.fv_iter(*f_iter); fv_iter.is_valid(); fv_iter++)
        {
            verts.push_back(fv_iter->idx());
        }
        // create cm and add verts
        CMapHandle cm = acm.addCmap();
        acm.addVertsToCMap(cm, verts[0], verts[3], verts[1], verts[2]);
        acm.setFaceColour(cm, getRandomRGBF());

    }
}

void HeartMesh::initializeACMVerts()
{
    QuadMesh::VertexIter vit;
    for(vit = mesh.vertices_begin(); vit != mesh.vertices_end(); vit++)
    {
        VertexHandle idx(createACMVertex(*vit));
    }
}

int HeartMesh::createACMVertex(const QuadMesh::VertexHandle QuadMeshVert)
{
    Vertex vert;
    vert.point = pointToVec3(mesh.point(QuadMeshVert));
    vert.normal = pointToVec3(mesh.normal(QuadMeshVert));
    return acm.addVertex(vert);
}

int HeartMesh::createACMVertex(const QuadMesh::VertexHandle QuadMeshVert, glm::vec3 colour)
{
    int idx(createACMVertex(QuadMeshVert));
    acm.getVertex(idx)->colour = colour;
    return idx;
}

void HeartMesh::getRandomRGB(int &r, int &g, int &b)
{
    r = gen(rng);
    g = gen(rng);
    b = gen(rng);
}

glm::vec3 HeartMesh::getRandomRGBF()
{
    glm::vec3 rgb;
    rgb.r = gen(rng)/255.0f;
    rgb.g = gen(rng)/255.0f;
    rgb.b = gen(rng)/255.0f;
    return rgb;
}
