#include "ViewWindow.h"

ViewWindow::ViewWindow()
  : m_T_View_World()
  , m_ar(1.f)
{

}

void ViewWindow::Scale(float val)
{
  m_T_View_World.scale *= val;
}

void ViewWindow::SetScale(float val)
{
  m_T_View_World.scale = xn::vec2(val, val);
}

void ViewWindow::Move(xn::vec2 const &delta)
{
  m_T_View_World.translation += delta;
}

void ViewWindow::SetPosition(xn::vec2 const &pos)
{
  m_T_View_World.translation = pos;
}

xn::vec2 ViewWindow::GetPosition() const
{
  return m_T_View_World.translation;
}

void ViewWindow::SetViewSize(xn::vec2 const &v)
{
  m_ar = v.x() / v.y();
}

xn::mat33 ViewWindow::GetMatrix_View_World() const
{
  auto t = m_T_View_World;

  if (m_ar < 1.f)
    t.scale.y() /= m_ar;
  else if (m_ar > 1.f)
    t.scale.x() *= m_ar;

  return t.ToMatrix33();
}
