#ifndef CONVEXPOLYGONRENDERER_H
#define CONVEXPOLYGONRENDERER_H

#include "xnColour.h"
#include "ObjectRenderer.h"

class IConvexPolygonRenderer : public ObjectRenderer
{
public:

  virtual ~IConvexPolygonRenderer() {}
  virtual void Draw(xn::vec2 const *pVerts, size_t vertCount, xn::Colour, uint32_t flags) = 0;
};

IConvexPolygonRenderer *CreateConvexPolygonRenderer();

#endif