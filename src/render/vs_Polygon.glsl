R"(
#version 430 core

layout (location = 0) in vec2 in_position;

uniform mat3 u_T_World_View;

void main(void)
{
  vec3 viewPos = u_T_World_View * vec3(in_position.x, in_position.y, 1.0);
  gl_Position = vec4(viewPos.x, viewPos.y, 0.0, 1.0);
}
)"