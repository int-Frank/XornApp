R"(
#version 430

layout(std430, binding = 0) buffer myLayout
{
    vec4 edges[];
};

uniform vec4 u_colour;
uniform int u_edgeCount;
uniform vec2 u_resolution;

out vec4 colour;

const int outside = 0;
const int inside = 1;
const int onBoundary = 2;

// Returns:
//   0: outside
//   1: inside
//   2: on boundary
int PointInsidePolygon(in vec2 point)
{
  int k = 0;
  float f = 0.0;
  float u1 = 0.0;
  float v1 = 0.0;
  float u2 = 0.0;
  float v2 = 0.0;
  
  int code = outside;
  
  for (int i = 0; i < u_edgeCount; i++)
  {
    vec2 p0 = vec2(edges[i].x, edges[i].y);
    vec2 p1 = vec2(edges[i].z, edges[i].w);

    float xi = p0.x;
    float yi = p0.y;
    float xj = p1.x;
    float yj = p1.y;
  
    v1 = yi - point.y;
    v2 = yj - point.y;
  
    if (((v1 < 0.0) && (v2 < 0.0)) || ((v1 > 0.0) && (v2 > 0.0)))
      continue;
  
    u1 = xi - point.x;
    u2 = xj - point.x;
  
    if ((v2 > 0.0) && (v1 <= 0.0))
    {
      f = u1 * v2 - u2 * v1;
      if (f > 0)
      {
        k++;
      }
      else if (f == 0)
      {
        code = onBoundary;
        break;
      }
    }
    else if ((v1 > 0.0) && (v2 <= 0.0))
    {
      f = u1 * v2 - u2 * v1;
      if (f < 0)
      {
        k++;
      }
      else if (f == 0)
      {
        code = onBoundary;
        break;
      }
    }
    else if ((v2 == 0.0) && (v1 < 0.0))
    {
      f = u1 * v2 - u2 * v1;
      if (f == 0)
      {
        code = onBoundary;
        break;
      }
    }
    else if ((v1 == 0.0) && (v2 < 0.0))
    {
      f = u1 * v2 - u2 * v1;
      if (f == 0)
      {
        code = onBoundary;
        break;
      }
    }
    else if ((v1 == 0.0) && (v2 == 0.0))
    {
      if ((u2 <= 0.0) && (u1 >= 0.0))
      {
        code = onBoundary;
        break;
      }
      else if ((u1 <= 0.0) && (u2 >= 0.0))
      {
        code = onBoundary;
        break;
      }
    }
  }
  
  if (code != onBoundary)
    code = (k % 2 == 0) ? outside : inside;
  
  return code;
}

void main(void)
{
  vec2 point = (gl_FragCoord.xy / u_resolution) * 2.0 - 1.0;

  if (PointInsidePolygon(point) == outside)
    discard;
  else
    colour = u_colour;
}
)"