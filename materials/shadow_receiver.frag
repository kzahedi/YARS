#version 330 core

uniform sampler2D shadowMap;

in vec4 oShadowUV;
out vec4 FragColor;

// Modulation strength in fully-shadowed regions. 1.0 = no darkening,
// 0.0 = pure black. 0.3 makes shadows clearly visible — YARS walls are
// only 0.5m tall so their shadow strips are subtle even at full strength.
const float SHADOW_STRENGTH = 0.3;

void main()
{
    vec2 uv = oShadowUV.xy / oShadowUV.w;
    // Empirical YX swap: the receiver's texture_worldviewproj_matrix
    // computes UVs that are rotated 90° from where the caster pass writes
    // silhouettes in the shadow texture. Swapping X and Y aligns them.
    // Without this, the floor samples white interior of the shadow texture
    // everywhere and no shadow is visible.
    // See docs/planning/shadows_state.md for full investigation history.
    uv = uv.yx;
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || oShadowUV.w <= 0.0) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    }
    float visibility = texture(shadowMap, uv).r;
    float modulation = mix(SHADOW_STRENGTH, 1.0, visibility);
    FragColor = vec4(modulation, modulation, modulation, 1.0);
}
