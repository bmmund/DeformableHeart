#version 330 core

out vec4 colour;

// Input from vertex shader
in VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
    vec3 C;
} fs_in;

void main(void)
{
    // Write final color to the framebuffer
    colour = vec4(fs_in.C, 1.0);
}
