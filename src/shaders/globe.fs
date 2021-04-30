#version 450

in vec3 pos;
in vec2 fragCoords;

uniform vec2 aspectRatio = vec2(1.0, 1.0);
uniform float radius = 0.1;
uniform vec3 lightDir = vec3(0., 0., -1.);

out vec4 fragColor;

void main() {
    const vec2 nodeScale = vec2(radius, radius) * aspectRatio;

    // "Simulate" a sphere projection
    const vec2 dir = (fragCoords - pos.xy) / nodeScale;
    float dirLength = length(dir);

    const float z = -sqrt(1.0 - dir.x * dir.x - dir.y * dir.y);
    const vec3 normal = normalize(vec3(dir, z));

    const vec3 color = vec3(1.0, 0.5, 0.);
    const vec3 phong = color * max(dot(-lightDir, normal), 0.15);
    
    fragColor = vec4(phong, 0.6);
}