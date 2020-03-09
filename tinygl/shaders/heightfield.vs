#version 440 core

layout(location = 0) in vec4 position;

uniform mat4 u_World;
uniform mat4 u_View;
uniform mat4 u_Projection;

uniform float u_MaxSnowHeight;

out vec4 gWorldPosition;
out float gLinearHeight;

void main()
{
	mat4 wvp = u_Projection * u_View * u_World;
	gl_Position = wvp * vec4(position.xyz, 1.0);
	gWorldPosition = u_World * vec4(position.xyz, 1.0);
	gLinearHeight = position.y / u_MaxSnowHeight;
}