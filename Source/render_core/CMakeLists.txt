# Declare the library name
set(LIB_NAME render-core)
set(RENDER_CORE_LIB ${LIB_NAME} PARENT_SCOPE)
#-------------------------------------------------------------------------------
# Dependancies
#-------------------------------------------------------------------------------
#########################################################
# Libs
#########################################################
list(APPEND LIBRARIES
    "${GLFW_LIBRARIES}"
    "${OPENGL_LIBRARIES}"
)

#########################################################
# Headers
#########################################################
list(APPEND INCLUDES_DIRS
    "${GLFW_INCLUDE_DIR}"
    "${OPENGL_INCLUDE_DIR}"
    "${GLM_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

#-------------------------------------------------------------------------------
# Render-App Engine
#-------------------------------------------------------------------------------
list(APPEND SRCS
    app.cc
)

list(APPEND HEADERS
    app.hpp
)

add_library (${LIB_NAME} ${SRCS} ${HEADERS})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDES_DIRS})
target_link_libraries(${LIB_NAME} ${LIBRARIES})
set_property(TARGET ${LIB_NAME} PROPERTY CXX_STANDARD 11)
