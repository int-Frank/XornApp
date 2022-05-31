#ifndef RENDERER_H
#define RENDERER_H

#include "xnCommon.h"
#include "xnGeometry.h"
#include "xnLineProperties.h"
#include "xnRenderer.h"

class ImGuiRenderer : public xn::Renderer
{
public:

  ImGuiRenderer();
  ~ImGuiRenderer();

  void Set(xn::vec2 const &p0, xn::vec2 const &p1) override;
  void BeginDraw() override;
  void DrawLine(xn::seg const &, xn::LineProperties const &) override;
  void DrawFilledTriangle(xn::vec2 const &p0, xn::vec2 const &p1, xn::vec2 const &p2, xn::Colour) override;
  void EndDraw() override;

private:

  class PIMPL;
  PIMPL *m_pimpl;
};

#endif