#version 450

in vec2 fragPos;

layout(binding = 4) uniform sampler1D transferFunction;

out vec4 fragColor;


void main() {
    const vec2 uv = fragPos * 0.5 + 0.5;
    fragColor = vec4(vec3(texture(transferFunction, uv.x).a), 1.);
}