#version 330 core

// Planar shadow proxy fragment shader. Outputs translucent black so
// the floor texture beneath darkens but is not fully replaced.

out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0, 0.0, 0.0, 0.5);
}
