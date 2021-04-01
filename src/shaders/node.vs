#version 450

layout(location = 0) in vec2 inPos;

out vec3 vCol;

void main() {
    vCol = vec3(inPos * 0.5 + 0.5, 0.);
    gl_Position = vec4(inPos, 0., 1.);
}