#version 430

in vec2 fragCoord;

uniform mat4 MVP;

out vec4 fragColor;

void main() {
    fragColor = vec4(abs(fragCoord), 0., 1.);
}