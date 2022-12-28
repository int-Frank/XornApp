R"(
#version 330 core

in vec4 in_position;

uniform mat4 u_mvp;

void main(void)
{
	gl_Position = u_mvp * in_position;
}
)"