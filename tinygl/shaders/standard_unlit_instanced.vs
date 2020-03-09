#version 440 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;

layout(std140, binding = 0) uniform Matrices
{
    mat4 u_View, u_Projection;
};

// cheap and easy way for instanced rendering, not necessarily the best...
uniform mat4 u_World[32];

out vec2 fTexcoord;

void main()
{
    mat4 wvp = u_Projection * u_View * u_World[gl_InstanceID];
    gl_Position = wvp * vec4(position, 1.0);
    fTexcoord = texcoord;
}
