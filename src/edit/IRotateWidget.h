#ifndef IROTATEWIDGET_H
#define IROTATEWIDGET_H

#include "xnGeometry.h"
#include "xnIRenderer.h"

class IRotateWidget
{
public:

  enum class Button
  {
    None,
    Move,
    Rotate
  };

  virtual ~IRotateWidget() {}
  virtual void Draw(xn::IRenderer *) = 0;
  
  virtual void SetPosition(xn::vec2 const &) = 0;
  virtual xn::vec2 GetPosition() const = 0;
  virtual void SetMouse(xn::vec2 const &) = 0; // returns angle
  virtual Button MouseDown(xn::vec2 const &) = 0; // returns true if button clicked
  virtual void Unclick(xn::vec2 const &) = 0;
  virtual float Angle() const = 0;
};

IRotateWidget *CreateRotateWidget(xn::vec2 const &anchor);

#endif