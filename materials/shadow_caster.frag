#version 330 core

in float vDepth;
out vec4 FragColor;

void main()
{
    // Encode normalised depth into the R channel; the receiver
    // samples this and compares with its own shadow-space depth.
    FragColor = vec4(vDepth, vDepth, vDepth, 1.0);
}
