#version 330 core

out vec4 colour;

// Needed for shading calculations in fragement shader
in VS_IN
{
    vec3 N;
    vec3 L;
    vec3 V;
    vec3 C;
} vs_in;

void main(void)
{
    colour = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
