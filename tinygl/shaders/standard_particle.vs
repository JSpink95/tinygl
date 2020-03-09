#version 440 core

layout(location = 0) in vec3 position;
layout(location = 1) in float rotation;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
	gl_Position = u_Projection * u_View * vec4(position, 1.0);
}