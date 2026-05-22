#version 330 core

uniform mat4 worldViewProjMatrix;
in vec4 vertex;
out float vDepth;

void main()
{
    gl_Position = worldViewProjMatrix * vertex;
    // Map clip-space z from [-1,1] to [0,1] for the colour-buffer output.
    vDepth = gl_Position.z / gl_Position.w * 0.5 + 0.5;
}
