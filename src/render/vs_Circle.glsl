R"(
#version 430 core

layout(std430, binding = 1) buffer myLayout
{
    vec2 positions[];
};

uniform float u_radius;
uniform vec2 u_windowSize;
uniform mat3 u_T_World_View;
uniform vec2 u_offsets[6];

out vec2 viewPosition;
out float radius;

void main(void)
{
  float ar = u_windowSize.y / u_windowSize.x;
  vec2 offset = u_radius * u_offsets[gl_VertexID] / u_windowSize.y;
  offset = vec2(offset.x * ar, offset.y);
  vec2 worldPos = positions[gl_InstanceID];
  vec3 viewPosition3 = u_T_World_View * vec3(worldPos.x, worldPos.y, 1.0);
  viewPosition = vec2(viewPosition3.x, viewPosition3.y);
  vec2 outPos = viewPosition + offset;

  gl_Position = vec4(outPos.x, outPos.y, 0.0, 1.0);
}
)"