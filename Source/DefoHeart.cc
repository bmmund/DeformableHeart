#include "DefoHeart.hpp"

#include "glad/glad.h"

#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "deformation.hpp"

using namespace std;

int const windowWidth = 640;
int const windowHeight = 480;
std::string const application_name = "DefoHeart";

DefoHeart::DefoHeart()
	: App(windowWidth, windowHeight, application_name),
        shaderPhong("vertex.glsl", "fragment.glsl"),
        shaderSolid("vertexSolid.glsl", "fragSolid.glsl"),
        shaderPoint("vertexPoints.glsl", "fragSolid.glsl"),
        mesh(CUBE_MESH_PATH),
        subDivider(),
        trackball(windowWidth, windowHeight),
        rayselector(windowWidth, windowHeight, &projM, &viewM, &modelM, &cameraOrigin),
        selectedIdx(-1),
        useNormal(true),
        useLighting(true),
        drawWireFrame(true)
{
    initializeGL();
}

DefoHeart::~DefoHeart()
{

}

void DefoHeart::loop()
{
    // Clear the frame buffer
    glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    shaderPhong.Use();
    getAtterLocations(shaderPhong);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelM));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewM));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projM));
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    mesh.Draw();
    glDisable(GL_POLYGON_OFFSET_FILL);

    if (drawWireFrame)
    {
        shaderSolid.Use();
        getAtterLocations(shaderSolid);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelM));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewM));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projM));
        glm::vec3 colour(0.0f, 0.0f, 0.0f);
        glUniform3fv(colourLoc, 1, glm::value_ptr(colour));
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        mesh.Draw();
    }

    // draw points
    shaderPoint.Use();
    getAtterLocations(shaderPoint);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelM));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewM));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projM));
    glPointSize(10);
    mesh.Draw(GL_POINTS);
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void DefoHeart::initializeGL()
{
	glEnable(GL_DEPTH_TEST);
	getAtterLocations(shaderPhong);
    resetModelView();
    resize();
}

void DefoHeart::resetModelView()
{
    modelM = glm::mat4(1.0);
    //modelM = glm::rotate(modelM, glm::radians(180.0f), glm::vec3(0,1,0));
    //modelM = glm::translate(modelM, glm::vec3(0,-0.5,0));
    cameraOrigin = glm::vec3(0.f, 1.0f, 1.4f);
    viewM = glm::lookAt(cameraOrigin, glm::vec3(0,0,0), glm::vec3(0,1,0));
}

void DefoHeart::resize()
{
    glViewport(0, 0, getPixelWidth(), getPixelHeight());
    projM = glm::perspective(45.0f, getWindowRatio(), 0.1f, 100.0f);
    trackball.setTrackball(getWidth(), getHeight());
    rayselector.setWindow(getWidth(), getHeight());
}

void DefoHeart::keyCallbackImp(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else if(key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        subDivider.subdivide(mesh.getACM());
        mesh.updateFaceIndeces();
    }
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        subDivider.decompose(mesh.getACM());
        mesh.updateFaceIndeces();
    }
    else if(key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        drawWireFrame = !drawWireFrame;
    }
    else if(key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        resetModelView();
    }
    else if(key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        if(useNormal)
        {
            std::cout<<"switching to explode displacement\n";
            useNormal = false;
        }
        else
        {
            std::cout<<"switching to normal displacement\n";
            useNormal = true;
        }
    }
    else if(key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        useLighting = !useLighting;
    }
    else if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        modelM = glm::rotate(modelM, glm::radians(45.0f), glm::vec3(0.f, 1.f, 0.f));
    }
    else if(key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        modelM = glm::rotate(modelM, glm::radians(-45.0f), glm::vec3(0.f, 1.f, 0.f));
    }
    else if(((key == GLFW_KEY_UP) || (key == GLFW_KEY_DOWN)) && action == GLFW_PRESS)
    {
        if(selectedIdx != -1)
        {
            if(key == GLFW_KEY_DOWN)
            {
                Deformation::pushVertsIn(mesh.getQuadMesh(), selectedIdx, 0.01f);
				mesh.updateFaceIndeces();
            }
            else
            {
                Deformation::pushVertsOut(mesh.getQuadMesh(), selectedIdx, 0.01f);
				mesh.updateFaceIndeces();
            }
        }
    }
}

void DefoHeart::mousePositionCallbackImp(GLFWwindow* window, double xpos, double ypos)
{
    glm::vec2 newMousePos(xpos, ypos);
    int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int shiftKey = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
    // rotate
    if ( mouseState && shiftKey)
    {
        if (trackball.isValid()) {
            glm::mat4 rotationM = trackball.getRotation(newMousePos);
            modelM = rotationM * modelM;
        }
    }
}

void DefoHeart::mouseClickCallbackImp(GLFWwindow* window, int button, int action, int mods)
{
        double mouseX,mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
    if(action == GLFW_PRESS)
    {
        int shiftKey = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
        if(button == GLFW_MOUSE_BUTTON_2)
        {
            selectedIdx = -1;
        }
        else if(button == GLFW_MOUSE_BUTTON_1)
        {
            if(shiftKey)
            {
                trackball.update(glm::vec2(mouseX, mouseY));
            }
            else
            {
                selectedIdx = rayselector.getVertexIndex(mesh.getQuadMesh(), glm::vec2(mouseX, mouseY));
            }
        }
    }
    else if(action == GLFW_RELEASE)
    {
        if(button == GLFW_MOUSE_BUTTON_1)
            trackball.inValidate();
    }
}

void DefoHeart::scrollCallbackImp(GLFWwindow* window, double xoffset, double yoffset)
{
    static float const zoom_factor = 0.5f;
	if (yoffset > 0)
	{
        glm::vec3 dir = glm::normalize(cameraOrigin);
        cameraOrigin = cameraOrigin + (-zoom_factor * dir);
        viewM = glm::lookAt(cameraOrigin, glm::vec3(0,0,0), glm::vec3(0,1,0));
	}
	else
	{
        glm::vec3 dir = glm::normalize(cameraOrigin);
        cameraOrigin = cameraOrigin + (zoom_factor * dir);
        viewM = glm::lookAt(cameraOrigin, glm::vec3(0,0,0), glm::vec3(0,1,0));
	}
}

void DefoHeart::getAtterLocations(Shader & s)
{
	modelLoc = glGetUniformLocation(s.Program, "model");
	viewLoc = glGetUniformLocation(s.Program, "view");
	projLoc = glGetUniformLocation(s.Program, "projection");
	colourLoc = glGetUniformLocation(s.Program, "colour");
}
