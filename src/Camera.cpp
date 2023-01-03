#include "Camera.h"

Camera::Camera()
  : m_T_Camera_World()
  , m_ar(1.f)
{

}

void Camera::Zoom(float val)
{
  m_T_Camera_World.scale *= val;
}

void Camera::Move(xn::vec2 const &delta)
{
  m_T_Camera_World.translation.x() -= delta.x() * m_T_Camera_World.scale.x() / 100.f;
  m_T_Camera_World.translation.y() += delta.y() * m_T_Camera_World.scale.y() / 100.f;
}

void Camera::SetWindowSize(xn::vec2 const &v)
{
  m_ar = v.x() / v.y();
}

xn::mat33 Camera::GetMatrix_Camera_World() const
{
  auto t = m_T_Camera_World;
  t.scale.x() *= m_ar;
  return t.ToMatrix33();
}
