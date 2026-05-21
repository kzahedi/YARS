#version 330 core

uniform mat4 worldViewProjMatrix;
uniform mat4 worldMatrix;

in vec4 vertex;
in vec4 uv0;

out vec2 texUV;       // diffuse texture (ground.jpg) UV
out vec2 shadowUV;    // shadow RTT UV — world-XZ-derived

// Arena half-extent in Ogre world units. MUST match
// ShadowMapper(arenaSize=6.0f) constructor argument in
// src/yars/view/gui/OgreHandler.cpp (search for "arena half-extent").
// If you change this here, change it there too.
const float ARENA_HALF = 6.0;

void main() {
    gl_Position = worldViewProjMatrix * vertex;

    // Diffuse UV from the mesh's existing UV0.
    texUV = uv0.xy;

    // Shadow UV from the vertex's world-space XZ position. The shadow
    // camera is top-down ortho covering [-ARENA_HALF, +ARENA_HALF] in
    // both X and Z, so worldXZ in [-ARENA_HALF, +ARENA_HALF] -> uv in [0, 1].
    vec4 worldPos = worldMatrix * vertex;
    shadowUV = (worldPos.xz / (ARENA_HALF * 2.0)) + vec2(0.5, 0.5);
}
