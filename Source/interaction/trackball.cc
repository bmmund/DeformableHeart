#include <cmath>
#include "trackball.hpp"
#include <glm/gtc/matrix_transform.hpp>

Trackball::Trackball(int width, int height, double radius)
    :   screenWidth(width),
        screenHeight(height),
        _radius(radius),
        _radius2(radius*radius),
        last_point_ok_(false)
{

}

void Trackball::setTrackball(int width, int height)
{
    setTrackball(width, height, _radius);
}

void Trackball::setTrackball(int width, int height, double radius)
{
    screenWidth = width;
    screenHeight = height;
    _radius = radius;
    _radius2 = radius * radius;
}

void Trackball::update(glm::vec2 mousePos)
{
    last_point_2D_ = mousePos;
    last_point_3D_ = map_screen_to_trackball(last_point_2D_);
    last_point_ok_ = true;
}

void Trackball::inValidate()
{
    last_point_ok_ = false;
    last_point_3D_ = glm::vec3(0);
    last_point_2D_ = glm::vec2(0);
}

glm::mat4 Trackball::getRotation(glm::vec2 mousePos)
{
    glm::mat4 rotationM;
    glm::vec3  newPoint3D;
    newPoint3D = map_screen_to_trackball(mousePos);
    if (isValid()) {
        glm::vec3 axis = glm::cross(last_point_3D_, newPoint3D);
        if (glm::dot(axis, axis) < 1e-7) {
            axis = glm::vec3(1, 0, 0);
        } else {
            axis = glm::normalize(axis);
        }
        // find the amount of rotation
        glm::vec3 d = last_point_3D_ - newPoint3D;
        float t = 0.5 * glm::length(d) / _radius;
        if (t < -1.0)
            t = -1.0;
        else if (t > 1.0)
            t = 1.0;
        float phi = 2.0 * asin(t);
        float angle = phi * 180.0 / M_PI;
        glm::vec3 axisRot(axis[0], axis[1], axis[2]);
        rotationM = glm::rotate(rotationM, angle, axisRot);
        last_point_3D_ = newPoint3D;
    }
    return rotationM;
}

glm::vec3 Trackball::map_screen_to_trackball(const glm::vec2& screenPoint)
{
    // This is actually doing the Sphere/Hyperbolic sheet hybrid thing,
    // based on Ken Shoemake's ArcBall in Graphics Gems IV, 1993.
    glm::vec3 trackballCords;
    // get coordinates in NDC
    glm::vec2 ndc = Utilities::normalized_device_cordinates(screenPoint, screenWidth, screenHeight);
    double x2y2 = ndc.x*ndc.x + ndc.y*ndc.y;

    trackballCords = glm::vec3(ndc,0.0f);
    if (x2y2 < 0.5*_radius2) {
        trackballCords.z = sqrt(_radius2 - x2y2);
    } else {
        trackballCords.z = 0.5*_radius2/sqrt(x2y2);
    }
    return trackballCords;
}