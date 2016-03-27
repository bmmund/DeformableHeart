#ifndef __DEFO_HEART__HPP
#define __DEFO_HEART__HPP

#include "app.hpp"

class DefoHeart : public App
{
public:
    DefoHeart();
    ~DefoHeart();
    void loop();
private:
    void keyCallbackImp(GLFWwindow* window, int key, int scancode, int action, int mods);

};
#endif /* __DEFO_HEART__HPP */
