#ifndef __UTILITIES_HPP
#define __UTILITIES_HPP

#include <glm/glm.hpp>
#include <string>
namespace Utilities {
    bool intersect(glm::vec4 &ray_dir, glm::vec4 &ray_origin, glm::vec4 &center, float &t_out);
    glm::vec2 normalized_device_cordinates(const glm::vec2 &pixelCords, int width, int height);
    float clamp(float value, float lowerBound, float upperBound);
    std::string readFileToString(const char * path);
}
#endif /* __UTILITIES_HPP */
