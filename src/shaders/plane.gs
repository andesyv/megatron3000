#version 450

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

flat in vec3 dir[];

uniform mat4 MVP;
uniform vec3 up = vec3(0., 1., 0.);

void main() {
    const float size = 2.0;
    const vec3 right = normalize(cross(dir[0], up));
    const vec3 up2 = normalize(cross(right, dir[0]));
    
    gl_Position = gl_in[0].gl_Position + MVP * vec4(size * (right + up2), 0.);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + MVP * vec4(size * (-right + up2), 0.);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + MVP * vec4(size * (right - up2), 0.);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + MVP * vec4(size * (-right - up2), 0.);
    EmitVertex();
    EndPrimitive();
}