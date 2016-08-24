#include "deformation.hpp"

#include <glm/glm.hpp>

void pushVerts(QuadMesh* mesh, int index, float amount)
{
    QuadMesh::VertexHandle vh(mesh->vertex_handle(index));
    QuadMesh::Normal n = mesh->normal(vh);
    QuadMesh::Point p = mesh->point(vh);
    glm::vec3 point(p[0],p[1],p[2]);
    glm::vec3 normal(n[0],n[1],n[2]);
    glm::vec3 displace(0,0,0);
    displace = point - displace;
    displace = glm::normalize(normal);

    point = point + (amount * displace);
    for(int i = 0 ; i<3; i++)
        p[i] = point[i];
    mesh->set_point(vh, p);
}

void Deformation::pushVertsOut(QuadMesh* mesh, int index, float amount)
{
    pushVerts(mesh, index, amount);
}

void Deformation::pushVertsIn(QuadMesh* mesh, int index, float amount)
{
    pushVerts(mesh, index, -amount);
}

