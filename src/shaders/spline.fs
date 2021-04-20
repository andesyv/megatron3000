#version 450

in vec2 fragPos;

layout(binding = 1) uniform sampler1D transferFunction;

out vec4 fragColor;


void main() {
    const vec2 uv = fragPos * 0.5 + 0.5;

    vec4 tex = texture(transferFunction, uv.x);
    vec3 color = tex.rgb * tex.a;
    fragColor = vec4(color, 1.);
}