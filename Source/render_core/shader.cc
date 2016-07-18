#include "shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

#include "utilities.hpp"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexShader;
    std::string fragmentShader;
    // read input file into string x2
    vertexShader = Utilities::readFileToString(vertexPath);
    fragmentShader = Utilities::readFileToString(fragmentPath);
}

void Shader::Use()
{

}
