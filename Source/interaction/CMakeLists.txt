# Declare the library name
set(LIB_NAME interaction)
set(INTERACTION_LIB ${LIB_NAME} PARENT_SCOPE)
#-------------------------------------------------------------------------------
# Dependancies
#-------------------------------------------------------------------------------
#########################################################
# Libs
#########################################################
list(APPEND LIBRARIES
    "${UTILITIES_LIB}"
)

#########################################################
# Headers
#########################################################
list(APPEND INCLUDES_DIRS
    "${GLM_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

#-------------------------------------------------------------------------------
# Trackball Lib
#-------------------------------------------------------------------------------
list(APPEND SRCS
    trackball.cc
)

list(APPEND HEADERS
    trackball.hpp
)

add_library (${LIB_NAME} ${SRCS} ${HEADERS})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDES_DIRS})
target_link_libraries(${LIB_NAME} ${LIBRARIES})
set_property(TARGET ${LIB_NAME} PROPERTY CXX_STANDARD 11)
