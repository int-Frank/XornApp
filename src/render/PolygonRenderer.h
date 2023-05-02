#ifndef POLYGONRENDERER_H
#define POLYGONRENDERER_H

#include "xnColour.h"
#include "ObjectRenderer.h"

class IPolygonRenderer : public ObjectRenderer
{
public:

  virtual ~IPolygonRenderer() {}
  virtual void Draw(xn::vec2 const *pVerts, size_t vertCount, int const *pPolySIzes, size_t polyCount, xn::Colour, uint32_t flags) = 0;
};

IPolygonRenderer *CreatePolygonRenderer();

#endif