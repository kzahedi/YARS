#version 330 core

uniform sampler2D shadowMap;

in vec4 oShadowUV;
out vec4 FragColor;

const float SHADOW_STRENGTH = 0.3;

void main()
{
    vec2 uv = oShadowUV.xy / oShadowUV.w;
    uv = uv.yx;  // YX swap (Ogre 14 quirk)
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || oShadowUV.w <= 0.0) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    }
    float visibility = texture(shadowMap, uv).r;
    float modulation = mix(SHADOW_STRENGTH, 1.0, visibility);
    FragColor = vec4(modulation, modulation, modulation, 1.0);
}
