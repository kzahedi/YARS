#version 330 core

// Planar shadow proxy fragment shader. Outputs translucent black so
// the floor texture beneath darkens but is not fully replaced.

out vec4 FragColor;

void main()
{
    // alpha=0.7 gives a clearly visible shadow on light-coloured
    // floors (YARS' default sand/dirt texture is bright enough that
    // 0.5 looked nearly identical to a dirt patch).
    FragColor = vec4(0.0, 0.0, 0.0, 0.7);
}
