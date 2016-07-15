#-------------------------------------------------------------------------------
# Test Project Setup
#-------------------------------------------------------------------------------
set(TEST_COLLECTION_NAME defo_heart_test)
#-------------------------------------------------------------------------------
# Dependancies
#-------------------------------------------------------------------------------
list(APPEND TEST_LIBS
    "${PROJECT_LIBS}"
    "${GTEST_BOTH_LIBRARIES}"
)
list(APPEND TEST_INCLUDES
    "${GTEST_INCLUDE_DIRS}"
)
message(STATUS "PROJECT_LIBS:${PROJECT_LIBS}")
message(STATUS "GTEST_BOTH_LIBRARIES:${GTEST_BOTH_LIBRARIES}")
#-------------------------------------------------------------------------------
# Unit tests
#-------------------------------------------------------------------------------
list(APPEND TEST_SRCS
    render_core_test.cc
)

add_executable(${TEST_COLLECTION_NAME} ${TEST_SRCS})
target_include_directories(${TEST_COLLECTION_NAME} PRIVATE ${TEST_INCLUDES})
target_link_libraries(${TEST_COLLECTION_NAME} ${TEST_LIBS})
