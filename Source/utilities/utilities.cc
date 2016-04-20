#include "utilities.hpp"
#include "polyroots.hpp"

bool Utilities::intersect(glm::vec4 &ray_dir, glm::vec4 &ray_origin, glm::vec4 &center, float &t_out)
{
    static float const radius2 = 0.0005f;
    double t[] = {0,0}; // solution(s) to sphere intersection
    glm::vec4 L = ray_origin - center;
    double a = glm::dot(ray_dir, ray_dir);
    double b = 2 * glm::dot(ray_dir, L);
    double c = glm::dot(L,L) - (radius2);
    int roots = (int)quadraticRoots(a, b, c, t);
    if (!roots) return false;

    // order roots such that t0 is less than t1
    if (t[0] > t[1]) std::swap(t[0], t[1]);

    // make sure that the roots are not negative
    if (t[0] < 0) {
        t[0] = t[1]; // if t0 is negative, let's use t1 instead
        if (t[0] < 0) return false; // both t0 and t1 are negative
    }
    t_out = t[0];
    return true;
}

glm::vec2 Utilities::normalized_device_cordinates(const glm::vec2 &pixelCords, int width, int height)
{
    glm::vec2 normDeviceCords;
    normDeviceCords.x = (2.0f * pixelCords.x) / width - 1.0f;
    normDeviceCords.y = 1.0f - (2.0f * pixelCords.y) / height;
    return normDeviceCords;
}

float Utilities::clamp(float value, float lowerBound, float upperBound)
{
    float temp;
    if(lowerBound > upperBound)
    {
        temp = lowerBound;
        lowerBound = upperBound;
        upperBound = temp;
    }
    return value < lowerBound ? lowerBound : (value > upperBound ? upperBound : value);
}
