#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inCol;

uniform mat4 viewMatrix = mat4(1.0);
uniform vec2 aspectRatio = vec2(1.0, 1.0);

#define SCREEN_SCALE 0.1

out vec3 color;

void main() {
    color = inCol;
    mat3 view = mat3(viewMatrix);
    vec3 pos = view * inPos;
    pos.xy *= aspectRatio;
    
    // Scale into corner (from [-1, 1]):
    /*
    1. First scale down
    2. Move to top end of screen
    3. Offset with sceen scale multiplied by aspect ratio to always center in corner
    */
    pos.xy = pos.xy * SCREEN_SCALE + 1.0 - SCREEN_SCALE * aspectRatio;
    gl_Position = vec4(pos, 1.0);
}