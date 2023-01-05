#include <vector>

#include "glad/glad.h"
#include "glfw3.h"

#include "xnGeometry.h"
#include "xnColour.h"

#include "CircleRenderer.h"
#include "MyException.h"

class CircleRenderer : public ICircleRenderer
{
public:

  CircleRenderer();
  ~CircleRenderer();

  void Draw(std::vector<xn::vec2> const &, float radius, xn::Colour, uint32_t flags);
  void SetRenderSize(xn::vec2 const &)override;

private:

  void _SetMatrix_World_View(xn::mat33 const &) override;
  GLuint m_shaderProgram;
};

ICircleRenderer *CreateCircleRenderer()
{
  return new CircleRenderer();
}

CircleRenderer::CircleRenderer()
{
  std::string vs_str = std::string(
#include "vs_Circle.glsl"
  );

  std::string fs_str = std::string(
#include "fs_Circle.glsl"
  );

  m_shaderProgram = GetShaderProgram(vs_str, fs_str);

  glUseProgram(m_shaderProgram);
  GLuint loc = glGetUniformLocation(m_shaderProgram, "u_offsets");
  if (loc == -1)
    throw MyException("Failed to set offsets for the circle renderer.");

  float offsets[12] =
  {
    -1.f,  1.f,
     1.f,  1.f,
     1.f, -1.f,
    -1.f,  1.f,
     1.f, -1.f,
    -1.f, -1.f,
  };
  glUniform2fv(loc, 6, offsets);
}

CircleRenderer::~CircleRenderer()
{
  glDeleteProgram(m_shaderProgram);
}

void CircleRenderer::_SetMatrix_World_View(xn::mat33 const &mat)
{
  glUseProgram(m_shaderProgram);
  GLuint loc = glGetUniformLocation(m_shaderProgram, "u_T_World_View");
  if (loc == -1)
    throw MyException("Failed to set world to view matrix for the line renderer.");
  glUniformMatrix3fv(loc, 1, GL_FALSE, mat.GetData());
}

void CircleRenderer::SetRenderSize(xn::vec2 const &sz)
{
  GLint prog;
  glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
  
  glUseProgram(m_shaderProgram);
  GLuint loc = glGetUniformLocation(m_shaderProgram, "u_windowSize");
  if (loc == -1)
    throw MyException("Failed to set window size for the circle renderer.");
  glUniform2fv(loc, 1, sz.GetData());
  
  glUseProgram(prog);
}

void CircleRenderer::Draw(std::vector<xn::vec2> const &positions, float radius, xn::Colour clr, uint32_t flags)
{
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Set up the positions data

  GLuint positionsBuffer;
  glGenBuffers(1, &positionsBuffer);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionsBuffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
    positions.size() * sizeof(xn::vec2),
    positions.data(),
    GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, positionsBuffer);

  // Draw elements...
  GLuint loc;

  glUseProgram(m_shaderProgram);
  loc = glGetUniformLocation(m_shaderProgram, "u_color");
  if (loc == -1)
    throw MyException("Failed to set colour for the line renderer.");

  float colour[4] =
  {
    (float)clr.rgba.r / 255.f,
    (float)clr.rgba.g / 255.f,
    (float)clr.rgba.b / 255.f,
    (float)clr.rgba.a / 255.f
  };

  glUniform4fv(loc, 1, colour);

  loc = glGetUniformLocation(m_shaderProgram, "u_radius");
  if (loc == -1)
    throw MyException("Failed to set thickness for the line renderer.");
  glUniform1f(loc, radius);

  glDrawArraysInstanced(GL_TRIANGLES, 0, 6, (GLsizei)positions.size());

  // Clean up...
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glDeleteBuffers(1, &positionsBuffer);
  glDeleteVertexArrays(1, &vao);
}