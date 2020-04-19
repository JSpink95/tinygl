#version 440 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord;
layout(location = 2) in mat4 a_transform; // per instance

uniform mat4 u_vp;
uniform float u_time;

out vec2 f_texcoord;

void main() {
	float windInfluence = a_position.y / 0.2f;
	float windAmount = 0.05f;
	vec3 position = a_position + vec3(sin(u_time) * windAmount, 0.0f, sin(u_time) * windAmount) * windInfluence;

	gl_Position = u_vp * a_transform * vec4(position, 1.0f);
	f_texcoord = a_texcoord;
}
