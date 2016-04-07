option(USE_OPENGL_LEGACY ON)
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
endif()
set(APP_ICON_LOCATION ${RESOURCE_DIR}/${APP_ICON_NAME})
#-------------------------------------------------------------------------------
# Dependancies
#-------------------------------------------------------------------------------
#########################################################
# Build Render-App Engine
#########################################################
add_subdirectory(render_core)
add_subdirectory(trackball)

#########################################################
# Libs
#########################################################
list(APPEND LIBRARIES
    "${GLFW_LIBRARIES}"
    "${OPENGL_LIBRARIES}"
    "${GLEW_LIBRARIES}"
    "${RENDER_CORE_LIB}"
    "${TRACKBALL_LIB}"
    #"${Qt5Widgets_LIBRARIES}"
)

#########################################################
# Headerss
#########################################################
list(APPEND INCLUDES_DIRS
    "${GLFW_INCLUDE_DIR}"
    "${OPENGL_INCLUDE_DIR}"
    "${GLEW_INCLUDE_DIRS}"
    "${GLM_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    #"${Qt5Widgets_INCLUDE_DIRS}"
)

#-------------------------------------------------------------------------------
# DEMO App
#-------------------------------------------------------------------------------
list(APPEND SRCS
    main.cc
    DefoHeart.cc
)

list(APPEND HEADERS
    DefoHeart.hpp
)

add_executable (${APP_NAME} ${SRCS} ${HEADERS} ${APP_ICON_LOCATION})
target_include_directories(${APP_NAME} PUBLIC ${INCLUDES_DIRS})
target_link_libraries(${APP_NAME} ${LIBRARIES})
set_property(TARGET ${APP_NAME} PROPERTY CXX_STANDARD 11)
# set(CMAKE_AUTOMOC ON)
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE true)
    set_target_properties(${APP_NAME} PROPERTIES
                        MACOSX_BUNDLE_INFO_PLIST
                        "${CMAKE_CURRENT_SOURCE_DIR}/OSX/MacOSXBundleInfo.plist.in")
    set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME})
    set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE ${APP_ICON_NAME})

    set_source_files_properties(${APP_ICON_LOCATION}
                                PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()
# set_target_properties(${APP_NAME} PROPERTIES AUTOMOC TRUE)
