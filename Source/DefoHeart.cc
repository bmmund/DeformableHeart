#include "DefoHeart.hpp"
#include "trackball.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <sstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

int const windowWidth = 640;
int const windowHeight = 480;
std::string const application_name = "DefoHeart";

DefoHeart::DefoHeart()
    : App(windowWidth, windowHeight, application_name),
      mesh(HEART_MESH_PATH),
      subDivider()
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

    #if defined(USE_OPENGL_LEGACY)
        updateGeometries();
    #else
    #endif
}


void DefoHeart::updateGeometries()
{
    float ratio;
    int width, height;
    width = getWidth();
    height = getHeight();
    ratio = width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMultMatrixf(&(modelM[0][0]));

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glEnable( GL_POLYGON_OFFSET_FILL );
    glPolygonOffset( 1, 1 );
    glColor3f(0.4f, 0.f, 0.f);
    drawMesh();
    glDisable( GL_POLYGON_OFFSET_FILL );

    // draw the wireframe
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glColor3f(0.0f, 0.f, 0.f);
    drawMesh();

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glColor3f(1.0, 1.0, 1.0);
    drawMeshPoints();
    glFlush();
}

void DefoHeart::drawMesh()
{
    glBegin(GL_TRIANGLES);
    TriMesh* meshPtr = mesh.getTriMesh();
    for(TriMesh::FaceIter f_it = meshPtr->faces_sbegin();
        f_it != meshPtr->faces_end(); ++f_it)
    {
        for(TriMesh::FaceVertexIter fv_it = meshPtr->fv_iter(*f_it);
            fv_it.is_valid();
            ++fv_it)
        {
            if (!meshPtr->has_vertex_normals())
            {
                std::cerr << "ERROR: Standard vertex property 'Normals' not available!\n";
                return;
            }
            TriMesh::Normal n = meshPtr->normal(*fv_it);
            glNormal3fv(n.data());
            TriMesh::Point p = meshPtr->point(*fv_it);
            glVertex3fv(p.data());
        }
    }
    glEnd();
}

void DefoHeart::drawMeshPoints()
{
    glPointSize(5);
    glBegin(GL_POINTS);
    TriMesh* meshPtr = mesh.getTriMesh();
    for(TriMesh::VertexIter v_it = meshPtr->vertices_sbegin();
        v_it != meshPtr->vertices_end(); ++v_it)
    {
        TriMesh::Point p = meshPtr->point(*v_it);
        glVertex3fv(p.data());
    }
    glEnd();
}

void DefoHeart::drawMeshPoint(int index)
{
    TriMesh* meshPtr = mesh.getTriMesh();
    // Check if index is out of bounds
    if( index < 0 || index >= meshPtr->n_vertices())
        return;
    glPointSize(5);
    glBegin(GL_POINTS);
    TriMesh::Point p = meshPtr->point(meshPtr->vertex_handle(index));
    glVertex3fv(p.data());
    glEnd();
}

void DefoHeart::initializeGL()
{
    // OpenGL state
    glEnable(GL_DEPTH_TEST);
    modelM = glm::mat4(1.0);
    modelM = glm::rotate(modelM, -90.0f, glm::vec3(1.f, 0, 0));
    modelM = glm::scale(modelM, glm::vec3(0.009));
    modelM = glm::translate(modelM, glm::vec3(0, 0, -50));
}

void DefoHeart::keyCallbackImp(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else if(key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        subDivider.subdivide(mesh.getTriMesh());
    }
    else if(key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        modelM = glm::rotate(modelM, 15.0f, glm::vec3(1.f, 0.f, 0.f));
    }

}

void DefoHeart::mousePositionCallbackImp(GLFWwindow* window, double xpos, double ypos)
{
    static int prevX = xpos;
    static int prevY= ypos;
    float fDiameter;
    int iCenterX, iCenterY;
    float fNewModX, fNewModY, fOldModX, fOldModY;
    float fRotVecX, fRotVecY, fRotVecZ;
    glm::mat4 rotationM;
    /* vCalcRotVec expects new and old positions in relation to the center of the
     * trackball circle which is centered in the middle of the window and
     * half the smaller of nWinWidth or nWinHeight.
     */
    fDiameter = (getWidth() < getHeight()) ? getWidth() * 0.7 : getHeight() * 0.7;
    iCenterX = getWidth() / 2;
    iCenterY = getHeight() / 2;
    fOldModX = prevX - iCenterX;
    fOldModY = prevY - iCenterY;
    fNewModX = xpos - iCenterX;
    fNewModY = ypos - iCenterY;
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int shiftKey = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	if( (state == GLFW_PRESS) && (shiftKey == GLFW_PRESS) )
	{
        vCalcRotVec(fNewModX, fNewModY,
                    fOldModX, fOldModY,
                    fDiameter,
                    &fRotVecX, &fRotVecY, &fRotVecZ);
        vAxisRotMatrix(fRotVecX, -fRotVecY, fRotVecZ, rotationM);
        modelM = rotationM * modelM;
	}
    else
    {
        rotationM = glm::mat4(1.0);
    }
    prevX = xpos;
    prevY = ypos;
}

void DefoHeart::mouseClickCallbackImp(GLFWwindow* window, int button, int action, int mods)
{
}

void DefoHeart::scrollCallbackImp(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset > 0)
	{
        glm::mat4 scaling = glm::scale(glm::mat4(1.0), glm::vec3(1.1));
        modelM = scaling * modelM;
	}
	else
	{
		glm::mat4 scaling = glm::scale(glm::mat4(1.0), glm::vec3(0.9));
        modelM = scaling * modelM;
	}
}