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
  virtual void DrawLine(xn::seg const &, float thickness, xn::Colour clr, uint32_t flags, xn::mat33 T_Model_World) = 0;
  virtual void DrawLineGroup(std::vector<xn::seg> const &, float thickness, xn::Colour clr, uint32_t flags, xn::mat33 T_Model_World) = 0;
  //virtual void DrawFilledNGon(xn::vec2 const &centre, uint32_t sides, float radius, xn::Colour clr, uint32_t flags, xn::mat33 T_Model_World) = 0;
  //virtual void DrawFilledNGonGroup(std::vector<xn::vec2> const &centres, uint32_t sides, float radius, uint32_t flags, xn::Colour clr, xn::mat33 T_Model_World) = 0;
  //virtual void DrawPolygon(xn::DgPolygon const &, float thickness, xn::Colour clr, uint32_t flags, xn::mat33 T_Model_World) = 0;
  //virtual void DrawFilledPolygon(xn::DgPolygon const &, xn::Colour clr, uint32_t flags, xn::mat33 T_Model_World) = 0;
  virtual void EndDraw() = 0;

  virtual unsigned int GetTexture() const = 0;
  virtual unsigned int GetWidth() const = 0;
  virtual unsigned int GetHeight() const = 0;
};

IRenderer *CreateRenderer();

#endif