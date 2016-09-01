# Declare the library name
set(LIB_NAME mesh-processing)
set(MESH_PROCESSING_LIB ${LIB_NAME} PARENT_SCOPE)
#-------------------------------------------------------------------------------
# Dependancies
#-------------------------------------------------------------------------------
#########################################################
# Libs
#########################################################
list(APPEND LIBRARIES
    "${OPENMESH_LIBRARIES}"
    "${UTILITIES_LIB}"
    "${ACM_LIB}"
)

#########################################################
# Headers
#########################################################
list(APPEND INCLUDES_DIRS
    "${OPENMESH_INCLUDE_DIRS}"
    "${GLM_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

#-------------------------------------------------------------------------------
# Render-App Engine
#-------------------------------------------------------------------------------
list(APPEND SRCS
    loop.cc
    loop_acm.cc
    deformation.cc
)

list(APPEND HEADERS
    loop.hpp
    loop_acm.hpp
    deformation.hpp
)

add_library (${LIB_NAME} ${SRCS} ${HEADERS})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDES_DIRS})
target_link_libraries(${LIB_NAME} ${LIBRARIES})
set_property(TARGET ${LIB_NAME} PROPERTY CXX_STANDARD 11)
