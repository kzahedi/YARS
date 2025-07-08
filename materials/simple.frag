#version 330 core

uniform vec4 diffuseColor;

out vec4 FragColor;

void main()
{
    FragColor = diffuseColor;
}