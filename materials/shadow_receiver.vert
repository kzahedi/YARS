#version 330 core

uniform mat4 worldViewProjMatrix;
uniform mat4 texWorldViewProjMatrix;

in vec4 vertex;

out vec4 oShadowUV;

void main()
{
    gl_Position = worldViewProjMatrix * vertex;
    oShadowUV = texWorldViewProjMatrix * vertex;
}
