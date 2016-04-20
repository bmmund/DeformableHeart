# Declare the library name
set(LIB_NAME utilities)
set(UTILITIES_LIB ${LIB_NAME} PARENT_SCOPE)
#-------------------------------------------------------------------------------
# Dependancies
#-------------------------------------------------------------------------------
#########################################################
# Headers
#########################################################
list(APPEND INCLUDES_DIRS
    "${GLM_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

#-------------------------------------------------------------------------------
# Render-App Engine
#-------------------------------------------------------------------------------
list(APPEND SRCS
    utilities.cc
    polyroots.cc
)

list(APPEND HEADERS
    utilities.hpp
    polyroots.hpp
)

add_library (${LIB_NAME} ${SRCS} ${HEADERS})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDES_DIRS})
target_link_libraries(${LIB_NAME} ${LIBRARIES})
set_property(TARGET ${LIB_NAME} PROPERTY CXX_STANDARD 11)
