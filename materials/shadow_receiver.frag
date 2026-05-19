#version 330 core

uniform sampler2D shadowMap;

in vec4 oShadowUV;
out vec4 FragColor;

// Modulation strength in fully-shadowed regions. 1.0 = no darkening,
// 0.0 = pure black. 0.5 keeps the ground texture readable while
// still showing clear shadow shapes.
const float SHADOW_STRENGTH = 0.5;

void main()
{
    // Perspective divide, then sample the shadow texture.
    vec2 uv = oShadowUV.xy / oShadowUV.w;
    // Outside the light frustum: full brightness (no shadow modulation).
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || oShadowUV.w <= 0.0) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    }
    // Shadow texture: ~1.0 where light reaches (texture border + clear),
    // ~0.0 where caster wrote black. Map this into a modulator that lerps
    // between SHADOW_STRENGTH (full shadow) and 1.0 (full light).
    float visibility = texture(shadowMap, uv).r;
    float modulation = mix(SHADOW_STRENGTH, 1.0, visibility);
    FragColor = vec4(modulation, modulation, modulation, 1.0);
}
