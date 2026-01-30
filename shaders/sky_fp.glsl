#version 330 core

in vec2 oUv0;
uniform sampler2D tex0;

out vec4 fragColor;

void main()
{
    fragColor = texture(tex0, oUv0);
}
