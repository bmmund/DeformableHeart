# Declare the library name
set(LIB_NAME glad)
set(GLAD_LIB ${LIB_NAME} PARENT_SCOPE)
#-------------------------------------------------------------------------------
# Dependancies
#-------------------------------------------------------------------------------
#########################################################
# Headers
#########################################################
list(APPEND INCLUDES_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

#-------------------------------------------------------------------------------
# Render-App Engine
#-------------------------------------------------------------------------------
list(APPEND SRCS
    src/glad.c
)

list(APPEND HEADERS
    include/glad/glad.h
    include/KHR/khrplatform.h

)

add_library (${LIB_NAME} ${SRCS} ${HEADERS})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDES_DIRS})
target_link_libraries(${LIB_NAME} ${LIBRARIES})
set_property(TARGET ${LIB_NAME} PROPERTY CXX_STANDARD 11)
