#include <vector>

#include "glad/glad.h"
#include "glfw3.h"

#include "xnGeometry.h"
#include "xnColour.h"

#include "LineRenderer.h"
#include "MyException.h"
#include "Renderable.h"

class LineRenderer : public ILineRenderer, public ObjectRenderer
{
public:

  LineRenderer();
  ~LineRenderer();

  void Draw(std::vector<xn::seg> const &, xn::Colour, float thickness, uint32_t flags, xn::mat33 const &T_Model_World);

private:

  void _SetMatrix_World_View(xn::mat33 const &) override;
  
  GLuint m_shaderProgram;
};

ILineRenderer *CreateLineRenderer()
{
  return new LineRenderer();
}

LineRenderer::LineRenderer()
{
  m_shaderProgram = GetShaderProgram("./shaders/fs_Line.glsl", "./shaders/vs_Line.glsl");
}

LineRenderer::~LineRenderer()
{
  glDeleteProgram(m_shaderProgram);
}

void LineRenderer::_SetMatrix_World_View(xn::mat33 const &mat)
{
  glUseProgram(m_shaderProgram);
  GLuint mwv = glGetUniformLocation(m_shaderProgram, "u_T_World_View");
  if (mwv == -1)
    throw MyException("Failed to set world to view matrix for the line renderer.");
  glUniformMatrix4fv(mwv, 1, GL_FALSE, mat.GetData());
}

void LineRenderer::Draw(std::vector<xn::seg> const &segments, xn::Colour clr, float thickness, uint32_t flags, xn::mat33 const &T_Model_World)
{
  std::vector<xn::vec3> points;

  // Load data...
  for each (auto & seg in segments)
  {
    xn::vec2 e0 = seg.GetP0();
    xn::vec2 e1 = seg.GetP1();

    xn::vec3 p0(e0.x(), e0.y(), 1.f);
    xn::vec3 p1(e0.x(), e0.y(), -1.f);
    xn::vec3 p2(e1.x(), e1.y(), 1.f);
    xn::vec3 p3(e1.x(), e1.y(), -1.f);

    points.push_back(p0);
    points.push_back(p1);
    points.push_back(p2);

    points.push_back(p0);
    points.push_back(p3);
    points.push_back(p2);
  }

  GLuint vao;
  GLuint vertexBuffer;
  GLuint indexBuffer;

  glGenBuffers(1, &vertexBuffer);
  glGenBuffers(1, &indexBuffer);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER,
               points.size() * sizeof(xn::vec3),
               points.data(), 
               GL_STATIC_DRAW);

  int stride = sizeof(xn::vec3);

  GLuint loc_position = glGetAttribLocation(m_shaderProgram, "in_position");

  glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(loc_position);

  // Draw elements...

  glUseProgram(m_shaderProgram);
  GLuint loc_color = glGetUniformLocation(m_shaderProgram, "u_color");
  if (loc_color == -1)
    throw MyException("Failed to set colour for the line renderer.");

  float colour[4] = 
  {
    (float)clr.rgba.r,
    (float)clr.rgba.g,
    (float)clr.rgba.b,
    (float)clr.rgba.a
  };

  glUniform4fv(loc_color, 1, colour);

  GLuint loc_matrix = glGetUniformLocation(m_shaderProgram, "u_T_Model_World");
  if (loc_matrix == -1)
    throw MyException("Failed to set model to world matrix for the line renderer.");
  glUniformMatrix4fv(loc_matrix, 1, GL_FALSE, T_Model_World.GetData());

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
  glDeleteBuffers(1, &indexBuffer);
  glDeleteVertexArrays(1, &vao);
}