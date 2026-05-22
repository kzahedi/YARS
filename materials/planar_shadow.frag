#version 330 core

// Planar shadow proxy fragment shader. Outputs translucent black so
// the floor texture beneath darkens but is not fully replaced.

out vec4 FragColor;

void main()
{
    // Soft gray overlay (rather than pure black alpha-blended which
    // produces a muddy dark patch). The mid-gray colour mixes with
    // the floor texture under alpha-blending to darken-but-not-black
    // the floor pixels — closer to how a real ambient-occluded shadow
    // looks on a textured surface.
    FragColor = vec4(0.25, 0.25, 0.25, 0.55);
}
