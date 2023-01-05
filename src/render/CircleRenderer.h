#ifndef CIRCLERENDERER_H
#define CIRCLERENDERER_H

#include "xnColour.h"
#include "ObjectRenderer.h"

class ICircleRenderer : public ObjectRenderer
{
public:

  virtual ~ICircleRenderer() {}
  virtual void Draw(std::vector<xn::vec2> const &, float radius, xn::Colour, uint32_t flags) = 0;
  virtual void SetRenderSize(xn::vec2 const &) = 0;
};

ICircleRenderer *CreateCircleRenderer();

#endif