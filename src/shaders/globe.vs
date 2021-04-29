#version 450

layout(location = 0) in vec2 inPos;

uniform mat4 viewMatrix = mat4(1.0);
uniform vec2 aspectRatio = vec2(1.0, 1.0);
uniform float radius = 0.1;

void main() {
    mat3 view = mat3(viewMatrix);
    vec3 pos = view * vec3(0., 0., 0.);
    pos.xy *= aspectRatio;
    
    // Scale into corner (from [-1, 1]):
    /*
    1. First scale down
    2. Move to top end of screen
    3. Offset with sceen scale multiplied by aspect ratio to always center in corner
    */
    const vec2 offset = inPos;
    pos.xy = 1.0 + offset * radius * aspectRatio;
    gl_Position = vec4(pos, 1.0);
}