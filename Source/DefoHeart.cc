#include "DefoHeart.hpp"
#include "trackball.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "polyroots.h"

using namespace std;

int const windowWidth = 640;
int const windowHeight = 480;
std::string const application_name = "DefoHeart";
const double TRACKBALL_RADIUS = 4;

DefoHeart::DefoHeart()
    : App(windowWidth, windowHeight, application_name),
      mesh(HEART_MESH_PATH),
      subDivider(),
    selectedIdx(-1),
    useNormal(true),
    useLighting(true)
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
//    projM = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    projM = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
//    glOrtho(-1.f, 1.f, -1.f, 1.f, 1.f, -1.f);
//    glMultMatrixf(&(projM[0][0]));
    glLoadMatrixf(&(projM[0][0]));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glm::mat4 modelViewM = viewM * modelM;
    glLoadMatrixf(&(modelViewM[0][0]));

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glEnable( GL_POLYGON_OFFSET_FILL );
    glPolygonOffset( 1, 1 );
    glColor3f(0.4f, 0.f, 0.f);
    drawMesh();
    glDisable( GL_POLYGON_OFFSET_FILL );

    glDisable(GL_LIGHTING);
    // draw the wireframe
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glColor3f(0.0f, 0.f, 0.f);
    drawMesh();

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glColor3f(1.0, 1.0, 1.0);
    drawMeshPoints();

    //    glColor3f(0.f, 1.f, 0.f);
    //    drawMeshNormals();

    drawModelGnomenPoints();

    if(useLighting)
        glEnable(GL_LIGHTING);
    glFlush();
}

void DefoHeart::drawMesh()
{
    setHeartMaterial();
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

void DefoHeart::drawMeshNormals()
{
    glBegin(GL_LINES);
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
            TriMesh::Point p = meshPtr->point(*fv_it);
            glVertex3fv(p.data());
            glVertex3fv(n.data());
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

void DefoHeart::drawModelGnomenPoints()
{
    glPointSize(5);
    glBegin(GL_POINTS);
    glColor3f(0.4f, 0.f, 1.f);
    glVertex3f(0, 0, 0);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(1.f, 0, 0);
    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(0, 1.0f, 0);
    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(0, 0, 1.f);
    glEnd();
}
//----------------------------------------------------------------------------

void DefoHeart::setDefaultMaterial(void)
{
    GLfloat mat_a[] = {0.1, 0.1, 0.1, 1.0};
    GLfloat mat_d[] = {0.7, 0.7, 0.5, 1.0};
    GLfloat mat_s[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shine[] = {120.0};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_a);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_s);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);
}


void DefoHeart::setHeartMaterial(void)
{
    GLfloat mat_a[] = {0.1, 0.0, 0., 1.0};
    GLfloat mat_d[] = {0.4, 0.2, 0.2, 1.0};
    GLfloat mat_s[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shine[] = {120.0};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_a);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_s);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);
}


//----------------------------------------------------------------------------

void DefoHeart::setDefaultLight(void)
{
    GLfloat pos1[] = { 0.1,  0.1, -0.02, 0.0};
    GLfloat pos2[] = {-0.1,  0.1, -0.02, 0.0};
    GLfloat pos3[] = { 0.0,  0.0,  0.1,  0.0};
    GLfloat pos4[] = { 0.0,  -0.5,  0.5,  0.0};
    GLfloat col1[] = { 0.7,  0.7,  0.8,  1.0};
    GLfloat col2[] = { 0.8,  0.7,  0.7,  1.0};
    GLfloat col3[] = { 1.0,  1.0,  1.0,  1.0};

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0,GL_POSITION, pos1);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,  col1);
    glLightfv(GL_LIGHT0,GL_SPECULAR, col1);

    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1,GL_POSITION, pos2);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,  col2);
    glLightfv(GL_LIGHT1,GL_SPECULAR, col2);

    glEnable(GL_LIGHT2);
    glLightfv(GL_LIGHT2,GL_POSITION, pos3);
    glLightfv(GL_LIGHT2,GL_DIFFUSE,  col3);
    glLightfv(GL_LIGHT2,GL_SPECULAR, col3);

    glEnable(GL_LIGHT3);
    glLightfv(GL_LIGHT3,GL_POSITION, pos4);
    glLightfv(GL_LIGHT3,GL_DIFFUSE,  col3);
    glLightfv(GL_LIGHT3,GL_SPECULAR, col3);
}

void DefoHeart::initializeGL()
{
    // OpenGL state
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);
    glEnable(GL_LIGHTING);
    // Material
    setDefaultMaterial();

    // Lighting
    glLoadIdentity();
    setDefaultLight();

    // Fog
    GLfloat fogColor[4] = { 0.3, 0.3, 0.4, 1.0 };
    glFogi(GL_FOG_MODE,    GL_LINEAR);
    glFogfv(GL_FOG_COLOR,  fogColor);
    glFogf(GL_FOG_DENSITY, 0.35);
    glHint(GL_FOG_HINT,    GL_DONT_CARE);
    glFogf(GL_FOG_START,    5.0f);
    glFogf(GL_FOG_END,     25.0f);

    modelM = glm::mat4(1.0);
    cameraOrigin = glm::vec3(0.f, 2.f, 2.f);
    viewM = glm::lookAt(cameraOrigin, glm::vec3(0,0,0), glm::vec3(0,1,0));
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
    else if(((key == GLFW_KEY_UP) || (key == GLFW_KEY_DOWN)) && action == GLFW_PRESS)
    {
        if(selectedIdx != -1)
        {
            TriMesh::VertexHandle vh(mesh.getTriMesh()->vertex_handle(selectedIdx));
            TriMesh::Normal n = mesh.getTriMesh()->normal(vh);
            TriMesh::Point p = mesh.getTriMesh()->point(vh);
            glm::vec3 point(p[0],p[1],p[2]);
            glm::vec3 normal(n[0],n[1],n[2]);
            glm::vec3 displace(0,0,0);
            displace = point - displace;
            if(useNormal)
                displace = glm::normalize(normal);
            else
                displace = glm::normalize(displace);

            if(key == GLFW_KEY_DOWN)
            {
                displace *= -1;
            }
//            displace = glm::normalize(displace);
            cout<<"displace:"<<displace.x<<" "<< displace.y<< " "<< displace.z<<std::endl;
            cout<<"normal:"<<normal.x<<" "<< normal.y<< " "<< normal.z<<std::endl;

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
                rotationM = glm::rotate(rotationM, angle, axisRot);
                modelM = rotationM * modelM;
            }
        }
    }
    // remember this point
    last_point_2D_ = newPoint2D;
    last_point_3D_ = newPoint3D;
    last_point_ok_ = newPoint_hitSphere;
}
bool DefoHeart::intersect(glm::vec4 &ray_dir, glm::vec4 &ray_origin, glm::vec4 &center, float &t_out)
{
    static float const radius2 = 0.0005f;
    double t[] = {0,0}; // solution(s) to sphere intersection
    glm::vec4 L = ray_origin - center;
    double a = glm::dot(ray_dir, ray_dir);
    double b = 2 * glm::dot(ray_dir, L);
    double c = glm::dot(L,L) - (radius2);
    int roots = (int)quadraticRoots(a, b, c, t);
    if (!roots) return false;

    // order roots such that t0 is less than t1
    if (t[0] > t[1]) std::swap(t[0], t[1]);

    // make sure that the roots are not negative
    if (t[0] < 0) {
        t[0] = t[1]; // if t0 is negative, let's use t1 instead
        if (t[0] < 0) return false; // both t0 and t1 are negative
    }
    t_out = t[0];
    return true;
}

void DefoHeart::mouseClickCallbackImp(GLFWwindow* window, int button, int action, int mods)
{
        double mouseX,mouseY;
        int wwidth, wheight;
        glfwGetWindowSize(window, &wwidth, &wheight);
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
                float dist;
                float tempDist;
                dist = numeric_limits<float>::infinity();

                // Get normalized device coordinates of ray
                glm::vec3 ray_nds;
                ray_nds.x = (2.0f * mouseX) / getWidth() - 1.0f;
                ray_nds.y = 1.0f - (2.0f * mouseY) / getHeight();
                ray_nds.z = 1.0f;

                // Get clip space coordinates of ray
                glm::vec4 ray_clip(ray_nds.x, ray_nds.y, -1.0, 1.0);

                // Get get eye space coordinates
                glm::mat4 inverseProj = glm::inverse(projM);
                glm::vec4 ray_eye = inverseProj * ray_clip;
                ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

                // Get world coordinates
                glm::mat4 inverseView = glm::inverse(viewM);
                glm::vec4 ray_world = (inverseView * ray_eye);
                glm::vec4 ray_dir(glm::normalize(ray_world));
                glm::vec4 ray_origin = glm::vec4(cameraOrigin, 1.0f);

                TriMesh* meshPtr = mesh.getTriMesh();
                TriMesh::VertexIter v_it(meshPtr->vertices_sbegin());
                for(;v_it != meshPtr->vertices_end(); ++v_it)
                {
                    TriMesh::Point p = meshPtr->point(*v_it);
                    TriMesh::Normal n = meshPtr->normal(*v_it);
                    glm::vec4 normal(n[0],n[1],n[2], 0.0f);
                    glm::vec4 vertPoint(p[0],p[1],p[2], 1.0f);
                    vertPoint = modelM * vertPoint;
                    normal = modelM * normal;
                    // make sure vertex is facing camera
                    float angle = glm::dot(normal, ray_dir);
                    if( angle < 0 )
                    {
                        if(intersect(ray_dir, ray_origin, vertPoint, tempDist))
                        {
                            if(tempDist < dist)  // If new t is smaller than min
                            {
                                dist = tempDist;
                                selectedIdx = (*v_it).idx();
                            }
                    }
                    }
                }
            }
        }
    }
}

void DefoHeart::scrollCallbackImp(GLFWwindow* window, double xoffset, double yoffset)
{
    static float const zoom_factor = 1.0f;
	if (yoffset > 0)
	{
//        glm::mat4 scaling = glm::scale(glm::mat4(1.0), glm::vec3(1.1));
//        modelM = scaling * modelM;
        glm::vec3 dir = glm::normalize(cameraOrigin);
        cameraOrigin = cameraOrigin + (zoom_factor * dir);
        viewM = glm::lookAt(cameraOrigin, glm::vec3(0,0,0), glm::vec3(0,1,0));
	}
	else
	{
//		glm::mat4 scaling = glm::scale(glm::mat4(1.0), glm::vec3(0.9));
//        modelM = scaling * modelM;
        glm::vec3 dir = glm::normalize(cameraOrigin);
        cameraOrigin = cameraOrigin + (-zoom_factor * dir);
        viewM = glm::lookAt(cameraOrigin, glm::vec3(0,0,0), glm::vec3(0,1,0));
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
