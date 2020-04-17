#version 440 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in float textureid;

uniform mat4 u_World;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec2 f_Texcoord;
out float f_TextureId;

void main()
{
	mat4 wvp = u_Projection * u_View * u_World;
	gl_Position = wvp * vec4(position, 1.0);

	f_Texcoord = texcoord;
	f_TextureId = textureid;
}
