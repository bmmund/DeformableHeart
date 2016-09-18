#version 330 core

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 colour;
layout (location=3) in uint isOdd;

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

//uniform vec3 colour = vec3(0.4f, 0.0f, 0.0f);
uniform vec3 light_pos = vec3(100.0, 100.0, 100.0);

void main(void)
{
    // Calculate model-view matrix
    mat4 mv_matrix = view * model;

    // Calculate view-space coordinate
    vec4 P = mv_matrix * vec4(position, 1.0f);

    // Store the colour attribute
    if (isOdd == 1u)
    {
        vs_out.C = vec3(0.f, 0.f, 1.f);
    }
    else
    {
        vs_out.C = vec3(0.f, 1.f, 0.f);
    }

    // Calculate the clip-space position of each vertex
    gl_Position = projection * P;
}
