
#include <string>
#include <vector>

#include "glad/glad.h"
#include "glfw3.h"

#include "xnColour.h"
#include "xnGeometry.h"
#include "xnIScene.h"

#include "DefaultData.h"
#include "IRenderer.h"
#include "MyException.h"
#include "LineRenderer.h"
#include "CircleRenderer.h"
#include "PolygonRenderer.h"

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

class OpenGLRenderer : public IRenderer
{
public:

  OpenGLRenderer(uint32_t width, uint32_t height);
  ~OpenGLRenderer();

  void SetResolution(uint32_t width, uint32_t height) override;

  void BeginDraw() override;
  void DrawLine(xn::seg const &, float thickness, xn::Colour clr, uint32_t flags) override;
  void DrawLineGroup(std::vector<xn::seg> const &, float thickness, xn::Colour clr, uint32_t flags) override;
  void DrawFilledCircle(xn::vec2 const &centre, float radius, xn::Colour clr, uint32_t flags);
  void DrawFilledCircleGroup(std::vector<xn::vec2> const &centres, float radius, xn::Colour clr, uint32_t flags);
  //void DrawPolygon(xn::DgPolygon const &, float thickness, xn::Colour clr, uint32_t flags, xn::mat33 T_Model_World) override;
  void DrawFilledPolygon(std::vector<xn::seg> const &, xn::Colour clr, uint32_t flags) override;
  void EndDraw() override;

  void SetMatrix_World_View(xn::mat33 const &) override;
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
    PolygonRenderer,
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
  m_pRenderers[PolygonRenderer] = CreatePolygonRenderer();

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
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGBA, width, height,GL_TRUE);
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

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  m_width = width;
  m_height = height;
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
  glBlendFunc(GL_ONE, GL_ZERO);
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
  std::vector<xn::seg> segments{seg};
  RENDERER(LineRenderer)->Draw(segments, thickness, clr, flags);

  if (flags & xn::RF_RoundedEndPoints)
  {
    std::vector<xn::vec2> points = {seg.GetP0(), seg.GetP1()};
    RENDERER(CircleRenderer)->Draw(points, thickness, clr, flags);
  }
}

void OpenGLRenderer::DrawLineGroup(std::vector<xn::seg> const &segments, float thickness, xn::Colour clr, uint32_t flags)
{
  RENDERER(LineRenderer)->Draw(segments, thickness, clr, flags);

  if (flags & xn::RF_RoundedEndPoints)
  {
    std::vector<xn::vec2> points;
    for (auto const &s : segments)
    {
      if (!PointExists(points, s.GetP0()))
        points.push_back(s.GetP0());
      if (!PointExists(points, s.GetP1()))
        points.push_back(s.GetP1());
    }
    RENDERER(CircleRenderer)->Draw(points, thickness, clr, flags);
  }
}

void OpenGLRenderer::DrawFilledCircle(xn::vec2 const &centre, float radius, xn::Colour clr, uint32_t flags)
{
  std::vector<xn::vec2> positions{ centre };
  RENDERER(CircleRenderer)->Draw(positions, radius, clr, flags);
}

void OpenGLRenderer::DrawFilledCircleGroup(std::vector<xn::vec2> const &positions, float radius, xn::Colour clr, uint32_t flags)
{
  RENDERER(CircleRenderer)->Draw(positions, radius, clr, flags);
}

void OpenGLRenderer::DrawFilledPolygon(std::vector<xn::seg> const &edges, xn::Colour clr, uint32_t flags)
{
  RENDERER(PolygonRenderer)->Draw(edges, clr, flags);
}

void OpenGLRenderer::SetMatrix_World_View(xn::mat33 const &mat)
{
  for (int i = 0; i < Renderers_COUNT; i++)
    m_pRenderers[i]->SetMatrix_World_View(mat);
}

IRenderer *CreateRenderer()
{
  return new OpenGLRenderer(10, 10);
}