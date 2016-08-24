#include "rayselector.hpp"
#include <limits>
#include "utilities.hpp"

RaySelector::RaySelector(
                         int width,
                         int height,
                         glm::mat4* projection,
                         glm::mat4* view,
                         glm::mat4* model,
                         glm::vec3* eye)
    :   screenWidth(width),
        screenHeight(height),
        projM(projection),
        viewM(view),
        modelM(model),
        cameraOrigin(eye)
{
}

void RaySelector::setWindow(int width, int height)
{
    screenWidth = width;
    screenHeight = height;
}

int RaySelector::getVertexIndex(QuadMesh *mesh, glm::vec2 pixelCords)
{
    int selectedIdx = -1;
    float dist;
    float tempDist;
    dist = std::numeric_limits<float>::infinity();
    // Get normalized device coordinates of ray
    glm::vec3 ray_nds;
    ray_nds = glm::vec3(
                        Utilities::normalized_device_cordinates(pixelCords,
                                                                screenWidth,
                                                                screenHeight),
                                                                1.0f);


    // Get clip space coordinates of ray
    glm::vec4 ray_clip(ray_nds.x, ray_nds.y, -1.0, 1.0);
    // Get get eye space coordinates
    glm::mat4 inverseProj = glm::inverse(*projM);
    glm::vec4 ray_eye = inverseProj * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

    // Get world coordinates
    glm::mat4 inverseView = glm::inverse(*viewM);
    glm::vec4 ray_world = (inverseView * ray_eye);
    glm::vec4 ray_dir(glm::normalize(ray_world));
    glm::vec4 ray_origin = glm::vec4(*cameraOrigin, 1.0f);

    QuadMesh::VertexIter v_it(mesh->vertices_sbegin());
    for(;v_it != mesh->vertices_end(); ++v_it)
    {
        QuadMesh::Point p = mesh->point(*v_it);
        QuadMesh::Normal n = mesh->normal(*v_it);
        glm::vec4 normal(n[0],n[1],n[2], 0.0f);
        glm::vec4 vertPoint(p[0],p[1],p[2], 1.0f);
        vertPoint = *modelM * vertPoint;
        normal = *modelM * normal;
        // make sure vertex is facing camera
        float angle = glm::dot(normal, ray_dir);
        if( angle < 0 )
        {
            if(Utilities::intersect(ray_dir, ray_origin, vertPoint, tempDist))
            {
                // If the new vertex is closer to the ray, remember it
                if(tempDist < dist)
                {
                    dist = tempDist;
                    selectedIdx = (*v_it).idx();
                }
            }
        }
    }
    return selectedIdx;
}