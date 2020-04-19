#version 440 core

layout(location = 0) in vec3 a_position;

uniform mat4 u_vp;
uniform mat4 u_w;

void main()
{
    gl_Position = u_vp * u_w * vec4(a_position, 1.0f);
}
