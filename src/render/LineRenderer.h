#ifndef LINERENDERER_H
#define LINERENDERER_H

#include "ObjectRenderer.h"

class ILineRenderer : public ObjectRenderer
{
public:

  virtual ~ILineRenderer() {}
  virtual void Draw(std::vector<xn::seg> const &, float thickness, xn::Colour, uint32_t flags) = 0;
  virtual void SetRenderSize(xn::vec2 const &) = 0;
};

ILineRenderer * CreateLineRenderer();

#endif