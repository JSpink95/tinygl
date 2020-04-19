#version 440 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord;

uniform mat4 u_vp;
uniform mat4 u_w;

out vec2 f_texcoord;

void main() {
	f_texcoord = a_texcoord;
	gl_Position = u_vp * u_w * vec4(a_position, 1.0f);
}
