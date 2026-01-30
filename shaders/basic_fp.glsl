#version 330 core

in vec2 oUv0;
in vec3 oNormal;
in vec3 oWorldPos;

uniform sampler2D tex0;
uniform vec4 ambient;
uniform vec4 lightPos;
uniform vec4 lightDiffuse;

out vec4 fragColor;

void main()
{
    vec4 texColor = texture(tex0, oUv0);

    // Simple diffuse lighting
    vec3 lightDir = normalize(lightPos.xyz - oWorldPos);
    float diff = max(dot(normalize(oNormal), lightDir), 0.0);

    vec3 finalColor = texColor.rgb * (ambient.rgb + lightDiffuse.rgb * diff);
    fragColor = vec4(finalColor, texColor.a);
}
