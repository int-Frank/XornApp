
#include <cmath>

#include "Common.h"
#include "IRotateWidget.h"
#include "Renderer.h"
#include "xnColour.h"

class RotateWidget : public IRotateWidget
{
  struct Aspect
  {
    xn::Colour colour[HS_COUNT];
    float anchorRadius;
    float buttonRadius;
    float lineThickness;
  };

public:

  RotateWidget(xn::vec2 const &anchor);

  void Draw(Renderer *) override;
  void Move(xn::vec2 const &position) override;
  float SetMouse(xn::vec2 const &) override;
  bool MouseDown(xn::vec2 const &) override;
  void Unclick(xn::vec2 const &) override;

private:

  float Angle();
  bool MouseHover(xn::vec2 const &);

  xn::vec2 m_anchor;
  xn::vec2 m_buttonPosition;
  xn::vec2 m_mouseOffset;
  HoverState m_state;
  static Aspect const s_state;
};

// TODO should live in Default data?
RotateWidget::Aspect const RotateWidget::s_state =
{
  {0xFFFFFFFF, 0xFF00FFFF, 0xFFFF00FF},
  7.f, 15.f, 2.f
};

IRotateWidget *CreateRotateWidget(xn::vec2 const &anchor)
{
  return new RotateWidget(anchor);
}

RotateWidget::RotateWidget(xn::vec2 const &anchor)
  : m_anchor(anchor)
  , m_buttonPosition(anchor + xn::vec2(50.f, 0.f))
  , m_mouseOffset()
  , m_state(HS_None)
{

}

void RotateWidget::Move(xn::vec2 const &position)
{
  xn::vec2 v = position - m_anchor;
  m_anchor = position;
  m_buttonPosition += v;
}

void RotateWidget::Draw(Renderer *pRenderer)
{
  pRenderer->DrawLine(xn::seg(m_anchor, m_buttonPosition),
    s_state.lineThickness,
    s_state.colour[m_state], 0);

  pRenderer->DrawFilledCircle(m_anchor,
    s_state.anchorRadius,
    s_state.colour[m_state], 0);

  pRenderer->DrawFilledCircle(m_anchor,
    s_state.buttonRadius,
    s_state.colour[m_state], 0);
}

float RotateWidget::SetMouse(xn::vec2 const &mouse)
{
  if (m_state == HS_Active)
    m_buttonPosition = mouse;
  else if (MouseHover(mouse))
    m_state = HS_Hover;
  else
    m_state = HS_None;
  return Angle();
}

bool RotateWidget::MouseDown(xn::vec2 const &mouse)
{
  if (!MouseHover(mouse))
    return false;

  m_state = HS_Active;
  return true;
}

void RotateWidget::Unclick(xn::vec2 const &mouse)
{
  m_state = HS_None;
  SetMouse(mouse);
}

float RotateWidget::Angle()
{
  if (m_anchor == m_buttonPosition)
    return 0.f;

  xn::vec2 v = (m_buttonPosition - m_anchor);
  return atan2f(v.y(), v.x());
}

bool RotateWidget::MouseHover(xn::vec2 const &mouse)
{
  xn::vec2 offset = mouse - m_buttonPosition;
  float radiusSq = s_state.buttonRadius;
  radiusSq *= radiusSq;
  if (Dg::MagSq(offset) < radiusSq)
    return true;

  return false;
}