#include "gtest/gtest.h"
#include "utilities.hpp"
#include <string>

TEST(ShaderReadFileToString, ReadsTestGLSL)
{
    std::string expected = "this is a test\n";
    std::string actual;
    const char* input = "test.glsl";
    actual = Utilities::readFileToString(input);
    EXPECT_STREQ(expected.c_str(), actual.c_str());
}