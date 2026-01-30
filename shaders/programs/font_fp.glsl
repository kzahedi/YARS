#version 330 core

// Input from vertex shader
in vec2 texCoord;
in vec4 vertexColour;

// Output
out vec4 fragColour;

// Font texture (BYTE_LA format: luminance in RGB, alpha in A)
uniform sampler2D fontTex;

void main()
{
    vec4 texColour = texture(fontTex, texCoord);
    // For BYTE_LA textures, the luminance is replicated to RGB
    // and alpha is in the alpha channel
    // Apply vertex color to the luminance and use texture alpha
    fragColour = vec4(vertexColour.rgb * texColour.rgb, texColour.a * vertexColour.a);
}
