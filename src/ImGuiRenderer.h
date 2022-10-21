#ifndef RENDERER_H
#define RENDERER_H

#include "xnCommon.h"
#include "xnGeometry.h"
#include "xnDraw.h"
#include "xnRenderer.h"

class ImGuiRenderer : public xn::Renderer
{
public:

  ImGuiRenderer();
  ~ImGuiRenderer();

  void Set(xn::vec2 const &p0, xn::vec2 const &p1) override;
  void BeginDraw() override;
  void DrawLine(xn::seg const &, xn::Draw::Stroke) override;
  void DrawNGon(Dg::Vector2<float> const &centre, float radius, uint32_t sides, xn::Draw::Stroke) override;
  void DrawFilledNGon(Dg::Vector2<float> const &centre, uint32_t sides, float radius, xn::Draw::Fill) override;
  void DrawFilledTriangle(xn::vec2 const &p0, xn::vec2 const &p1, xn::vec2 const &p2, xn::Draw::Fill) override;
  void EndDraw() override;

private:

  class PIMPL;
  PIMPL *m_pimpl;
};

#endif