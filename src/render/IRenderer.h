#ifndef IRENDERER_H
#define IRENDERER_H

#include <stdint.h>

#include "xnColour.h"
#include "xnGeometry.h"

class IRenderer
{
public:

  virtual ~IRenderer() {}

  virtual void SetSize(uint32_t width, uint32_t height) = 0;

  virtual void BeginDraw() = 0;
  virtual void DrawLine(xn::seg const &, float thickness, xn::Colour clr, uint32_t flags) = 0;
  virtual void DrawLineGroup(std::vector<xn::seg> const &, float thickness, xn::Colour clr, uint32_t flags) = 0;
  virtual void DrawFilledCircle(xn::vec2 const &centre, float radius, xn::Colour clr, uint32_t flags) = 0;
  virtual void DrawFilledCircleGroup(std::vector<xn::vec2> const &centres, float radius, xn::Colour clr, uint32_t flags) = 0;
  //virtual void DrawPolygon(xn::DgPolygon const &, float thickness, xn::Colour clr, uint32_t flags, xn::mat33 T_Model_World) = 0;
  virtual void DrawFilledPolygon(std::vector<xn::seg> const &, xn::Colour clr, uint32_t flags) = 0;
  virtual void EndDraw() = 0;

  virtual void SetMatrix_World_View(xn::mat33 const &) = 0;
  virtual unsigned int GetTexture() const = 0;
  virtual unsigned int GetWidth() const = 0;
  virtual unsigned int GetHeight() const = 0;
};

IRenderer *CreateRenderer();

#endif