#ifndef __SHADER__HPP
#define __SHADER__HPP

#include <GLFW/glfw3.h> // for gl headers

class Shader
{
public:
    // The program ID
    GLuint Program;
    // Constructor reads and builds the shader
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    // Use the program
    void Use();
};

#endif /* __SHADER__HPP */
