#version 330 core

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 colour;

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

    // Calculate normal in view-space
    vs_out.N = mat3(mv_matrix) * normal;

    // Calculate light vector
    vs_out.L = light_pos - P.xyz;

    // Calculate view vector
    vs_out.V = -P.xyz;

    // Store the colour attribute
    vs_out.C = colour;

    // Calculate the clip-space position of each vertex
    gl_Position = projection * P;
}
