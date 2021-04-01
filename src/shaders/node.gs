#version 450

#define TRIANGLECOUNT 24

layout(points) in;
layout(triangle_strip, max_vertices = TRIANGLECOUNT * 3) out;

in vec3 vCol[];

uniform vec2 nodeScale = vec2(0.02, 0.02);

out vec3 col;

#define PI 3.1415926
#define TWOPI 6.283185


void main() {
    col = vCol[0];
    const float angle = TWOPI / float(TRIANGLECOUNT);

    for (int i = 0; i < TRIANGLECOUNT; i++) {
        const float a1 = angle * float(i);
        const float a2 = angle * float(i+1);

        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position + vec4(cos(a1) * nodeScale.x, sin(a1) * nodeScale.y, 0., 0.);
        EmitVertex();
        gl_Position = gl_in[0].gl_Position + vec4(cos(a2) * nodeScale.x, sin(a2) * nodeScale.y, 0., 0.);
        EmitVertex();

        EndPrimitive();
    }

}
