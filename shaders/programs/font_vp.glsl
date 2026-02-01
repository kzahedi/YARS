#version 330 core

// Vertex attributes (OGRE 14 GL3Plus naming convention)
in vec4 vertex;
in vec2 uv0;
in vec4 colour;

// Outputs to fragment shader
out vec2 texCoord;
out vec4 vertexColour;

// Uniform matrices
uniform mat4 worldViewProj;

void main()
{
    gl_Position = worldViewProj * vertex;
    texCoord = uv0;
    vertexColour = colour;
}
