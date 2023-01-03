#include <vector>

#include "glad/glad.h"
#include "glfw3.h"

#include "xnGeometry.h"
#include "xnColour.h"

#include "LineRenderer.h"
#include "MyException.h"

class LineRenderer : public ILineRenderer
{
public:

  LineRenderer();
  ~LineRenderer();

  void Draw(std::vector<xn::seg> const &, xn::Colour, float thickness, uint32_t flags, xn::mat33 const &T_Model_World);
  void SetRenderSize(xn::vec2 const &)override;

private:

  void _SetMatrix_World_Camera(xn::mat33 const &) override;
  GLuint m_shaderProgram;
};

ILineRenderer *CreateLineRenderer()
{
  return new LineRenderer();
}

LineRenderer::LineRenderer()
{
  std::string vs_str = std::string(
#include "vs_Line.glsl"
  );

  std::string fs_str = std::string(
#include "fs_Line.glsl"
  );

  m_shaderProgram = GetShaderProgram(vs_str, fs_str);
}

LineRenderer::~LineRenderer()
{
  glDeleteProgram(m_shaderProgram);
}

void LineRenderer::_SetMatrix_World_Camera(xn::mat33 const &mat)
{
  glUseProgram(m_shaderProgram);
  GLuint loc = glGetUniformLocation(m_shaderProgram, "u_T_World_Camera");
  if (loc == -1)
    throw MyException("Failed to set world to camera matrix for the line renderer.");
  glUniformMatrix3fv(loc, 1, GL_FALSE, mat.GetData());
}

void LineRenderer::SetRenderSize(xn::vec2 const &sz)
{
  GLint prog;
  glGetIntegerv(GL_CURRENT_PROGRAM, &prog);

  glUseProgram(m_shaderProgram);
  GLuint loc = glGetUniformLocation(m_shaderProgram, "u_windowSize");
  if (loc == -1)
    throw MyException("Failed to set world to camera matrix for the line renderer.");
  glUniform2fv(loc, 1, sz.GetData());

  glUseProgram(prog);
}

void LineRenderer::Draw(std::vector<xn::seg> const &segments, xn::Colour clr, float thickness, uint32_t flags, xn::mat33 const &T_Model_World)
{
  std::vector<xn::vec3> points;
  std::vector<xn::vec2> perpVectors;

  // TODO Could probably be more efficient. Can we use instance rendering?
  //      Maybe gl_VertexID and index into a uniform/sso buffer?
  // Load data...
  for each (auto & seg in segments)
  {
    perpVectors.push_back(Dg::Perpendicular(Dg::Normalize(seg.Vect())));

    xn::vec2 e0 = seg.GetP0();
    xn::vec2 e1 = seg.GetP1();

    xn::vec3 p0(e0.x(), e0.y(), 1.f);
    xn::vec3 p1(e0.x(), e0.y(), -1.f);
    xn::vec3 p2(e1.x(), e1.y(), 1.f);
    xn::vec3 p3(e1.x(), e1.y(), -1.f);

    points.push_back(p0);
    points.push_back(p1);
    points.push_back(p3);

    points.push_back(p0);
    points.push_back(p3);
    points.push_back(p2);
  }

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  
  // Set up the position data

  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER,
               points.size() * sizeof(xn::vec3),
               points.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // Set up the perp vector data

  GLuint perpVecBuffer;
  glGenBuffers(1, &perpVecBuffer);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, perpVecBuffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               perpVectors.size() * sizeof(xn::vec2),
               perpVectors.data(), 
               GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, perpVecBuffer);

  // Draw elements...

  glUseProgram(m_shaderProgram);
  GLuint loc_color = glGetUniformLocation(m_shaderProgram, "u_color");
  if (loc_color == -1)
    throw MyException("Failed to set colour for the line renderer.");

  float colour[4] = 
  {
    (float)clr.rgba.r / 255.f,
    (float)clr.rgba.g / 255.f,
    (float)clr.rgba.b / 255.f,
    (float)clr.rgba.a / 255.f
  };

  glUniform4fv(loc_color, 1, colour);

  GLuint loc_matrix = glGetUniformLocation(m_shaderProgram, "u_T_Model_World");
  if (loc_matrix == -1)
    throw MyException("Failed to set model to world matrix for the line renderer.");
  glUniformMatrix3fv(loc_matrix, 1, GL_FALSE, T_Model_World.GetData());

  GLuint loc_thickness = glGetUniformLocation(m_shaderProgram, "u_thickness");
  if (loc_thickness == -1)
    throw MyException("Failed to set thickness for the line renderer.");
  glUniform1f(loc_thickness, thickness);

  glDrawArrays(GL_TRIANGLES, 0, (GLsizei)points.size());

  //if (m_loadList[i].flags & RF_RoundedEndPoints)
  //{
  //  // Instance rendering of endpoints.
  //  std::vector<xn::vec2> positions = GetUniqueEndPoints(m_loadList[i].segments);
  //}

  // Clean up...
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glDeleteBuffers(1, &vertexBuffer);
  glDeleteBuffers(1, &perpVecBuffer);
  glDeleteVertexArrays(1, &vao);
}