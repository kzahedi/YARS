#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 worldViewProjMatrix;
uniform mat4 worldMatrix;
uniform mat3 normalMatrix;

out vec3 fragNormal;
out vec2 fragTexCoord;

void main()
{
    gl_Position = worldViewProjMatrix * vec4(position, 1.0);
    fragNormal = normalMatrix * normal;
    fragTexCoord = texCoord;
}