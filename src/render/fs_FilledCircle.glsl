R"(
#version 430

in vec2 viewPosition;

uniform float u_size;
uniform vec2 u_resolution;
uniform vec4 u_colour;

out vec4 colour;

void main(void)
{
  vec2 st = gl_FragCoord.xy;
  vec2 vp = (viewPosition + vec2(1.0, 1.0)) / 2.0 * u_resolution;
  
  float fragDist = distance(vp, st);

  float aaRad = 1.3;
  float a = u_size / 2.0;
  float b = u_size / 2.0 - aaRad;

  if (fragDist > a)
  {
    discard;
  }
  else if (fragDist > b)
  {
    float alpha = (aaRad - (fragDist - b)) / aaRad;
    colour = vec4(u_colour.x, u_colour.y, u_colour.z, u_colour.w * alpha * alpha);
  }
  else
  {
    colour = u_colour;
  }
}
)"