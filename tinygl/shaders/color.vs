#version 440 core

layout(location = 0) in vec3 position;

uniform mat4 u_World;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
	mat4 wvp = u_Projection * u_View * u_World;
	gl_Position = wvp * vec4(position, 1.0f);
}