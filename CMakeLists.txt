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

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXECUTABLE_OUTPUT_PATH}")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_PATH}")

# Set the directory where dependancies are stored
set(DEPENDANCY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependancies")

#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Locate Dependancies
#-------------------------------------------------------------------------------
option(USE_LOCAL_DEPENDANCIES "Use locally installed libraries when possible" OFF)
option(BUILD_UNIT_TESTS "Build google test unit test" OFF)
#########################################################
# FIND GLFW
#########################################################
if(USE_LOCAL_DEPENDANCIES)
    # GLFW_FOUND
    # GLFW_INCLUDE_DIR
    # GLFW_LIBRARIES
    find_package(GLFW REQUIRED)
else()
    # GLFW_LIBRARIES
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(${DEPENDANCY_DIR}/glfw)
    list(APPEND GLFW_LIBRARIES glfw)
endif()

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
# FIND GLAD
#########################################################
# GLAD_LIB - glad library
add_subdirectory(${DEPENDANCY_DIR}/glad)

#########################################################
# FIND GLM
#########################################################
# GLM_INCLUDE_DIRS  - where to find glm/glm.hpp
# GLM_FOUND         - if the library was successfully located
if(NOT USE_LOCAL_DEPENDANCIES)
    set(GLM_ROOT_DIR "${DEPENDANCY_DIR}/glm")
    set(GLM_FOUND TRUE)
endif()
find_package(GLM REQUIRED)

#########################################################
# FIND EIGEN
#########################################################
#  EIGEN3_INCLUDE_DIR - the eigen include directory
if(USE_LOCAL_DEPENDANCIES)
    find_package(Eigen3 REQUIRED)
else()
    add_subdirectory(${DEPENDANCY_DIR}/eigen3)
endif()

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

if(CMAKE_GENERATOR STREQUAL Xcode)
    message(WARNING "Repo's OpenMesh cannot be used when compiling with Xcode."
    "The local version of OpenMesh will be linked with instead."
    "Please use version 6.1 or greater.")
endif()

if(USE_LOCAL_DEPENDANCIES OR (CMAKE_GENERATOR STREQUAL Xcode))
    find_package(OpenMesh REQUIRED)
else()
    set(BUILD_APPS OFF CACHE BOOL "" FORCE)
    set(OPENMESH_BUILD_PYTHON_BINDINGS OFF CACHE BOOL "" FORCE)
    set(OPENMESH_BUILD_PYTHON_UNIT_TESTS OFF CACHE BOOL "" FORCE)
    set(OPENMESH_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
    set(OPENMESH_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    add_subdirectory(${DEPENDANCY_DIR}/OpenMesh)
endif()

#########################################################
# FIND google test
#########################################################
# GTEST_FOUND - Found the Google Testing framework
# GTEST_INCLUDE_DIRS - Include directories
# GTEST_BOTH_LIBRARIES - Both libgtest & libgtest-main
# GTEST_LIBRARIES - libgtest
# GTEST_MAIN_LIBRARIES - libgtest-main
if(BUILD_UNIT_TESTS)
    if(USE_LOCAL_DEPENDANCIES)
        find_package(GTest REQUIRED)
    else()
        add_subdirectory(${DEPENDANCY_DIR}/gtest)
        set(GTEST_LIBRARIES gtest)
        set(GTEST_MAIN_LIBRARIES gtest_main)
        list(APPEND GTEST_BOTH_LIBRARIES
            "${GTEST_LIBRARIES}"
            "${GTEST_MAIN_LIBRARIES}"
        )
        set(GTEST_INCLUDE_DIRS ${DEPENDANCY_DIR}/gtest/include)
        set(GTEST_FOUND true)
    endif()
endif()
#########################################################
# Notify of missing dependancies
#########################################################
set(MISSING "")

if (NOT OPENGL_FOUND)
    list(APPEND MISSING OpenGL)
endif()

#if (NOT Qt5Widgets_FOUND)
#    list(APPEND MISSING Qt5Widgets)
#endif()

if (NOT GLM_FOUND)
    list(APPEND MISSING glm)
endif()

if (NOT OPENMESH_FOUND)
    list(APPEND MISSING OpenMesh)
endif()

if(USE_LOCAL_DEPENDANCIES)
    if (NOT GLFW_FOUND)
        list(APPEND MISSING glfw)
    endif()
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
