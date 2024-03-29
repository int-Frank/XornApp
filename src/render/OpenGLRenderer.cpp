
#include <string>
#include <vector>

#include "glad/glad.h"
#include "glfw3.h"

#include "xnColour.h"
#include "xnGeometry.h"
#include "Renderer.h"

#include "DefaultData.h"
#include "MyException.h"
#include "LineRenderer.h"
#include "CircleRenderer.h"
#include "FilledCircleRenderer.h"
#include "PolygonRenderer.h"
#include "ConvexPolygonRenderer.h"

#define RENDERER(x) ((I ## x*)m_pRenderers[x])

//----------------------------------------------------------------
// Helper functions
//----------------------------------------------------------------

static bool PointExists(std::vector<xn::vec2> const &points, xn::vec2 const &point)
{
  for (auto const &p : points)
  {
    if (p == point)
      return true;
  }
  return false;
}

//----------------------------------------------------------------
// OpenGLRenderer
//----------------------------------------------------------------

class OpenGLRenderer : public Renderer
{
public:

  OpenGLRenderer(uint32_t width, uint32_t height);
  ~OpenGLRenderer();

  void SetResolution(uint32_t width, uint32_t height) override;

  void BeginDraw() override;
  void DrawLine(xn::seg const &, float thickness, xn::Colour clr, uint32_t flags) override;
  void DrawLineGroup(xn::seg const *, size_t segCount, float thickness, xn::Colour clr, uint32_t flags) override;
  void DrawCircle(xn::vec2 const &centre, float size, float thickness, xn::Colour clr, uint32_t flags) override;
  void DrawCircleGroup(xn::vec2 const *, size_t circleCount, float size, float thickness, xn::Colour clr, uint32_t flags) override;
  void DrawFilledCircle(xn::vec2 const &centre, float size, xn::Colour clr, uint32_t flags) override;
  void DrawFilledCircleGroup(xn::vec2 const *, size_t circleCount, float size, xn::Colour clr, uint32_t flags) override;
  void DrawPolygon(xn::DgPolygon const &, float thickness, xn::Colour clr, uint32_t flags) override;
  void DrawPolygon(xn::vec2 const *, size_t vertCount, float thickness, xn::Colour clr, uint32_t flags) override;
  void DrawFilledPolygon(xn::DgPolygon const &, xn::Colour clr, uint32_t flags) override;
  void DrawFilledPolygon(xn::PolygonWithHoles const &, xn::Colour clr, uint32_t flags) override;
  void DrawFilledPolygon(xn::vec2 const *, size_t vertCount, int const *, size_t polyCount, xn::Colour clr, uint32_t flags) override;
  void DrawFilledConvexPolygon(xn::vec2 const *pVerts, size_t vertCount, xn::Colour clr, uint32_t flags) override;
  void EndDraw() override;

  void SetViewMatrix(xn::mat33 const &) override;
  unsigned int GetTexture() const override { return m_texture; };
  unsigned int GetWidth() const override { return m_width; }
  unsigned int GetHeight() const override { return m_height; }

private:

  void Init(uint32_t width, uint32_t height);
  void Destroy();

  enum Renderers
  {
    LineRenderer = 0,
    CircleRenderer,
    FilledCircleRenderer,
    PolygonRenderer,
    ConvexPolygonRenderer,
    Renderers_COUNT
  };

  ObjectRenderer *m_pRenderers[Renderers_COUNT];

  uint32_t m_width;
  uint32_t m_height;
  GLuint m_frameBuffer;
  GLuint m_texture;
  GLuint m_frameBufferMultisample;
  GLuint m_textureMultisample;
};

OpenGLRenderer::OpenGLRenderer(uint32_t width, uint32_t height)
  : m_pRenderers{}
  , m_width(-1)
  , m_height(-1)
  , m_frameBuffer(0)
  , m_texture(0)
  , m_frameBufferMultisample(0)
  , m_textureMultisample(0)
{
  m_pRenderers[LineRenderer] = CreateLineRenderer();
  m_pRenderers[CircleRenderer] = CreateCircleRenderer();
  m_pRenderers[FilledCircleRenderer] = CreateFilledCircleRenderer();
  m_pRenderers[PolygonRenderer] = CreatePolygonRenderer();
  m_pRenderers[ConvexPolygonRenderer] = CreateConvexPolygonRenderer();

  Init(width, height);
}

OpenGLRenderer::~OpenGLRenderer()
{
  Destroy();

  for (int i = 0; i < Renderers_COUNT; i++)
    delete m_pRenderers[i];
}

void OpenGLRenderer::SetResolution(uint32_t width, uint32_t height)
{
  if (width != m_width || height != m_height)
  {
    Destroy();
    Init(width, height);

    for (int i = 0; i < Renderers_COUNT; i++)
      m_pRenderers[i]->SetResolution(xn::vec2((float)width, (float)height));
  }
}

void OpenGLRenderer::Init(uint32_t width, uint32_t height)
{
  // Multisample frame buffer
  glGenFramebuffers(1, &m_frameBufferMultisample);
  glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferMultisample);

  // generate texture
  glGenTextures(1, &m_textureMultisample);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_textureMultisample);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGBA, width, height, GL_TRUE);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

  // attach it to currently bound framebuffer object
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_textureMultisample, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    throw MyException("Failed to initialise multisample frame buffer.");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Normal frame buffer
  glGenFramebuffers(1, &m_frameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

  // generate texture
  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  // attach it to currently bound framebuffer object
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    throw MyException("Failed to initialise frame buffer.");

  m_width = width;
  m_height = height;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLRenderer::Destroy()
{
  glDeleteFramebuffers(1, &m_frameBufferMultisample);
  glDeleteTextures(1, &m_textureMultisample);
  glDeleteFramebuffers(1, &m_frameBuffer);
  glDeleteTextures(1, &m_texture);
  m_frameBufferMultisample = 0;
  m_textureMultisample = 0;
  m_frameBuffer = 0;
  m_texture = 0;
}

void OpenGLRenderer::BeginDraw()
{
  // Clear the frame
  glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferMultisample);
  glClearColor(DefaultData::data.backgroundColour.r(), DefaultData::data.backgroundColour.g(), DefaultData::data.backgroundColour.b(), DefaultData::data.backgroundColour.a());
  glClear(GL_COLOR_BUFFER_BIT);

  // Prepare the draw state
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glViewport(0, 0, m_width, m_height);
}

void OpenGLRenderer::EndDraw()
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_frameBufferMultisample);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_frameBuffer);
  glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void OpenGLRenderer::DrawLine(xn::seg const &seg, float thickness, xn::Colour clr, uint32_t flags)
{
  RENDERER(LineRenderer)->Draw(&seg, 1, thickness, clr, flags);

  if (flags & xn::RF_RoundedEndPoints)
  {
    xn::vec2 points[2] = {seg.GetP0(), seg.GetP1()};
    RENDERER(FilledCircleRenderer)->Draw(points, 2, thickness, clr, flags);
  }
}

void OpenGLRenderer::DrawLineGroup(xn::seg const *pSegs, size_t segCount, float thickness, xn::Colour clr, uint32_t flags)
{
  RENDERER(LineRenderer)->Draw(pSegs, segCount, thickness, clr, flags);

  if (flags & xn::RF_RoundedEndPoints)
  {
    std::vector<xn::vec2> points;
    for (size_t i = 0; i < segCount; i++)
    {
      if (!PointExists(points, pSegs[i].GetP0()))
        points.push_back(pSegs[i].GetP0());
      if (!PointExists(points, pSegs[i].GetP1()))
        points.push_back(pSegs[i].GetP1());
    }
    RENDERER(FilledCircleRenderer)->Draw(points.data(), points.size(), thickness * 1.9f, clr, flags);
  }
}

void OpenGLRenderer::DrawCircle(xn::vec2 const &centre, float size, float thickness, xn::Colour clr, uint32_t flags)
{
  RENDERER(CircleRenderer)->Draw(&centre, 1, size, thickness, clr, flags);
}

void OpenGLRenderer::DrawCircleGroup(xn::vec2 const *pPositions, size_t circleCount, float size, float thickness, xn::Colour clr, uint32_t flags)
{
  RENDERER(CircleRenderer)->Draw(pPositions, circleCount, size, thickness, clr, flags);
}

void OpenGLRenderer::DrawFilledCircle(xn::vec2 const &centre, float size, xn::Colour clr, uint32_t flags)
{
  RENDERER(FilledCircleRenderer)->Draw(&centre, 1, size, clr, flags);
}

void OpenGLRenderer::DrawFilledCircleGroup(xn::vec2 const *pPositions, size_t circleCount, float size, xn::Colour clr, uint32_t flags)
{
  RENDERER(FilledCircleRenderer)->Draw(pPositions, circleCount, size, clr, flags);
}

void OpenGLRenderer::DrawPolygon(xn::DgPolygon const &polygon, float thickness, xn::Colour clr, uint32_t flags)
{
  std::vector<xn::seg> edges;
  for (auto edge_it = polygon.cEdgesBegin(); edge_it != polygon.cEdgesEnd(); edge_it++)
    edges.push_back(edge_it.ToSegment());
  DrawLineGroup(edges.data(), edges.size(), thickness, clr, flags);
}

void OpenGLRenderer::DrawPolygon(xn::vec2 const *pVerts, size_t vertCount, float thickness, xn::Colour clr, uint32_t flags)
{
  std::vector<xn::seg> edges;
  for (size_t i = 0; i < vertCount; i++)
  {
    xn::vec2 p0 = pVerts[i];
    xn::vec2 p1 = pVerts[(i + 1) % vertCount];
    edges.push_back(xn::seg(p0, p1));
  }
  DrawLineGroup(edges.data(), edges.size(), thickness, clr, flags);
}

void OpenGLRenderer::DrawFilledPolygon(xn::DgPolygon const &polygon, xn::Colour clr, uint32_t flags)
{
  int polySize = (int)polygon.Size();

  std::vector<xn::vec2> vertices;
  for (auto vert_it = polygon.cPointsBegin(); vert_it != polygon.cPointsEnd(); vert_it++)
    vertices.push_back(*vert_it);
  DrawFilledPolygon(vertices.data(), vertices.size(), &polySize, 1, clr, flags);
}

void OpenGLRenderer::DrawFilledPolygon(xn::PolygonWithHoles const &polygon, xn::Colour clr, uint32_t flags)
{
  std::vector<int> polygons;
  std::vector<xn::vec2> vertices;
  for (auto loop_it = polygon.loops.cbegin(); loop_it != polygon.loops.cend(); loop_it++)
  {
    polygons.push_back((int)loop_it->Size());
    for (auto vert_it = loop_it->cPointsBegin(); vert_it != loop_it->cPointsEnd(); vert_it++)
      vertices.push_back(*vert_it);
  }
  DrawFilledPolygon(vertices.data(), vertices.size(), polygons.data(), polygons.size(), clr, flags);
}

void OpenGLRenderer::DrawFilledPolygon(xn::vec2 const *pVerts, size_t vertCount, int const *pPolys, size_t polyCount, xn::Colour clr, uint32_t flags)
{
  RENDERER(PolygonRenderer)->Draw(pVerts, vertCount, pPolys, polyCount, clr, flags);
}

void OpenGLRenderer::DrawFilledConvexPolygon(xn::vec2 const *pVerts, size_t vertCount, xn::Colour clr, uint32_t flags)
{
  RENDERER(ConvexPolygonRenderer)->Draw(pVerts, vertCount, clr, flags);
}

void OpenGLRenderer::SetViewMatrix(xn::mat33 const &mat)
{
  for (int i = 0; i < Renderers_COUNT; i++)
    m_pRenderers[i]->SetViewMatrix(mat);
}

Renderer *CreateRenderer()
{
  return new OpenGLRenderer(10, 10);
}