R"(
#version 430

in vec2 viewPosition;

uniform float u_radius;
uniform vec2 u_resolution;
uniform vec4 u_colour;

out vec4 colour;

void main(void)
{
  vec2 st = gl_FragCoord.xy;
  vec2 vp = (viewPosition + vec2(1.0, 1.0)) / 2.0 * u_resolution;

  if (distance(vp, st) > u_radius / 2.0)
    discard;
  else
    colour = u_colour;
}
)"