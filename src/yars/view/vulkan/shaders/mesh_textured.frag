#version 450

// Input from vertex shader
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec2 fragTexCoord;

// Texture sampler
layout(binding = 1) uniform sampler2D texSampler;

// Output
layout(location = 0) out vec4 outColor;

void main() {
    // Sample texture
    vec4 texColor = texture(texSampler, fragTexCoord);

    // DEBUG: Output texture at full brightness (ignore lighting)
    // Simple lighting: use normal for basic shading
    //vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    //float diffuse = max(dot(normalize(fragNormal), lightDir), 0.3);

    // Combine texture color with lighting
    //vec3 finalColor = texColor.rgb * diffuse;
    //outColor = vec4(finalColor, texColor.a);

    // DEBUG: Output texture color directly (no lighting)
    outColor = vec4(texColor.rgb, 1.0);
}
