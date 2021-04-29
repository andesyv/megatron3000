#version 450

layout(location = 0) in vec3 inPos;

uniform mat4 viewMatrix = mat4(1.0);
uniform vec2 aspectRatio = vec2(1.0, 1.0);
uniform float radius = 0.1;

void main() {
    mat3 view = mat3(viewMatrix);
    vec3 pos = view * inPos;
    pos.xy *= aspectRatio;
    
    // Scale into corner (from [-1, 1]):
    /*
    1. First scale down
    2. Move to top end of screen
    3. Offset with sceen scale multiplied by aspect ratio to always center in corner
    */
    vec2 offset = vec2(-1.0, -3.0);
    pos.xy = pos.xy * radius + 1.0 + offset * radius * aspectRatio;
    gl_Position = vec4(pos, 1.0);
}