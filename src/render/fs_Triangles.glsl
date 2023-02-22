R"(
#version 430

uniform vec4 u_colour;
out vec4 colour;

void main(void)
{
  colour = u_colour;
}
)"