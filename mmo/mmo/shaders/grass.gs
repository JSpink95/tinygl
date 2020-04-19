#version 440 core

layout(points) in;
//layout(triangle_strip,max_vertices=4) out;
layout(points,max_vertices=1) out;

in vec3 g_position[];
in mat4 g_transform[];

uniform mat4 u_vp;

void main() {
	
	// output a single point for now
	gl_Position = u_vp * g_transform[0] * vec4(g_position[0], 1.0f);
	EmitVertex();

}
