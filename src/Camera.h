#ifndef CAMERA_H
#define CAMERA_H

#include "xnGeometry.h"

class Camera
{
public:

  Camera();

  void Zoom(float);
  void Move(xn::vec2 const &);
  void SetWindowSize(xn::vec2 const &);

  xn::mat33 GetMatrix_Camera_World() const;

private:

  xn::Transform m_T_Camera_World;
  float m_ar;
};

#endif