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

  void Draw(std::vector<xn::seg> const &, xn::Colour, uint32_t flags) override;
  void SetMatrix_World_View(xn::mat33 const &) override;
  void SetResolution(xn::vec2 const &) override;

private:


  GLuint m_shaderProgram;
  xn::mat33 m_T_World_View;
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

void PolygonRenderer::SetMatrix_World_View(xn::mat33 const &mat)
{
  GLint prog;
  glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
  glUseProgram(m_shaderProgram);
  GLuint loc = glGetUniformLocation(m_shaderProgram, "u_T_World_View");
  if (loc == -1)
    throw MyException("Failed to set world to view matrix for the polygon renderer.");
  glUniformMatrix3fv(loc, 1, GL_FALSE, mat.GetData());
  glUseProgram(prog);

  m_T_World_View = mat;
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

void PolygonRenderer::Draw(std::vector<xn::seg> const &segments, xn::Colour clr, uint32_t flags)
{
  if (segments.empty())
    return;

  std::vector<xn::vec4> edges;
  xn::vec2 minPt(FLT_MAX, FLT_MAX);
  xn::vec2 maxPt(-FLT_MAX, -FLT_MAX);
  for (auto const &e : segments)
  {
    xn::vec2 p0 = e.GetP0();
    xn::vec2 p1 = e.GetP1();

    float maxX = std::max(p0.x(), p1.x());
    float maxY = std::max(p0.y(), p1.y());
    float minX = std::min(p0.x(), p1.x());
    float minY = std::min(p0.y(), p1.y());
    if (maxX > maxPt.x()) maxPt.x() = maxX;
    if (maxY > maxPt.y()) maxPt.y() = maxY;
    if (minX < minPt.x()) minPt.x() = minX;
    if (minY < minPt.y()) minPt.y() = minY;

    xn::vec3 p03(p0.x(), p0.y(), 1.f);
    xn::vec3 p13(p1.x(), p1.y(), 1.f);

    p03 = p03 * m_T_World_View;
    p13 = p13 * m_T_World_View;

    edges.push_back(xn::vec4(p03.x(), p03.y(), p13.x(), p13.y()));
  }

  // Set up polygon data

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint edgesBuffer;
  glGenBuffers(1, &edgesBuffer);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, edgesBuffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               edges.size() * sizeof(xn::vec4),
               edges.data(),
               GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, edgesBuffer);

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

  loc = glGetUniformLocation(m_shaderProgram, "u_edgeCount");
  if (loc == -1)
    throw MyException("Failed to set edge count for the polygon renderer.");
  glUniform1i(loc, (int)edges.size());

  glDrawArrays(GL_TRIANGLES, 0, 6);

  // Clean up...
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glDeleteBuffers(1, &quadBuffer);
  glDeleteBuffers(1, &edgesBuffer);
  glDeleteVertexArrays(1, &vao);
}