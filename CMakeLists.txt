# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
project (DeformableHeart)
cmake_minimum_required (VERSION 3.1.3)

# this is the directory where the currently processed CMakeLists.txt is located in
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Set the directory where the executables will be stored.
set(EXECUTABLE_OUTPUT_PATH
    "${PROJECT_BINARY_DIR}/bin"
    CACHE PATH
    "Directory where executables will be stored"
)
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Locate Dependancies
#-------------------------------------------------------------------------------

#########################################################
# FIND GLFW
#########################################################
# GLFW_FOUND
# GLFW_INCLUDE_DIR
# GLFW_LIBRARIES
find_package(GLFW REQUIRED)

#########################################################
# FIND OPENGL
#########################################################
# OPENGL_FOUND        - system has OpenGL
# OPENGL_XMESA_FOUND  - system has XMESA
# OPENGL_GLU_FOUND    - system has GLU
# OPENGL_INCLUDE_DIR  - the GL include directory
# OPENGL_LIBRARIES    - Link these to use OpenGL and GLU
find_package(OpenGL REQUIRED)

#########################################################
# FIND GLEW
#########################################################
# GLEW_INCLUDE_DIRS - include directories for GLEW
# GLEW_LIBRARIES - libraries to link against GLEW
# GLEW_FOUND - true if GLEW has been found and can be used
find_package(GLEW REQUIRED)

#########################################################
# FIND GLM
#########################################################
# GLM_INCLUDE_DIRS  - where to find glm/glm.hpp
# GLM_FOUND         - if the library was successfully located
find_package(GLM REQUIRED)

#########################################################
# FIND Qt5
#########################################################
# Qt5Widgets_LIBRARIES      - librariers to link agianst Qt
# Qt5Widgets_INCLUDE_DIRS   - include directories for Qt
# Qt5Widgets_FOUND          - true it Qt5Widgets is found
#find_package(Qt5Widgets REQUIRED)

#########################################################
# FIND OpenMesh
#########################################################
# OPENMESH_FOUND           - system has OPENMESH
# OPENMESH_INCLUDE_DIRS    - the OPENMESH include directories
# OPENMESH_LIBRARIES       - Link these to use OPENMESH
# OPENMESH_LIBRARY_DIR     - directory where the libraries are included
find_package(OpenMesh REQUIRED)

set(MISSING "")

if (NOT OPENGL_FOUND)
    list(APPEND MISSING OpenGL)
endif()

if (NOT GLEW_FOUND)
    list(APPEND MISSING glew)
endif()

if (NOT GLFW_FOUND)
    list(APPEND MISSING glfw)
endif()

#if (NOT Qt5Widgets_FOUND)
#    list(APPEND MISSING Qt5Widgets)
#endif()

if (NOT OPENMESH_FOUND)
    list(APPEND MISSING OpenMesh)
endif()

if (NOT (MISSING STREQUAL ""))
    message(WARNING
        "The following libraries could not be found : ${MISSING}.  "
    )
endif()

# Recurse into the "Source" subdirectories. This does not actually cause another
# cmake executable to run. The same process will walk through the project's
# entire directory structure. Calling this here gives it access to the above
# environment variables
add_subdirectory (Source)
