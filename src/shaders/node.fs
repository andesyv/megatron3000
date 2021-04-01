#version 450

in vec3 col;
in vec3 pos;
in vec2 fragCoords;

uniform vec2 nodeScale = vec2(0.02, 0.02);

out vec4 fragColor;

void main() {
    const vec3 lightDir = normalize(vec3(0.5, -0.5, 1.0));

    // "Simulate" a sphere projection
    const vec2 dir = (fragCoords - pos.xy) / nodeScale;
    float z = -sqrt(1.0 - dir.x * dir.x - dir.y * dir.y);
    const vec3 normal = normalize(vec3(dir, z));

    vec3 phong = vec3(1.0, 0., 0.) * max(dot(-lightDir, normal), 0.15);

    fragColor = vec4(phong, 1.0);
}