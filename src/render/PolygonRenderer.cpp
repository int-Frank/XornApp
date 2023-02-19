#include <vector>
#include <algorithm>

#include "glad/glad.h"
#include "glfw3.h"

#include "xnGeometry.h"
#include "xnColour.h"

#include "PolygonRenderer.h"
#include "MyException.h"

class PolygonRenderer : public IPolygonRenderer
{
public:

  PolygonRenderer();
  ~PolygonRenderer();

  void Draw(std::vector<xn::vec2> const &vertices, std::vector<int> const &polygonSizes, xn::Colour, uint32_t flags) override;
  void SetViewMatrix(xn::mat33 const &) override;
  void SetResolution(xn::vec2 const &) override;

private:


  GLuint m_shaderProgram;
  xn::mat33 m_mView;
};

IPolygonRenderer *CreatePolygonRenderer()
{
  return new PolygonRenderer();
}

PolygonRenderer::PolygonRenderer()
{
  std::string vs_str = std::string(
#include "vs_Polygon.glsl"
  );

  std::string fs_str = std::string(
#include "fs_Polygon.glsl"
  );

  m_shaderProgram = GetShaderProgram(vs_str, fs_str);
}

PolygonRenderer::~PolygonRenderer()
{
  glDeleteProgram(m_shaderProgram);
}

void PolygonRenderer::SetViewMatrix(xn::mat33 const &mat)
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

void PolygonRenderer::SetResolution(xn::vec2 const &sz)
{
  GLint prog;
  glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
  glUseProgram(m_shaderProgram);
  GLuint loc = glGetUniformLocation(m_shaderProgram, "u_resolution");
  if (loc == -1)
    throw MyException("Failed to set resolution for the polygon renderer.");
  glUniform2fv(loc, 1, sz.GetData());
  glUseProgram(prog);
}

void PolygonRenderer::Draw(std::vector<xn::vec2> const &_vertices, std::vector<int> const &polygonSizes, xn::Colour clr, uint32_t flags)
{
  if (_vertices.empty())
    return;

  std::vector<xn::vec2> vertices;
  xn::vec2 minPt(FLT_MAX, FLT_MAX);
  xn::vec2 maxPt(-FLT_MAX, -FLT_MAX);
  for (size_t i = 0; i < _vertices.size(); i++)
  {
    xn::vec2 p0 = _vertices[i];

    if (p0.x() > maxPt.x()) maxPt.x() = p0.x();
    if (p0.x() < minPt.x()) minPt.x() = p0.x();
    if (p0.y() > maxPt.y()) maxPt.y() = p0.y();
    if (p0.y() < minPt.y()) minPt.y() = p0.y();

    xn::vec3 p03(p0.x(), p0.y(), 1.f);
    p03 = p03 * m_mView;
    vertices.push_back(xn::vec2(p03.x(), p03.y()));
  }

  // Set up polygon data

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
    vertices.size() * sizeof(xn::vec2),
    vertices.data(),
    GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertexBuffer);

  GLuint polygonBuffer;
  glGenBuffers(1, &polygonBuffer);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, polygonBuffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
    polygonSizes.size() * sizeof(int),
    polygonSizes.data(),
    GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, polygonBuffer);

  // Set up the positions data

  GLuint quadBuffer;
  float quad[12] =
  {
    minPt.x(),  minPt.y(),
    maxPt.x(),  minPt.y(),
    minPt.x(),  maxPt.y(),

    maxPt.x(),  minPt.y(),
    maxPt.x(),  maxPt.y(),
    minPt.x(),  maxPt.y()
  };

  glGenBuffers(1, &quadBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(quad),
               quad,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

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

  loc = glGetUniformLocation(m_shaderProgram, "u_polygonCount");
  if (loc == -1)
    throw MyException("Failed to set vertex count for the polygon renderer.");
  glUniform1i(loc, (int)polygonSizes.size());

  glDrawArrays(GL_TRIANGLES, 0, 6);

  // Clean up...
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glDeleteBuffers(1, &quadBuffer);
  glDeleteBuffers(1, &vertexBuffer);
  glDeleteBuffers(1, &polygonBuffer);
  glDeleteVertexArrays(1, &vao);
}