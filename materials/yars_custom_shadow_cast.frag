#version 330 core
out vec4 FragColor;
void main() {
    // Pure black for any caster pixel. The shadow RTT clears to white
    // before this pass renders, so casters paint themselves dark on a
    // light background.
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
