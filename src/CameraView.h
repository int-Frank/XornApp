#ifndef CAMERAVIEW_H
#define CAMERAVIEW_H

#include "xnGeometry.h"

// Default camera is a centred on the origin with a size of
// 2 x 2 (ranging from -1 to 1).
class CameraView
{
public:

  CameraView();

  void Scale(float);
  void Move(xn::vec2 const &);

  void SetScale(float);
  void SetPosition(xn::vec2 const &);
  void SetViewSize(xn::vec2 const &);

  xn::vec2 GetPosition() const;
  xn::mat33 GetMatrix_View_World() const;

private:

  xn::Transform m_T_View_World;
  float m_ar;
};

#endif