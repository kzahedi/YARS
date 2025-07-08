#version 330 core

uniform mat4 worldViewProjMatrix;

in vec4 vertex;

void main()
{
    gl_Position = worldViewProjMatrix * vertex;
}