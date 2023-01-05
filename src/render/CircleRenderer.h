#ifndef CIRCLERENDERER_H
#define CIRCLERENDERER_H

#include "xnColour.h"
#include "ObjectRenderer.h"

class ICircleRenderer : public ObjectRenderer
{
public:

  virtual ~ICircleRenderer() {}
  virtual void Draw(std::vector<xn::vec2> const &, xn::Colour, float radius, uint32_t flags, xn::mat33 const &T_Model_World) = 0;
  virtual void SetRenderSize(xn::vec2 const &) = 0;
};

ICircleRenderer *CreateCircleRenderer();

#endif