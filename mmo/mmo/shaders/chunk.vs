#version 440 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;

out vec3 gs_pos;
out vec4 gs_col;

void main()
{
    gs_pos = a_position;
    gs_col = a_color;
}
