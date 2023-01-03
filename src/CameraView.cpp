#include "CameraView.h"

CameraView::CameraView()
  : m_T_View_World()
  , m_ar(1.f)
{

}

void CameraView::Scale(float val)
{
  m_T_View_World.scale *= val;
}

void CameraView::Move(xn::vec2 const &delta)
{
  m_T_View_World.translation += delta;
}

void CameraView::SetPosition(xn::vec2 const &pos)
{
  m_T_View_World.translation = pos;
}

xn::vec2 CameraView::GetPosition() const
{
  return m_T_View_World.translation;
}

void CameraView::SetViewSize(xn::vec2 const &v)
{
  m_ar = v.x() / v.y();
}

xn::mat33 CameraView::GetMatrix_View_World() const
{
  auto t = m_T_View_World;

  if (m_ar < 1.f)
    t.scale.y() /= m_ar;
  else if (m_ar > 1.f)
    t.scale.x() *= m_ar;

  return t.ToMatrix33();
}
