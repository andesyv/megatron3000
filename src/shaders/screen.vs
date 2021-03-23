#version 430

in vec2 inPos;

// Screen spaced coordinates in homogenous space [-1, 1]
out vec2 fragCoord;

void main() {
    fragCoord = inPos;
    gl_Position = vec4(inPos, 0., 1.);
}