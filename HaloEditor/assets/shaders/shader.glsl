#type vertex
#version 430

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Bitangent;
layout(location = 4) in vec2 a_TexCoord;

uniform mat4 u_viewProjection;

out vec3 v_Normal;
out vec2 v_TexCoord;
out vec3 v_Position;

void main()
{
	gl_Position =  u_viewProjection * vec4(a_Position, 1.0);
	v_Normal = a_Normal;
	v_TexCoord = a_TexCoord;
	v_Position = a_Position;
}

#type fragment
#version 430

layout(location = 0) out vec4 finalColor;
in vec3 v_Normal;	
in vec2 v_TexCoord;
in vec3 v_Position;
uniform vec3 u_lightPos;
uniform vec3 u_viewPos;
uniform sampler2D u_baseColor;
void main()
{
	vec3 lightColor = vec3(1.0,1.0,1.0);
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	// diffuse 
	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(u_lightPos - v_Position);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	// specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(u_viewPos - v_Position);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;  

	finalColor = vec4((ambient + diffuse + specular),1.0) * texture(u_baseColor,v_TexCoord);
}