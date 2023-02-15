#ifndef ROTATEWIDGET_H
#define IROTATEWIDGET_H

#include "xnGeometry.h"

namespace xn
{
  class IScene;
}

class IRotateWidget
{
public:

  virtual ~IRotateWidget() {}
  virtual void Draw(xn::IScene *) = 0;

  virtual void SetPositions(xn::vec2 const &anchor, xn::vec2 const &button) = 0;
  virtual float SetMouse(xn::vec2 const &) = 0; // returns angle
  virtual bool MouseDown(xn::vec2 const &) = 0; // returns true if button clicked
  virtual void Unclick() = 0;
};

IRotateWidget *CreateRotateWidget(xn::vec2 const &anchor, xn::vec2 const &button);

#endif