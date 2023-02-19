R"(
#version 430

layout(std430, binding = 0) buffer vertLayout
{
    vec2 vertices[];
};

layout(std430, binding = 1) buffer polygonLayout
{
    int polygonSizes[];
};

uniform vec4 u_colour;
uniform int u_polygonCount;
uniform vec2 u_resolution;

out vec4 colour;

const int outside = 0;
const int inside = 1;
const int onBoundary = 2;

float Dot(in vec2 a, in vec2 b)
{
  return (a.x * b.x) + (a.y * b.y);
}

vec2 ClosestPointToSegment(in vec2 point, in vec2 p0, in vec2 p1)
{
  vec2 v = p1 - p0;
  float u = 0.0;

  vec2 w = point - p0;
  float proj = Dot(w, v);
  if (proj < 0.0)
  {
    u = 0.0;
  }
  else
  {
    float vsq = Dot(v, v);
    if (proj >= vsq)
      u = 1.0;
    else
      u = proj / vsq;
  }

  return p0 + u * v;
}

vec2 ClosestPointToPolygon(in vec2 point)
{
  float minDist = 3.402823466e+38;
  vec2 closestPoint = vec2(0.0, 0.0);

  for (int p = 0; p < u_polygonCount; p++)
  {
    int size = polygonSizes[p];
    int currentVertex = 0;
    for (int i = 0; i < size; i++)
    {
      int j = currentVertex + i;
      int k = currentVertex + ((i + 1) % size);

      vec2 p0 = vertices[j];
      vec2 p1 = vertices[k];

      vec2 cp = ClosestPointToSegment(point, p0, p1);
      vec2 v = cp - point;
      float dist = Dot(v, v);
      if (dist < minDist)
      {
        minDist = dist;
        closestPoint = cp;
      }
    }
    currentVertex = currentVertex + size;
  }
  return closestPoint;
}

// Returns:
//   0: outside
//   1: inside
//   2: on boundary
// Adapted from "Optimal Reliable Point-in-Polygon Test and Differential Coding Boolean Operations on Polygons"
// Authors: Jianqiang Hao, Jianzhi Sun, Yi Chen, Qiang Cai and Li Tan 
int PointInsidePolygon(in vec2 point, in int start, in int size)
{
  int k = 0;
  float f = 0.0;
  float u1 = 0.0;
  float v1 = 0.0;
  float u2 = 0.0;
  float v2 = 0.0;
  
  int code = outside;
  
  for (int i = 0; i < size; i++)
  {
    int j = start + i;
    int k = start + ((i + 1) % size);

    vec2 p0 = vertices[j];
    vec2 p1 = vertices[k];

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

bool PointInsidePolygon(in vec2 point)
{
  int currentSize = polygonSizes[0];
  if (PointInsidePolygon(point, 0, currentSize) == outside)
  {
    return false;
  }

  if (PointInsidePolygon(point, 0, currentSize) == onBoundary)
  {
    return true;
  }

  int currentVertex = currentSize;
  for (int i = 1; i < u_polygonCount; i++)
  {
    currentSize = polygonSizes[i];

    if (PointInsidePolygon(point, currentVertex, currentSize) == inside)
    {
      return false;
    }

    currentVertex = currentVertex + currentSize;
  }
  return true;
}

void main(void)
{
  vec2 point = (gl_FragCoord.xy / u_resolution) * 2.0 - 1.0;

  if (!PointInsidePolygon(point))
  {
    vec2 cp = ClosestPointToPolygon(point);
    cp = (cp + 1.0) / 2.0 * u_resolution;
    vec2 v = cp - gl_FragCoord.xy;
    float dist = sqrt(Dot(v, v));
    float thickness = 1.2;

    if (dist <= thickness)
    {
      float alpha = 1.0 - dist / thickness;
      colour = vec4(u_colour.x, u_colour.y, u_colour.z, alpha * alpha);
    }
    else
    {
      discard;
    }
  }
  else
  {
    colour = u_colour;
  }
}
)"