#version 330 core

in float vDepth;
out vec4 FragColor;

void main()
{
    // Encode normalised depth into the R channel of a single-channel
    // FLOAT32_R texture; the receiver samples this and compares with its
    // own shadow-space depth. Only R is stored.
    FragColor = vec4(vDepth, 0.0, 0.0, 1.0);
}
