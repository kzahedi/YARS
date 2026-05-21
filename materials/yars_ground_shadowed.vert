#version 330 core

uniform mat4 worldViewProjMatrix;
uniform mat4 worldMatrix;
// Pushed per-frame from C++ (ShadowMapper::update). Combines the
// shadow camera's view + projection + clip-to-texture bias, so a
// world-space vertex multiplied through it yields texture-space UV
// after a perspective divide (which for our ortho shadow camera is
// a no-op — w == 1). This sidesteps Ogre 14's broken
// `texture_worldviewproj_matrix` auto-param on GL3+ core.
uniform mat4 shadowViewProjMatrix;

in vec4 vertex;
in vec4 uv0;

out vec2 texUV;          // diffuse texture (ground.jpg) UV
out vec4 shadowClipPos;  // shadow-space clip-position; .xy/.w is the UV

void main() {
    gl_Position = worldViewProjMatrix * vertex;

    // Diffuse UV from the mesh's existing UV0.
    texUV = uv0.xy;

    // Project the world-space vertex through the shadow camera's
    // view+proj+bias product. The fragment shader does the perspective
    // divide.
    vec4 worldPos = worldMatrix * vertex;
    shadowClipPos = shadowViewProjMatrix * worldPos;
}
