#version 450

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

out vec2 fragPos;

void main() {
    gl_Position = gl_in[0].gl_Position;
    fragPos = gl_Position.xy;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    fragPos = gl_Position.xy;
    EmitVertex();

    gl_Position = vec4(gl_in[0].gl_Position.x, -1.0, gl_in[0].gl_Position.zw);
    fragPos = gl_Position.xy;
    EmitVertex();
    gl_Position = vec4(gl_in[1].gl_Position.x, -1.0, gl_in[1].gl_Position.zw);
    fragPos = gl_Position.xy;
    EmitVertex();

    EndPrimitive();
}