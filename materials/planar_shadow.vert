#version 330 core

// Planar shadow proxy vertex shader. Geometry is rendered using the
// proxy's own world transform (already flattened onto the floor plane
// by PlanarShadowProjector::update before the frame).

uniform mat4 worldViewProjMatrix;

in vec4 vertex;

void main()
{
    gl_Position = worldViewProjMatrix * vertex;
}
