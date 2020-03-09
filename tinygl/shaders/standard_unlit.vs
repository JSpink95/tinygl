#version 440 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;

layout(std140, binding = 0) uniform Matrices
{
    mat4 u_View, u_Projection;
};

uniform mat4 u_World;

out vec2 fTexcoord;

void main()
{
    mat4 wvp = u_Projection * u_View * u_World;
    gl_Position = wvp * vec4(position, 1.0);
    fTexcoord = texcoord;
}