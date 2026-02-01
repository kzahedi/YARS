#version 330 core

in vec4 vertex;
in vec2 uv0;
in vec3 normal;

uniform mat4 worldViewProj;
uniform mat4 world;
uniform vec4 texMatrixRow0;
uniform vec4 texMatrixRow1;

out vec2 oUv0;
out vec3 oNormal;
out vec3 oWorldPos;

void main()
{
    gl_Position = worldViewProj * vertex;

    // Apply texture matrix for scaling/rotating/scrolling
    vec3 uv3 = vec3(uv0, 1.0);
    oUv0.x = dot(texMatrixRow0.xyz, uv3);
    oUv0.y = dot(texMatrixRow1.xyz, uv3);

    oNormal = mat3(world) * normal;
    oWorldPos = (world * vertex).xyz;
}
