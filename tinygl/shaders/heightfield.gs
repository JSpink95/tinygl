#version 440 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec4 gWorldPosition[3];
in float gLinearHeight[3];

uniform mat4 u_Projection;
uniform mat4 u_View;

out vec3 fNormal;
out vec3 fPosition;
out float fLinearHeight;

void main()
{
	vec3 ba = normalize(gWorldPosition[1].xyz - gWorldPosition[0].xyz);
	vec3 ca = normalize(gWorldPosition[2].xyz - gWorldPosition[0].xyz);

	mat4 vp = u_Projection * u_View;
	fNormal = normalize(cross(ba, ca));

	fLinearHeight = gLinearHeight[0];
	fPosition = gWorldPosition[0].xyz;
	gl_Position = vp * gWorldPosition[0];
	EmitVertex();
	
	fLinearHeight = gLinearHeight[1];
	fPosition = gWorldPosition[1].xyz;
	gl_Position = vp * gWorldPosition[1];
	EmitVertex();
	
	fLinearHeight = gLinearHeight[2];
	fPosition = gWorldPosition[2].xyz;
	gl_Position = vp * gWorldPosition[2];
	EmitVertex();
}