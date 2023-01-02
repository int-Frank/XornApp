R"(
#version 330

uniform vec4 u_color;
out vec4 color;

void main(void)
{
  color = u_color;
}
)"