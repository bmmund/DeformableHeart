#include "DefoHeart.hpp"
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "deformation.hpp"

using namespace std;

int const windowWidth = 640;
int const windowHeight = 480;
std::string const application_name = "DefoHeart";

DefoHeart::DefoHeart()
    :   App(windowWidth, windowHeight, application_name),
        mesh(DODEDECAHEDRON_MESH_PATH),
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

    #if defined(USE_OPENGL_LEGACY)
        updateGeometries();
    #else
    #endif
}


void DefoHeart::updateGeometries()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
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
    if(drawWireFrame)
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glColor3f(0.0f, 0.f, 0.f);
        drawMesh();
    }

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glColor3f(1.0, 1.0, 1.0);
    drawMeshPoints();
    drawEvenOddPoints();

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

void DefoHeart::drawEvenOddPoints()
{
    glPointSize(5);
    glBegin(GL_POINTS);
    TriMesh* meshPtr = mesh.getTriMesh();
    for (TriMesh::VertexIter v_it = meshPtr->vertices_sbegin();
        v_it != meshPtr->vertices_end(); ++v_it)
    {
        TriMesh::Point p = meshPtr->point(*v_it);
        OpenMesh::VPropHandleT<bool> isEvenVertex;
        if (!meshPtr->get_property_handle(isEvenVertex, "v_prop_even_odd"))
            break;
        bool isEven = meshPtr->property(isEvenVertex, *v_it);
        if (isEven)
            glColor3f(0.0, 1.0, 0.0);
        else
            glColor3f(0.0, 0.0, 1.0);
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

void DefoHeart::setDefaultLight(void)
{
    GLfloat pos1[] = { 0,  1.0,  1.4,  0.0};
    GLfloat col1[] = { 1,1,1,  1.0};
    GLfloat col2[] = { 0, 0, 0,  1.0};

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0,GL_POSITION, pos1);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,  col1);
    glLightfv(GL_LIGHT0,GL_SPECULAR, col2);

}

void DefoHeart::initializeGL()
{
    // OpenGL state
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
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
        subDivider.subdivide(mesh.getTriMesh());
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
                Deformation::pushVertsIn(mesh.getTriMesh(), selectedIdx, 0.01f);
            }
            else
            {
                Deformation::pushVertsOut(mesh.getTriMesh(), selectedIdx, 0.01f);
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
                selectedIdx = rayselector.getVertexIndex(mesh.getTriMesh(), glm::vec2(mouseX, mouseY));
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
