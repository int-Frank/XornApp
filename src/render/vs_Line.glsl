R"(
#version 430 core

layout (location = 0) in vec3 in_position;

layout(std430, binding = 1) buffer myLayout
{
    vec2 perpVects[];
};

uniform float u_thickness;
uniform mat3 u_T_World_View;
uniform mat3 u_T_Model_World;
uniform vec2 u_windowSize;

void main(void)
{
  float ar = u_windowSize.x / u_windowSize.y;
  vec2 perpVec = perpVects[gl_VertexID / 6];
  vec3 pos = u_T_Model_World * u_T_World_View * vec3(in_position.x, in_position.y, 1.0); //u_T_Model_World * u_T_World_Camera * vec3(in_position.x, in_position.y, 1.0);
  vec3 extensionVector = in_position.z * vec3(perpVec.x / ar, perpVec.y, 0.0);
  float extensionLength = u_thickness / u_windowSize.y;
  vec3 finalPos = pos + extensionLength * extensionVector;

  gl_Position = vec4(finalPos.x, finalPos.y, 0.0, 1.0);
}
)"