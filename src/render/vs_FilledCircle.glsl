R"(
#version 430 core

layout(std430, binding = 1) buffer myLayout
{
    vec2 positions[];
};

uniform float u_size;
uniform vec2 u_resolution;
uniform mat3 u_mView;
uniform vec2 u_offsets[6];

out vec2 viewPosition;

void main(void)
{
  vec2 offset = u_size * u_offsets[gl_VertexID];
  offset = offset / u_resolution * 2.0;
  vec2 worldPos = positions[gl_InstanceID];
  vec3 viewPosition3 = u_mView * vec3(worldPos.x, worldPos.y, 1.0);
  viewPosition = vec2(viewPosition3.x, viewPosition3.y);
  vec2 outPos = viewPosition + offset;

  gl_Position = vec4(outPos.x, outPos.y, 0.0, 1.0);
}
)"