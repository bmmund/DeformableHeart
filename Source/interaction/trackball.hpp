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
    bool isValid() {return last_point_ok_;}
    void inValidate();
    glm::mat4 getRotation(glm::vec2 mousePos);
private:
    int screenWidth;
    int screenHeight;
    double _radius;
    double _radius2;
    glm::vec2 last_point_2D_;
    glm::vec3 last_point_3D_;
    bool      last_point_ok_;
    glm::vec3 map_screen_to_trackball(const glm::vec2& screenPoint);
};
#endif
