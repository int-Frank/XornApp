#ifndef FILLEDCIRCLERENDERER_H
#define FILLEDCIRCLERENDERER_H

#include "xnColour.h"
#include "ObjectRenderer.h"

class IFilledCircleRenderer : public ObjectRenderer
{
public:

  virtual ~IFilledCircleRenderer() {}
  virtual void Draw(std::vector<xn::vec2> const &, float radius, xn::Colour, uint32_t flags) = 0;
};

IFilledCircleRenderer *CreateFilledCircleRenderer();

#endif