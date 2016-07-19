#-------------------------------------------------------------------------------
# Macros/Functions
#-------------------------------------------------------------------------------
function(COPY_RESOURCE dir name)
    set(name ${${name}})
    set(dir ${${dir}})
    # Copy resource files
    configure_file(${dir}/${name} ${EXECUTABLE_OUTPUT_PATH}/${name} COPYONLY)

    # By default, Visual studio runs the executable from
    # the CMAKE_CURRENT_BINARY_DIR folder. We copy the resource files here as well
    # to make execution to work out of the box.
    if(MSVC_IDE)
        configure_file(${dir}/${name} ${CMAKE_CURRENT_BINARY_DIR}/${name} COPYONLY)
    endif(MSVC_IDE)
endfunction(COPY_RESOURCE)

#-------------------------------------------------------------------------------
# Project Setup
#-------------------------------------------------------------------------------
option(USE_OPENGL_LEGACY "Use fixed-pipeline opengl" ON)
if(USE_OPENGL_LEGACY)
    add_definitions(-DUSE_OPENGL_LEGACY)
endif()
# Declare the app name in the APP_NAME var
set(APP_NAME demo)
set(RESOURCE_DIR ${PROJECT_SOURCE_DIR}/Resources)

if(APPLE)
    set(APP_ICON_NAME "heart_icon.icns")
endif()
if(WIN32)
    set(APP_ICON_NAME "res.rc")
    add_definitions(-D_USE_MATH_DEFINES)
endif()

set(APP_ICON_LOCATION ${RESOURCE_DIR}/${APP_ICON_NAME})
set(HEART_MESH_NAME "Heart_Final_Flat.obj")
set(HEART_MESH_LOCATION ${RESOURCE_DIR}/${HEART_MESH_NAME})
set(HEART_MESH_MTL_NAME "Heart_Final_Flat.mtl")
set(HEART_MESH_MTL_LOCATION ${RESOURCE_DIR}/${HEART_MESH_MTL_NAME})
add_definitions(-DHEART_MESH_PATH=\"${HEART_MESH_NAME}\")

set(DODEDECAHEDRON_MESH_NAME "dodecahedron.obj")
set(DODEDECAHEDRON_MESH_MTL_NAME "dodecahedron.mtl")
add_definitions(-DDODEDECAHEDRON_MESH_PATH=\"${DODEDECAHEDRON_MESH_NAME}\")

list(APPEND RESOURCES
    "${RESOURCE_DIR}/${DODEDECAHEDRON_MESH_NAME}"
    "${RESOURCE_DIR}/${DODEDECAHEDRON_MESH_MTL_NAME}"
    "${APP_ICON_LOCATION}"
)

# Copy resource files
COPY_RESOURCE(RESOURCE_DIR HEART_MESH_NAME)
COPY_RESOURCE(RESOURCE_DIR HEART_MESH_MTL_NAME)
COPY_RESOURCE(RESOURCE_DIR DODEDECAHEDRON_MESH_NAME)
COPY_RESOURCE(RESOURCE_DIR DODEDECAHEDRON_MESH_MTL_NAME)

set(SHADER_LOCATION ${RESOURCE_DIR})
#-------------------------------------------------------------------------------
# Dependancies
#-------------------------------------------------------------------------------
#########################################################
# Build Render-App Engine
#########################################################
add_subdirectory(utilities)
add_subdirectory(interaction)
add_subdirectory(render_core)
add_subdirectory(mesh_processing)

set(PROJECT_LIBS "")
list(APPEND PROJECT_LIBS
    "${RENDER_CORE_LIB}"
    "${INTERACTION_LIB}"
    "${MESH_PROCESSING_LIB}"
    "${UTILITIES_LIB}"
)
#########################################################
# Libs
#########################################################
list(APPEND LIBRARIES
    "${GLFW_LIBRARIES}"
    "${OPENGL_LIBRARIES}"
    "${PROJECT_LIBS}"
    "${OPENMESH_LIBRARIES}"
    "${GLAD_LIB}"
)

#########################################################
# Headers
#########################################################
list(APPEND INCLUDES_DIRS
    "${GLFW_INCLUDE_DIR}"
    "${OPENGL_INCLUDE_DIR}"
    "${GLM_INCLUDE_DIRS}"
    "${OPENMESH_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/system"
    #"${Qt5Widgets_INCLUDE_DIRS}"
)

#-------------------------------------------------------------------------------
# DEMO App
#-------------------------------------------------------------------------------
list(APPEND SRCS
    main.cc
    DefoHeart.cc
    HeartMesh.cc
)

list(APPEND HEADERS
    DefoHeart.hpp
    HeartMesh.hpp
)

file(GLOB PROJECT_SHADERS ${SHADER_LOCATION}/*.vert
                          ${SHADER_LOCATION}/*.frag
                          ${SHADER_LOCATION}/*.glsl)
source_group("Shaders" FILES ${PROJECT_SHADERS})

# Copy shader files to directory
foreach(infileName ${PROJECT_SHADERS})
    get_filename_component(FILENAME ${infileName} NAME)
    get_filename_component(DIR ${infileName} DIRECTORY)
    COPY_RESOURCE(DIR FILENAME)
endforeach(infileName)

add_executable (${APP_NAME} ${SRCS} ${HEADERS} ${RESOURCES} ${PROJECT_SHADERS})
target_include_directories(${APP_NAME} PUBLIC ${INCLUDES_DIRS})
target_link_libraries(${APP_NAME} ${LIBRARIES})
set_property(TARGET ${APP_NAME} PROPERTY CXX_STANDARD 11)
# ensure that obj mesh files aren't considered linkable files
set_source_files_properties(${RESOURCES}
                            PROPERTIES
                            HEADER_FILE_ONLY ON)
# set(CMAKE_AUTOMOC ON)
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE true)
    set_target_properties(${APP_NAME} PROPERTIES
                        MACOSX_BUNDLE_INFO_PLIST
                        "${CMAKE_CURRENT_SOURCE_DIR}/OSX/MacOSXBundleInfo.plist.in")
    set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME})
    set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE ${APP_ICON_NAME})

    set_source_files_properties(${RESOURCES}
                                PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_source_files_properties(${PROJECT_SHADERS}
                                PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()
# set_target_properties(${APP_NAME} PROPERTIES AUTOMOC TRUE)
#-------------------------------------------------------------------------------
# Unit tests
#-------------------------------------------------------------------------------
if(BUILD_UNIT_TESTS)
    add_subdirectory(tests)
endif()
