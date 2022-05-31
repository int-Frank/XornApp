#ifndef RENDERER_H
#define RENDERER_H

#include "a2dCommon.h"
#include "a2dGeometry.h"
#include "a2dLineProperties.h"
#include "a2dRenderer.h"

class ImGuiRenderer : public a2d::Renderer
{
public:

  ImGuiRenderer();
  ~ImGuiRenderer();

  void Set(a2d::vec2 const &p0, a2d::vec2 const &p1) override;
  void BeginDraw() override;
  void DrawLine(a2d::seg const &, a2d::LineProperties const &) override;
  void DrawFilledTriangle(a2d::vec2 const &p0, a2d::vec2 const &p1, a2d::vec2 const &p2, a2d::Colour) override;
  void EndDraw() override;

private:

  class PIMPL;
  PIMPL *m_pimpl;
};

#endif