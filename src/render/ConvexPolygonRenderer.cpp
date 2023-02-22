#include <vector>
#include <algorithm>

#include "glad/glad.h"
#include "glfw3.h"

#include "xnGeometry.h"
#include "xnColour.h"

#include "ConvexPolygonRenderer.h"
#include "MyException.h"

class ConvexPolygonRenderer : public IConvexPolygonRenderer
{
public:

  ConvexPolygonRenderer();
  ~ConvexPolygonRenderer();

  void Draw(std::vector<xn::vec2> const &vertices, xn::Colour, uint32_t flags) override;
  void SetViewMatrix(xn::mat33 const &) override;
  void SetResolution(xn::vec2 const &) override {};

private:

  GLuint m_shaderProgram;
  xn::mat33 m_mView;
};

IConvexPolygonRenderer *CreateConvexPolygonRenderer()
{
  return new ConvexPolygonRenderer();
}

ConvexPolygonRenderer::ConvexPolygonRenderer()
{
  std::string vs_str = std::string(
#include "vs_Triangles.glsl"
  );

  std::string fs_str = std::string(
#include "fs_Triangles.glsl"
  );

  m_shaderProgram = GetShaderProgram(vs_str, fs_str);
}

ConvexPolygonRenderer::~ConvexPolygonRenderer()
{
  glDeleteProgram(m_shaderProgram);
}

void ConvexPolygonRenderer::SetViewMatrix(xn::mat33 const &mat)
{
  GLint prog;
  glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
  glUseProgram(m_shaderProgram);
  GLuint loc = glGetUniformLocation(m_shaderProgram, "u_mView");
  if (loc == -1)
    throw MyException("Failed to set view matrix for the polygon renderer.");
  glUniformMatrix3fv(loc, 1, GL_FALSE, mat.GetData());
  glUseProgram(prog);

  m_mView = mat;
}

void ConvexPolygonRenderer::Draw(std::vector<xn::vec2> const &vertices, xn::Colour clr, uint32_t flags)
{
  if (vertices.size() < 3)
    return;

  std::vector<int> indices;
  int i = 1;
  for (size_t j = 2; j < vertices.size(); j++)
  {
    indices.push_back(0);
    indices.push_back(i);
    indices.push_back(j);
    i++;
  }

  // Set up polygon data

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER,
    vertices.size() * sizeof(xn::vec2),
    vertices.data(),
    GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  glBindBufferBase(GL_ARRAY_BUFFER, 0, vertexBuffer);

  GLuint indexBuffer;
  glGenBuffers(1, &indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    indices.size() * sizeof(int),
    indices.data(),
    GL_STATIC_DRAW);
  glBindBufferBase(GL_ELEMENT_ARRAY_BUFFER, 1, indexBuffer);

  // Draw elements...
  GLuint loc;

  glUseProgram(m_shaderProgram);
  loc = glGetUniformLocation(m_shaderProgram, "u_colour");
  if (loc == -1)
    throw MyException("Failed to set colour for the polygon renderer.");

  float colour[4] =
  {
    (float)clr.rgba.r / 255.f,
    (float)clr.rgba.g / 255.f,
    (float)clr.rgba.b / 255.f,
    (float)clr.rgba.a / 255.f
  };

  glUniform4fv(loc, 1, colour);

  glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, 0);

  // Clean up...
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glDeleteBuffers(1, &indexBuffer);
  glDeleteBuffers(1, &vertexBuffer);
  glDeleteVertexArrays(1, &vao);
}