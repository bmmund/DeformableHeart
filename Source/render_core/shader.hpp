#ifndef __SHADER__HPP
#define __SHADER__HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h> // for gl headers
#include <string>

class Shader
{
public:
    // The program ID
    GLuint Program;
    // Constructor reads and builds the shader
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

    // Use the program
    void Use();

private:
	bool compileShaders(GLchar const * vertShaderSrc, GLchar const* fragShaderSrc);
};

#endif /* __SHADER__HPP */
