
#include <string>
#include <vector>

#include "glad/glad.h"
#include "glfw3.h"

#include "xnColour.h"
#include "xnGeometry.h"

#include "IRenderer.h"
#include "MyException.h"
#include "LineRenderer.h"

class OpenGLRenderer : public IRenderer
{
public:

  OpenGLRenderer(uint32_t width, uint32_t height);
  ~OpenGLRenderer();

  void SetSize(uint32_t width, uint32_t height);

  void BeginDraw() override;
  void DrawLine(xn::seg const &, float thickness, xn::Colour clr, uint32_t flags, xn::mat33 T_Model_World) override;
  void DrawLineGroup(std::vector<xn::seg> const &, float thickness, xn::Colour clr, uint32_t flags, xn::mat33 T_Model_World) override;
  //void DrawFilledNGon(xn::vec2 const &centre, uint32_t sides, float radius, xn::Colour clr, uint32_t flags, xn::mat33 T_Model_World) override;
  //void DrawFilledNGonGroup(std::vector<xn::vec2> const &centres, uint32_t sides, float radius, uint32_t flags, xn::Colour clr, xn::mat33 T_Model_World) override;
  //void DrawPolygon(xn::DgPolygon const &, float thickness, xn::Colour clr, uint32_t flags, xn::mat33 T_Model_World) override;
  //void DrawFilledPolygon(xn::DgPolygon const &, xn::Colour clr, uint32_t flags, xn::mat33 T_Model_World) override;
  void EndDraw() override;

  void SetMatrix_World_Camera(xn::mat33 const &) override;
  unsigned int GetTexture() const override { return m_texture; };
  unsigned int GetWidth() const override { return m_width; }
  unsigned int GetHeight() const override { return m_height; }

private:

  void Init(uint32_t width, uint32_t height);
  void Destroy();

  ILineRenderer *m_pLineRenderer;

  uint32_t m_width;
  uint32_t m_height;
  GLuint m_frameBuffer;
  GLuint m_texture;
};

OpenGLRenderer::OpenGLRenderer(uint32_t width, uint32_t height)
  : m_pLineRenderer(CreateLineRenderer())
  , m_width(-1)
  , m_height(-1)
  , m_frameBuffer(0)
  , m_texture(0)
{
  Init(width, height);
}

OpenGLRenderer::~OpenGLRenderer()
{
  Destroy();
  delete m_pLineRenderer;
}

void OpenGLRenderer::SetSize(uint32_t width, uint32_t height)
{
  if (width != m_width || height != m_height)
  {
    Destroy();
    Init(width, height);

    m_pLineRenderer->SetRenderSize(xn::vec2((float)width, (float)height));
  }
}

void OpenGLRenderer::Init(uint32_t width, uint32_t height)
{
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
  glDeleteFramebuffers(1, &m_frameBuffer);
  glDeleteTextures(1, &m_texture);
  m_frameBuffer = 0;
  m_texture = 0;
}

void OpenGLRenderer::BeginDraw()
{
  // Clear the frame
  glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Prepare the draw state
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ZERO);
  glViewport(0, 0, m_width, m_height);
}

void OpenGLRenderer::EndDraw()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLRenderer::DrawLine(xn::seg const &seg, float thickness, xn::Colour clr, uint32_t flags, xn::mat33 T_Model_World)
{
  std::vector<xn::seg> segments{seg};
  m_pLineRenderer->Draw(segments, clr, thickness, flags, T_Model_World);
}

void OpenGLRenderer::DrawLineGroup(std::vector<xn::seg> const &segments, float thickness, xn::Colour clr, uint32_t flags, xn::mat33 T_Model_World)
{
  m_pLineRenderer->Draw(segments, clr, thickness, flags, T_Model_World);
}

void OpenGLRenderer::SetMatrix_World_Camera(xn::mat33 const &mat)
{
  m_pLineRenderer->SetMatrix_World_Camera(mat);
}

IRenderer *CreateRenderer()
{
  return new OpenGLRenderer(10, 10);
}