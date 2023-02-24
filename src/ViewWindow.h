#ifndef VIEWWINDOW_H
#define VIEWWINDOW_H

#include "xnGeometry.h"

// Default view window is a centred on the origin with a size of
// 2 x 2 (ranging from -1 to 1).
class ViewWindow
{
public:

  ViewWindow();

  void Scale(float);
  void Move(xn::vec2 const &);

  void SetScale(float);
  void SetPosition(xn::vec2 const &);
  void SetViewSize(xn::vec2 const &);

  xn::vec2 GetPosition() const;
  xn::mat33 GetMatrix_View_World() const;

private:

  // T_View_World
  xn::vec2 m_position;
  float m_rotation;
  xn::vec2 m_scale;
  float m_ar;
};

#endif