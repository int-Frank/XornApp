R"(
#version 430 core

layout (location = 0) in vec3 in_position;

layout(std430, binding = 1) buffer myLayout
{
    vec2 perpVects[];
};

uniform float u_thickness;
uniform mat3 u_T_World_Camera;
uniform mat3 u_T_Model_World;

void main(void)
{
  vec2 perpVec = perpVects[gl_VertexID / 6];
  vec3 pos = u_T_Model_World * u_T_World_Camera * vec3(in_position.x, in_position.y, 1.0); //u_T_Model_World * u_T_World_Camera * vec3(in_position.x, in_position.y, 1.0);
  vec3 extension = u_thickness / 200.0 * in_position.z * vec3(perpVec.x, perpVec.y, 0.0);
  vec3 finalPos = pos + extension;

  gl_Position = vec4(finalPos.x, finalPos.y, 0.0, 1.0);
}
)"