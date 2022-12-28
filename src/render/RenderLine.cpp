#include <vector>

#include "../3rdParty/glad/include/glad/glad.h"
#include "../3rdParty/GLFW/include/glfw3.h"

#include "xnRenderer.h"
#include "xnDraw.h"
#include "xnGeometry.h"

#include "MyException.h"
#include "Renderable.h"

struct Line
{
  unsigned indices[2];
};

class LineMesh
{
public:

  LineMesh()
    : color(0xFFFFFFFF)
    , thickness(2.0f)
  {

  }

  std::vector<xn::vec2> verts;
  std::vector<Line>     lines;
  xn::mat33             T_Model_World;
  xn::Colour            color;
  float                 thickness;
};

class RenderLine : Renderable
{
public:

  RenderLine();
  ~RenderLine();

  void Clear();
  void Add(LineMesh const &);

private:

  void _SetMatrix_World_View(xn::mat33 const &) override;
  void _Draw() override;

  std::vector<LineMesh> m_loadList;
  GLuint                m_shaderProgram;
};

Renderable *GetRenderLine()
{
  return (Renderable*) new RenderLine();
}

RenderLine::RenderLine()
{
  m_shaderProgram = GetShaderProgram("./shaders/fs_Line.glsl", "./shaders/vs_Line.glsl");
}

RenderLine::~RenderLine()
{
  Clear();
  glDeleteProgram(m_shaderProgram);
}

void RenderLine::_SetMatrix_World_View(xn::mat33 const &mat)
{
  glUseProgram(m_shaderProgram);
  GLuint mwv = glGetUniformLocation(m_shaderProgram, "u_T_World_View");
  if (mwv == -1)
    throw MyException("Failed to set world to view matrix for the line renderer.");
  glUniformMatrix4fv(mwv, 1, GL_FALSE, mat.GetData());
}

void RenderLine::Clear()
{
  m_loadList.clear();
}

void RenderLine::_Draw()
{
  if (m_loadList.empty())
    return;

  std::vector<xn::vec2> points;
  std::vector<GLuint> indices;
  std::vector<GLuint> offsets;
  GLuint vertexOffset = 0;

  // Load data...

  for each (auto const &mesh in m_loadList)
  {
    offsets.push_back((GLuint)indices.size() * sizeof(GLuint));

    for each (auto const &index in mesh.lines)
    {
      indices.push_back(index.indices[0] + vertexOffset);
      indices.push_back(index.indices[1] + vertexOffset);
    }

    vertexOffset += GLuint(mesh.verts.size());
    points.insert(points.end(), mesh.verts.begin(), mesh.verts.end());
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
    points.size() * sizeof(xn::vec2),
    points.data(), GL_STATIC_DRAW);

  int stride = sizeof(xn::vec2);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    indices.size() * sizeof(GLuint),
    indices.data(), GL_STATIC_DRAW);

  GLuint loc_position = glGetAttribLocation(m_shaderProgram, "in_position");

  glVertexAttribPointer(loc_position, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(loc_position);

  // Draw elements...

  glUseProgram(m_shaderProgram);
  GLuint loc_color = glGetUniformLocation(m_shaderProgram, "u_color");
  for (size_t i = 0; i < m_loadList.size(); i++)
  {
    float clr[4] = 
    {
      (float)m_loadList[i].color.rgba.r,
      (float)m_loadList[i].color.rgba.g,
      (float)m_loadList[i].color.rgba.b,
      (float)m_loadList[i].color.rgba.a
    };

    glUniform4fv(loc_color, 1, clr);
    glLineWidth(m_loadList[i].thickness);

    GLuint mmw = glGetUniformLocation(m_shaderProgram, "u_T_Model_World");
    if (mmw == -1)
      throw MyException("Failed to set model to world matrix for the line renderer.");
    glUniformMatrix4fv(mmw, 1, GL_FALSE, m_loadList[i].T_Model_World.GetData());

    glDrawElements(GL_LINES,
      m_loadList[i].lines.size() * 2,
      GL_UNSIGNED_INT,
      (const void *)offsets[i]);
  }

  // Clean up...
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glDeleteBuffers(1, &vertexBuffer);
  glDeleteBuffers(1, &indexBuffer);
  glDeleteVertexArrays(1, &vao);
}

void RenderLine::Add(LineMesh const &mesh)
{
  m_loadList.push_back(mesh);
}
