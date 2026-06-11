#version 330 core

// Shadow-mapping receiver fragment shader. Runs as Ogre's
// SHADOWTYPE_TEXTURE_MODULATIVE modulating pass — re-renders every
// receiver after the main scene pass. Our output multiplies the
// framebuffer's existing colour (scene_blend modulate). White =
// unchanged; gray = darkened proportionally.

uniform sampler2D shadowMap;
uniform float     shadowBias;     // small offset to combat shadow acne
uniform float     shadowDarkness; // modulation factor for shadowed pixels

in vec4 vShadowClip;
in vec3 vWorldPos;
out vec4 FragColor;

void main()
{
    // Perspective divide → shadow-camera NDC [-1, 1].
    vec3 ndc = vShadowClip.xyz / vShadowClip.w;

    // NDC → texture UV [0, 1].
    vec2 uv = ndc.xy * 0.5 + 0.5;

    // FBO Y-flip compensation: the shadow texture's V axis is inverted
    // relative to the UV we derive from shadow-camera NDC, so the
    // receiver must sample at (u, 1-v). Verified empirically 2026-06-11
    // under the custom projection matrix from YarsFixedShadowCameraSetup
    // (A/B frame captures of braitenberg + falling_objects): WITH this
    // flip, the robot's shadow hugs its base and the airborne ball's
    // shadow lies on the floor below/offset from the ball, consistent
    // with light (-1,-1,-1); WITHOUT it, those shadows vanish and a
    // phantom blob appears mirrored across the arena. The convention is
    // unchanged from the old FocusedShadowCameraSetup path — Ogre's
    // caster pass renders into a GL FBO with Y=0 at the bottom (the
    // per-render-target flip applies even with setCustomProjectionMatrix
    // on the shadow camera).
    uv.y = 1.0 - uv.y;

    float currentDepth = ndc.z * 0.5 + 0.5;

    // Outside the shadow camera's frustum → not shadowed.
    if (uv.x < 0.0 || uv.x > 1.0 ||
        uv.y < 0.0 || uv.y > 1.0 ||
        currentDepth < 0.0 || currentDepth > 1.0) {
        FragColor = vec4(1.0);
        return;
    }

    float occluderDepth = texture(shadowMap, uv).r;
    bool inShadow = (currentDepth - shadowBias) > occluderDepth;

    float k = inShadow ? shadowDarkness : 1.0;
    FragColor = vec4(k, k, k, 1.0);
}
