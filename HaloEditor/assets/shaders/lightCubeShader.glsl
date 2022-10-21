#type vertex
#version 430

layout(location = 0) in vec3 a_Position;

uniform mat4 u_viewProjection;
uniform mat4 u_model;
out vec3 v_pos;
void main()
{
	gl_Position =  u_viewProjection * u_model * vec4(a_Position, 1.0);
	v_pos = a_Position;
}

#type fragment
#version 430

layout(location = 0) out vec4 finalColor;

void main()
{
	finalColor = vec4(1.0);
}
