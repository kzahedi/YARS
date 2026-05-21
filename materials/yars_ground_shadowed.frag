#version 330 core

uniform sampler2D diffuseMap;     // ground.jpg
uniform sampler2D shadowMap;      // YarsShadowRTT (R8: 0=caster, 255=clear)

in vec2 texUV;
in vec2 shadowUV;

out vec4 FragColor;

const float SHADOW_STRENGTH = 0.4; // 0=full shadow, 1=no shadow

void main() {
    vec3 diffuse = texture(diffuseMap, texUV).rgb;

    // Out-of-arena: no shadow (border-colour sample). Guard.
    float occluder = 1.0;
    if (shadowUV.x >= 0.0 && shadowUV.x <= 1.0 &&
        shadowUV.y >= 0.0 && shadowUV.y <= 1.0) {
        occluder = texture(shadowMap, shadowUV).r;
    }

    // occluder == 1.0 -> no shadow; 0.0 -> full shadow.
    float modulation = mix(SHADOW_STRENGTH, 1.0, occluder);
    FragColor = vec4(diffuse * modulation, 1.0);
}
