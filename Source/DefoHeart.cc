#include "DefoHeart.hpp"
#include "trackball.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

int const windowWidth = 640;
int const windowHeight = 480;
float xvec;
float yvec;
float zvec;
double angle;
double prevX;
double prevY;
double scaleval = 1;
bool rotating;
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

//    glRotatef((float) glfwGetTime() * 50.f, 0.f, 1.f, 0.f);

    glRotatef( 270, 1.f, 0.f, 0.f);
    glScalef(0.01,0.01,0.01);
    glTranslatef(0, 0, -50);
    glPopMatrix();
    glRotatef(angle, xvec, yvec, zvec);
	glPushMatrix();
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
}

void DefoHeart::keyCallbackImp(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else if(key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        subDivider.subdivide(mesh.getTriMesh());
    }

}

void DefoHeart::mousePositionCallbackImp(GLFWwindow* window, double xpos, double ypos)
{
	if (rotating)
	{
		angle = 1;
		float diameter = (windowWidth < windowHeight) ? windowWidth * 0.5 : windowHeight * 0.5;
		vCalcRotVec((xpos-(windowWidth/2)), (ypos - (windowHeight / 2)),
			(prevX - (windowWidth / 2)), (prevY - (windowHeight / 2)),
			diameter,
			&xvec, &yvec, &zvec);
		prevX = xpos;
		prevY = ypos;
	}
    else
    {
        angle = 0;
        xvec = 0;
        yvec = 0;
        zvec = 0;
    }
}

void DefoHeart::mouseClickCallbackImp(GLFWwindow* window, int button, int action, int mods)
{
	if (button == 0 && action == 1)
	{
		rotating = true;
		glfwGetCursorPos(window, &prevX, &prevY);
	}
	if (button == 0 && action == 0)
	{
		rotating = false;
	}
}

void DefoHeart::scrollCallbackImp(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset > 0)
	{
		scaleval = 1.1;
	}
	else
	{
		scaleval = 0.9;
	}
}