#ifndef LINERENDERER_H
#define LINERENDERER_H

#include "ObjectRenderer.h"

class ILineRenderer : public ObjectRenderer
{
public:

  virtual ~ILineRenderer() {}
  virtual void Draw(std::vector<xn::seg> const &, xn::Colour, float thickness, uint32_t flags, xn::mat33 const &T_Model_World) = 0;
  virtual void SetRenderSize(xn::vec2 const &) = 0;
};

ILineRenderer * CreateLineRenderer();

#endif