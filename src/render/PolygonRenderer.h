#ifndef POLYGONRENDERER_H
#define POLYGONRENDERER_H

#include "xnColour.h"
#include "ObjectRenderer.h"

class IPolygonRenderer : public ObjectRenderer
{
public:

  virtual ~IPolygonRenderer() {}
  virtual void Draw(std::vector<xn::vec2> const &vertices, std::vector<int> const &polygonSizes, xn::Colour, uint32_t flags) = 0;
};

IPolygonRenderer *CreatePolygonRenderer();

#endif