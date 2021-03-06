#version 450

in vec3 pos;
in vec2 fragCoords;

flat in int amISelected;

uniform vec2 nodeScale = vec2(0.02, 0.02);
uniform float glowRadius = 1.0;

out vec4 fragColor;

void main() {
    const vec3 lightDir = normalize(vec3(0.5, -0.5, 1.0));

    // "Simulate" a sphere projection
    const vec2 dir = (fragCoords - pos.xy) / nodeScale;
    float dirLength = length(dir);

    // Glow blending:
    if (1.0 < dirLength) {
        float alpha = dirLength -= 1.0;
        alpha /= glowRadius;
        fragColor = vec4(0., 1.0, 1.0, pow(1.0 - alpha, 2.0));
    // Node rendering:
    } else {
        const float z = -sqrt(1.0 - dir.x * dir.x - dir.y * dir.y);
        const vec3 normal = normalize(vec3(dir, z));

        const vec3 color = amISelected == 1 ? vec3(0., 0., 1.) : vec3(1.0, 0., 0.);
        const vec3 phong = color * max(dot(-lightDir, normal), 0.15);
        
        fragColor = vec4(phong, 1.0);
    }
}