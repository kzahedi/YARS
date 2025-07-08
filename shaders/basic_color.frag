#version 330 core

in vec3 fragNormal;
in vec2 fragTexCoord;

uniform vec4 diffuse;
uniform vec3 lightDir;
uniform vec3 ambient;

out vec4 fragColor;

void main()
{
    // Simple diffuse lighting
    vec3 normal = normalize(fragNormal);
    float NdotL = max(dot(normal, -lightDir), 0.0);

    vec3 finalColor = ambient + diffuse.rgb * NdotL;
    fragColor = vec4(finalColor, diffuse.a);
}