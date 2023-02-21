R"(
#version 430

in vec2 viewPosition;

uniform float u_size;
uniform float u_thickness;
uniform vec2 u_resolution;
uniform vec4 u_colour;

out vec4 colour;

void main(void)
{
  vec2 st = gl_FragCoord.xy;
  vec2 vp = (viewPosition + vec2(1.0, 1.0)) / 2.0 * u_resolution;
  
  float fragDist = distance(vp, st);

  float aaRad = 1.3;
  float radius = u_size / 2.0;

  if (u_thickness > aaRad * 2.0)
  {
    float thickness = u_thickness - 2.0 * aaRad;
    float a = radius - (thickness / 2.0) - aaRad;
    float b = radius - (thickness / 2.0);
    float c = radius + (thickness / 2.0);
    float d = radius + (thickness / 2.0) + aaRad;
    
    if (fragDist < a)
    {
      discard;
    }
    else if (fragDist < b)
    {
      float alpha = (aaRad - (b - fragDist)) / aaRad;
      colour = vec4(u_colour.x, u_colour.y, u_colour.z, u_colour.w * alpha * alpha);
    }
    else if (fragDist < c)
    {
      colour = u_colour;
    }
    else if (fragDist < d)
    {
      float alpha = (d - fragDist) / aaRad;
      colour = vec4(u_colour.x, u_colour.y, u_colour.z, u_colour.w * alpha * alpha);
    }
    else
    {
      discard;
    }
  }
  else
  {
    float peak = (u_thickness / 2.0) / aaRad;
    float a = radius - (u_thickness / 2.0);
    float b = radius;
    float c = radius + (u_thickness / 2.0);

    if (fragDist < a)
    {
      discard;
    }
    else if (fragDist < b)
    {
      float alpha = (aaRad - (b - fragDist)) / aaRad;
      colour = vec4(u_colour.x, u_colour.y, u_colour.z, u_colour.w * alpha * peak);
    }
    else if (fragDist < c)
    {
      float alpha = (c - fragDist) / aaRad;
      colour = vec4(u_colour.x, u_colour.y, u_colour.z, u_colour.w * alpha * peak);
    }
    else
    {
      discard;
    }
  }
}
)"