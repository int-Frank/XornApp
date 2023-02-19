#ifndef ROTATEWIDGET_H
#define IROTATEWIDGET_H

#include "xnGeometry.h"
#include "Renderer.h"

class IRotateWidget
{
public:

  virtual ~IRotateWidget() {}
  virtual void Draw(Renderer *) = 0;

  virtual void Move(xn::vec2 const &position) = 0;
  virtual float SetMouse(xn::vec2 const &) = 0; // returns angle
  virtual bool MouseDown(xn::vec2 const &) = 0; // returns true if button clicked
  virtual void Unclick(xn::vec2 const &) = 0;
};

IRotateWidget *CreateRotateWidget(xn::vec2 const &anchor);

#endif