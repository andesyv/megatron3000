#version 430

in vec2 fragCoord;

uniform mat4 MVP = mat4(1.0);
layout(binding = 0) uniform sampler3D volume;
uniform vec3 volumeScale = vec3(1., 1., 1.);
uniform vec3 volumeSpacing = vec3(1., 1., 1.);
uniform float time;

out vec4 fragColor;

void main() {
    const vec2 uv = fragCoord * 0.5 + 0.5;
    vec4 slice = MVP * vec4(fragCoord, .0, 1.);
    slice /= slice.w;

    const float sTime = fract(time * 0.1);

    // Convert [-1, 1] -> [0, 1]
    vec3 texCoords = slice.xyz * 0.5 + 0.5;
    // Apply volume spacing: [0, 1] -> [-0.5, 0.5] -> volumeSpacing * [-0.5, 0.5] -> 0.5 + volumeSpacing * [-0.5, 0.5] 
    texCoords = volumeSpacing * (texCoords - 0.5) / volumeScale + 0.5;
    const float grayScale = texture(volume, texCoords).r;

    fragColor = vec4(vec3(grayScale), 1.);
}