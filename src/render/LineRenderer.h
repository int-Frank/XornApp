#ifndef LINERENDERER_H
#define LINERENDERER_H

#include "ObjectRenderer.h"

class ILineRenderer : public ObjectRenderer
{
public:

  virtual ~ILineRenderer() {}
  virtual void Draw(xn::seg const *, size_t lineCount, float thickness, xn::Colour, uint32_t flags) = 0;
};

ILineRenderer * CreateLineRenderer();

#endif