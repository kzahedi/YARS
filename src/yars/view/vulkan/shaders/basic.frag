#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

layout(push_constant) uniform PushConstants {
    vec4 color;
    vec4 lightDir;
    int useTexture;
} pc;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(pc.lightDir.xyz);

    // Simple diffuse lighting
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 ambient = vec3(0.3);
    vec3 diffuse = diff * vec3(0.7);

    vec4 baseColor;
    if (pc.useTexture == 1) {
        baseColor = texture(texSampler, fragTexCoord);
    } else {
        baseColor = pc.color;
    }

    vec3 result = (ambient + diffuse) * baseColor.rgb;
    outColor = vec4(result, baseColor.a);
}
