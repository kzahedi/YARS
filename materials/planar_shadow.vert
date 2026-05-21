#version 330 core

// Planar shadow proxy vertex shader.
//
// The proxy entity is attached to a static world-origin scene node;
// Ogre's `worldviewproj_matrix` auto-param therefore doesn't help us
// (it would map proxy-local vertices into world via identity). Instead
// we receive the CASTER's world matrix per-frame and apply the planar
// shadow projection per-vertex, then transform to clip space via
// view*proj.
//
// Why a custom shader and not the proxy node's transform?
// The planar shadow matrix is projective (w_row != (0,0,0,1)). Forcing
// it through Ogre::Affine3::decomposition + SceneNode TRS silently
// truncates the projective component and produces a degenerate result.
// Doing the projection here on the GPU sidesteps the issue: we keep
// the full 4x4 and do the perspective divide explicitly per vertex.
//
// See PlanarShadowProjector.cpp for the casterWorldMatrix update and
// for the constant planarShadowMatrix (derived from the floor plane
// and light direction at projector construction time).

uniform mat4 casterWorldMatrix;    // per-caster, set every frame from C++
uniform mat4 planarShadowMatrix;   // constant per projector instance
uniform mat4 viewProjMatrix;       // Ogre's viewproj_matrix auto-param

in vec4 vertex;

void main()
{
    // Caster vertex -> world space
    vec4 worldPos = casterWorldMatrix * vertex;
    // Project onto the floor plane along the light direction
    vec4 projWorldPos = planarShadowMatrix * worldPos;
    // The shadow matrix is projective (w != 1 in general). Do the
    // perspective divide explicitly.
    projWorldPos /= projWorldPos.w;
    // Lift the projection 5cm above the floor to defeat z-fighting
    // against the ground mesh (also at world Y=0). On macOS arm64
    // GL3+ the material's depth_bias alone wasn't enough, and even
    // 5mm/1cm lifts failed at oblique camera angles where the depth
    // resolution per pixel is coarser. 5cm is still visually
    // imperceptible (no daylight visible between shadow and floor
    // unless you're looking at a near-horizontal grazing angle) but
    // reliably wins the depth test from every YARS camera angle.
    projWorldPos.y += 0.05;
    // World space -> clip space
    gl_Position = viewProjMatrix * projWorldPos;
}
