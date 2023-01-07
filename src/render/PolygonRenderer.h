#ifndef POLYGONRENDERER_H
#define POLYGONRENDERER_H

#include "xnColour.h"
#include "ObjectRenderer.h"

class IPolygonRenderer : public ObjectRenderer
{
public:

  virtual ~IPolygonRenderer() {}
  virtual void Draw(std::vector<xn::seg> const &, xn::Colour, uint32_t flags) = 0;
};

IPolygonRenderer *CreatePolygonRenderer();

#endif