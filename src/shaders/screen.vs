#version 430

in vec2 inPos;

out vec2 fragCoord;

void main() {
    fragCoord = inPos;
    gl_Position = vec4(inPos, 0., 1.);
}