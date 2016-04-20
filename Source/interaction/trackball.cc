#include <cmath>
#include "trackball.hpp"
#include <glm/gtc/matrix_transform.hpp>

/*
 *  A mouse interaction technique based on Ken Shoemake's ArcBall,
 *  Graphics Gems IV, 1993
 */

Trackball::Trackball(int width, int height, double radius)
    :   screenWidth(width),
        screenHeight(height),
        _radius(radius),
        _radius2(radius*radius),
        valid(false)
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
    prevBallPoint = mapToTrackball(mousePos);
    valid = true;
}

void Trackball::inValidate()
{
    valid = false;
    prevBallPoint = glm::vec3(0);
}

glm::mat4 Trackball::getRotation(glm::vec2 mousePos)
{
    glm::mat4 rotationM(1.0f);
    glm::vec3  newBallPoint;
    newBallPoint = mapToTrackball(mousePos);
    if (isValid()) {
        // retrieve axis of rotation which is the cross product of
        // the old position vector and new position vector
        glm::vec3 axis = glm::cross(prevBallPoint, newBallPoint);
        if (glm::dot(axis, axis) <= 0) {
            axis = glm::vec3(1, 0, 0);
        } else {
            axis = glm::normalize(axis);
        }
        // find the amount of rotation by using arcsine of the ratio
        glm::vec3 d = prevBallPoint - newBallPoint;
        float t = 0.5 * glm::length(d) / _radius;
        // clamp range
        t = Utilities::clamp(t, -1.0, 1.0);
        float angle = 2.0 * asin(t);
        rotationM = glm::rotate(rotationM, glm::degrees(angle), axis);
        prevBallPoint = newBallPoint;
    }
    return rotationM;
}

glm::vec3 Trackball::mapToTrackball(const glm::vec2& screenPoint)
{
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