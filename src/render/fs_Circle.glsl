R"(
#version 430

in vec2 viewPosition;

uniform float u_radius;
uniform vec2 u_resolution;
uniform vec4 u_colour;
uniform float u_thickness;

out vec4 colour;

void main(void)
{
  vec2 st = gl_FragCoord.xy;
  vec2 vp = (viewPosition + vec2(1.0, 1.0)) / 2.0 * u_resolution;
  
  float fragDist = distance(vp, st);

  float aaRad = 1.0;
  float a = u_radius - (u_thickness / 2.0) - aaRad;
  float b = u_radius - (u_thickness / 2.0);
  float c = u_radius + (u_thickness / 2.0);
  float d = u_radius + (u_thickness / 2.0) + aaRad;

  if ()

  if (fragDist > radius + 1.0)
  {
    discard;
  }
  else
  {
    float alpha = -(fragDist - radius - 1.0) / 2.0;
    if (alpha > 1.0)
      alpha = 1.0;

    colour = vec4(u_colour.x, u_colour.y, u_colour.z, u_colour.w * alpha * alpha);
  }
}
)"