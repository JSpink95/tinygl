#version 440 core

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec3 gs_pos[];
in vec4 gs_col[];

uniform mat4 u_w;
uniform mat4 u_vp;

out vec3 fs_normal;
out vec4 fs_position;
out vec4 fs_albedo;

void main()
{
    fs_normal = -normalize(cross(normalize(gs_pos[2] - gs_pos[0]), normalize(gs_pos[1] - gs_pos[0])));
    
    mat4 wvp = u_vp * u_w;
    fs_position = u_w * vec4(gs_pos[0], 1.0f);
    fs_albedo = gs_col[0];
    gl_Position = wvp * vec4(gs_pos[0], 1.0f);
    EmitVertex();

    fs_position = u_w * vec4(gs_pos[1], 1.0f);
    fs_albedo = gs_col[1];
    gl_Position = wvp * vec4(gs_pos[1], 1.0f);
    EmitVertex();

    fs_position = u_w * vec4(gs_pos[2], 1.0f);
    fs_albedo = gs_col[2];
    gl_Position = wvp * vec4(gs_pos[2], 1.0f);
    EmitVertex();

    EndPrimitive();
}
