R"(
#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_perpVect;

uniform float u_thickness;
uniform mat3 u_T_World_View;
uniform mat3 u_T_Model_World;

void main(void)
{
	vec3 pos = vec3(in_position.x, in_position.y, 1.0); //u_T_Model_World * u_T_World_View * vec3(in_position.x, in_position.y, 1.0);
	vec3 extension = u_thickness / 20.0 * in_position.z * vec3(in_perpVect.x, in_perpVect.y, 0.0);
	vec3 finalPos = pos + extension;
	
	gl_Position = vec4(finalPos.x, finalPos.y, 0.0, 1.0);
}
)"