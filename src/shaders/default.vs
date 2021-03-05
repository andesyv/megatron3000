#version 430

in vec3 inPos;
in vec3 inCol;

out vec3 col;

void main() {
    col = inCol;
    gl_Position = vec4(inPos, 1.0);
}