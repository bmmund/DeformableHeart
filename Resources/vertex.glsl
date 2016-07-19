#version 330 core

layout (location = 0) in vec4 position;
layout (location = 0) in vec3 colour;
layout (location = 0) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Needed for shading calculations in fragement shader
out VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
    vec3 C;
} vs_out;

void main(void)
{
    gl_Position = projection * view * model * position;
}
