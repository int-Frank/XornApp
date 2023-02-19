#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>

#include "xnColour.h"
#include "xnGeometry.h"
#include "xnIRenderer.h" 

class Renderer : public xn::IRenderer
{
public:

  virtual ~Renderer() {}

  virtual void SetResolution(uint32_t width, uint32_t height) = 0;

  virtual void BeginDraw() = 0;
  virtual void EndDraw() = 0;

  virtual void SetViewMatrix(xn::mat33 const &) = 0;
  virtual unsigned int GetTexture() const = 0;
  virtual unsigned int GetWidth() const = 0;
  virtual unsigned int GetHeight() const = 0;
};

Renderer *CreateRenderer();

#endif