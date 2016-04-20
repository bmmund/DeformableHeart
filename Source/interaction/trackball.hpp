#ifndef __TRACKBALL_HPP
#define __TRACKBALL_HPP

#include <glm/glm.hpp>
#include "utilities.hpp"

const double TRACKBALL_RADIUS = 4;

class Trackball {
public:
    Trackball(int width, int height, double radius = TRACKBALL_RADIUS);
    void setTrackball(int width, int height);
    void setTrackball(int width, int height, double radius);
    void update(glm::vec2 mousePos);
    bool isValid() {return valid;}
    void inValidate();
    glm::mat4 getRotation(glm::vec2 mousePos);
private:
    int screenWidth;
    int screenHeight;
    double _radius;
    double _radius2;
    bool valid;
    glm::vec3 prevBallPoint;
    glm::vec3 mapToTrackball(const glm::vec2& screenPoint);
};
#endif
