#include "ViewWindow.h"

ViewWindow::ViewWindow()
  : m_position(0.f, 0.f)
  , m_rotation(0.f)
  , m_scale(1.f, 1.f)
  , m_ar(1.f)
{

}

void ViewWindow::Scale(float val)
{
  m_scale *= val;
}

void ViewWindow::SetScale(float val)
{
  m_scale = xn::vec2(val, val);
}

void ViewWindow::Move(xn::vec2 const &delta)
{
  m_position += delta;
}

void ViewWindow::SetPosition(xn::vec2 const &pos)
{
  m_position = pos;
}

xn::vec2 ViewWindow::GetPosition() const
{
  return m_position;
}

void ViewWindow::SetViewSize(xn::vec2 const &v)
{
  m_ar = v.x() / v.y();
}

xn::mat33 ViewWindow::GetMatrix_View_World() const
{
  auto scale = m_scale;

  if (m_ar < 1.f)
    scale.y() /= m_ar;
  else if (m_ar > 1.f)
    scale.x() *= m_ar;

  xn::mat33 mScale, mRotation, mTranslation;

  mScale.Scaling(scale);
  mRotation.Rotation(m_rotation);
  mTranslation.Translation(m_position);

  return mScale * mRotation * mTranslation;
}
