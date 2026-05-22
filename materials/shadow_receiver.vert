#version 330 core

uniform mat4 worldViewProjMatrix;
uniform mat4 worldMatrix;
uniform mat4 shadowViewProjMatrix;

in vec4 vertex;

out vec4 vShadowClip;
out vec3 vWorldPos;

void main()
{
    gl_Position = worldViewProjMatrix * vertex;
    vec4 worldPos = worldMatrix * vertex;
    vWorldPos = worldPos.xyz / worldPos.w;
    vShadowClip = shadowViewProjMatrix * worldPos;
}
