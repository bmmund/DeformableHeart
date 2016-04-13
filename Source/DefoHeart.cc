#include "DefoHeart.hpp"
#include "trackball.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

int const windowWidth = 640;
int const windowHeight = 480;
std::string const application_name = "DefoHeart";
const double TRACKBALL_RADIUS = 4;

DefoHeart::DefoHeart()
    : App(windowWidth, windowHeight, application_name),
      mesh(HEART_MESH_PATH),
      subDivider(),
    selectedIdx(-1)
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
//    glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    projM = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
//    glOrtho(-1.f, 1.f, -1.f, 1.f, 1.f, -1.f);
//    glMultMatrixf(&(projM[0][0]));
    glLoadMatrixf(&(projM[0][0]));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPointSize(5);
        glColor3f(0.4f, 0.f, 1.f);
    glBegin(GL_POINTS);
    glVertex3f(0, 0, 0);
    glEnd();


    glLoadMatrixf(&(modelM[0][0]));

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
        if(selectedIdx == (*v_it).idx())
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
    modelM = glm::translate(modelM, glm::vec3(0, -0.5, 0));
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
        modelM = glm::rotate(modelM, glm::radians(10.0f), glm::vec3(0.f, 1.f, 0.f));
    }
    else if(((key == GLFW_KEY_UP) || (key == GLFW_KEY_DOWN)) && action == GLFW_PRESS)
    {
        if(selectedIdx != -1)
        {
            TriMesh::VertexHandle vh(mesh.getTriMesh()->vertex_handle(selectedIdx));
//            TriMesh::Normal n = mesh.getTriMesh()->calc_vertex_normal(vh);
            TriMesh::Point p = mesh.getTriMesh()->point(vh);
            glm::vec3 point(p[0],p[1],p[2]);
            glm::vec3 displace(0,0,0);
            displace = point - displace;
            displace = glm::normalize(displace);
            if(key == GLFW_KEY_DOWN)
            {
                displace *= -1;
            }
//            displace = glm::normalize(displace);
            cout<<"displace:"<<displace.x<<" "<< displace.y<< " "<< displace.z<<std::endl;

            point = point + (0.10f * displace);
//            p = p + (0.05 * n);
            for(int i = 0 ; i<3; i++)
                p[i] = point[i];
            mesh.getTriMesh()->set_point(vh, p);
        }
    }
}

void DefoHeart::mousePositionCallbackImp(GLFWwindow* window, double xpos, double ypos)
{
    glm::vec2 newPoint2D(xpos, ypos);
    OpenMesh::Vec3f  newPoint3D;
    bool   newPoint_hitSphere = map_to_sphere( newPoint2D, newPoint3D );
    int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int shiftKey = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
    // rotate
    if ( mouseState && shiftKey)
    {
        if (last_point_ok_) {
            if ((newPoint_hitSphere = map_to_sphere(newPoint2D, newPoint3D))) {
                OpenMesh::Vec3f axis = last_point_3D_ % newPoint3D;
                if (axis.sqrnorm() < 1e-7) {
                    axis = OpenMesh::Vec3f(1, 0, 0);
                } else {
                    axis.normalize();
                }
                // find the amount of rotation
                OpenMesh::Vec3f d = last_point_3D_ - newPoint3D;
                float t = 0.5 * d.norm() / TRACKBALL_RADIUS;
                if (t < -1.0)
                    t = -1.0;
                else if (t > 1.0)
                    t = 1.0;
                float phi = 2.0 * asin(t);
                float angle = phi * 180.0 / M_PI;
                glm::vec3 axisRot(axis[0], axis[1], axis[2]);
                glm::mat4 rotationM;
                rotationM = glm::rotate(rotationM, -angle, axisRot);
                modelM = rotationM * modelM;
            }
        }
    }
    // remember this point
    last_point_2D_ = newPoint2D;
    last_point_3D_ = newPoint3D;
    last_point_ok_ = newPoint_hitSphere;
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
                last_point_2D_.x = mouseX;
                last_point_2D_.y = mouseY;
                last_point_ok_ = map_to_sphere( last_point_2D_, last_point_3D_ );
            }
            else
            {
                double mouseX,mouseY;
                glm::vec4 point;
                glm::vec4 vect(0,0,-1,0.f);
                float dist;
                float tempDist;
                glfwGetCursorPos(window, &mouseX, &mouseY);
                point.x = ((mouseX/getWidth())*2.0f)-1;
                point.y = ((mouseY/getHeight())*(-2.0f))+1;
                point.z = 1.f;
                point.w = 1.f;
                std::cout<<"point:"<< point.x<<" "<< point.y<< " "<< point.z<<std::endl;
            //    vect
                // for all verts in mesh, get closest intersection
                glm::mat4 inverseModel = glm::inverse(modelM);
                glm::mat4 inverseProj = glm::inverse(projM);
                point = point * inverseProj;
                point = point*inverseModel;

                vect = vect * inverseProj;
                vect = vect * inverseModel;
                std::cout<<"point new:"<< point.x<<" "<< point.y<< " "<< point.z<<std::endl;
                TriMesh* meshPtr = mesh.getTriMesh();
                TriMesh::VertexIter v_it(meshPtr->vertices_sbegin());
                TriMesh::VertexHandle _vh(*v_it);
                TriMesh::Point p = meshPtr->point(_vh);
                glm::vec3 rayOrgToPoint(p[0],p[1],p[2]);
                rayOrgToPoint = rayOrgToPoint - glm::vec3(point);
                rayOrgToPoint = glm::normalize(rayOrgToPoint);
                dist = glm::length(glm::cross(glm::vec3(vect), rayOrgToPoint));
                cout<<"length:"<<dist<<std::endl;
                for(;v_it != meshPtr->vertices_end(); ++v_it)
                {
                    p = meshPtr->point(*v_it);
                    glm::vec3 rayOrgToPoint(p[0],p[1],p[2]);
                    rayOrgToPoint = rayOrgToPoint - glm::vec3(point);
                    rayOrgToPoint = glm::normalize(rayOrgToPoint);
                    tempDist = glm::length(glm::cross(glm::vec3(vect), rayOrgToPoint));
                    if(tempDist<dist)
                    {
                        dist = tempDist;
                        _vh = *v_it;
                    }
                }
                cout<<"closest vert at vert "<<_vh.idx()<<std::endl;
                selectedIdx = _vh.idx();
            }
        }
    }
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

bool DefoHeart::map_to_sphere( const glm::vec2& _v2D, OpenMesh::Vec3f& _v3D )
{
    // This is actually doing the Sphere/Hyperbolic sheet hybrid thing,
    // based on Ken Shoemake's ArcBall in Graphics Gems IV, 1993.
    double x =  (2.0*_v2D.x - getWidth())/getWidth();
    double y = -(2.0*_v2D.y - getHeight())/getHeight();
    double xval = x;
    double yval = y;
    double x2y2 = xval*xval + yval*yval;

    const double rsqr = TRACKBALL_RADIUS*TRACKBALL_RADIUS;
    _v3D[0] = xval;
    _v3D[1] = yval;
    if (x2y2 < 0.5*rsqr) {
        _v3D[2] = sqrt(rsqr - x2y2);
    } else {
        _v3D[2] = 0.5*rsqr/sqrt(x2y2);
    }

    return true;
}
