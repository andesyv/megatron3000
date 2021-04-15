#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inDir;

uniform mat4 MVP;

flat out vec3 dir;

void main() {
    dir = inDir;
    gl_Position = MVP * vec4(inPos, 1.0);
}