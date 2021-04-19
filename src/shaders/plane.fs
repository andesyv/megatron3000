#version 450

uniform float alpha;

out vec4 fragColor;

void main() {
    fragColor = vec4(1., 0., 0., alpha);
}