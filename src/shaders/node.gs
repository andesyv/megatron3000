#version 450

#define TRIANGLECOUNT 24

layout(points) in;
layout(triangle_strip, max_vertices = TRIANGLECOUNT * 3) out;

uniform vec2 nodeScale = vec2(0.02, 0.02);
uniform int selected = 0;
uniform float glowRadius = 1.0;

out vec3 pos;
out vec2 fragCoords;

flat out int amISelected;

#define PI 3.1415926
#define TWOPI 6.283185


void main() {
    pos = gl_in[0].gl_Position.xyz;
    const float angle = TWOPI / float(TRIANGLECOUNT);
    float radius = 1.0;
    if (gl_PrimitiveIDIn == selected) {
        radius += glowRadius;
        amISelected = 1;
    } else {
        amISelected = 0;
    }


    for (int i = 0; i < TRIANGLECOUNT; i++) {
        const float a1 = angle * float(i);
        const float a2 = angle * float(i+1);

        gl_Position = gl_in[0].gl_Position;
        fragCoords = gl_Position.xy;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position + vec4(cos(a1) * nodeScale.x * radius, sin(a1) * nodeScale.y * radius, 0., 0.);
        fragCoords = gl_Position.xy;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position + vec4(cos(a2) * nodeScale.x * radius, sin(a2) * nodeScale.y * radius, 0., 0.);
        fragCoords = gl_Position.xy;
        EmitVertex();

        EndPrimitive();
    }
}
