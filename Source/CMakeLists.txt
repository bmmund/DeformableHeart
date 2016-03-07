# Add executable called "helloDemo" that is built from the source files
# "demo.cxx" and "demo_b.cxx". The extensions are automatically found.
list(APPEND PLATFORM_SOURCES
    "main.cc"
)
add_executable (demo WIN32 MACOSX_BUNDLE ${PLATFORM_SOURCES} )

#-------------------------------------------------------------------------------
# Dependancies
#-------------------------------------------------------------------------------
#########################################################
# Libs
#########################################################
list(APPEND PLATFORM_LIBRARIES
    "${GLFW_LIBRARIES}"
    "${OPENGL_LIBRARIES}"
    "${GLEW_LIBRARIES}"
)

#########################################################
# Headers
#########################################################
list(APPEND PLATFORM_INCLUDES
    "${GLFW_INCLUDE_DIRS}"
    "${OPENGL_INCLUDE_DIR}"
    "${GLEW_INCLUDE_DIRS}"
    "${GLM_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(demo PUBLIC ${PLATFORM_INCLUDES})
target_link_libraries(demo ${PLATFORM_LIBRARIES})

set_target_properties(demo PROPERTIES
MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/OSX/MacOSXBundleInfo.plist.in")
