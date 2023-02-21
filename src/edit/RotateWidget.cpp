
#include <cmath>

#include "Common.h"
#include "IRotateWidget.h"
#include "Renderer.h"
#include "xnColour.h"

class RotateWidget : public IRotateWidget
{
  struct Aspect
  {
    xn::Colour ringColour;
    float ringDiameter;
    float ringThickness;

    xn::Colour buttonColour[HS_COUNT];
    float buttonRadius;

    xn::Colour wombleColour;
    float wombleRadius;
  };

public:

  RotateWidget(xn::vec2 const &anchor);

  void Draw(xn::IRenderer *) override;
  void SetPosition(xn::vec2 const &position) override;
  float SetMouse(xn::vec2 const &) override;
  bool MouseDown(xn::vec2 const &) override;
  void Unclick(xn::vec2 const &) override;

private:

  float Angle() const;
  bool MouseHover(xn::vec2 const &);
  xn::vec2 ButtonPosition() const;

  xn::vec2 m_position;
  xn::vec2 m_womble;
  HoverState m_state;
  static Aspect const s_state;
};

RotateWidget::Aspect const RotateWidget::s_state =
{
  0x55FFFF55, 100.f, 4.f,
  {0xFFFFFFFF, 0xFF00FFFF, 0xFFFF00FF}, 15.f,
  0xFF00FF00, 5.f
};

IRotateWidget *CreateRotateWidget(xn::vec2 const &anchor)
{
  return new RotateWidget(anchor);
}

RotateWidget::RotateWidget(xn::vec2 const &anchor)
  : m_position(anchor)
  , m_womble()
  , m_state(HS_None)
{

}

void RotateWidget::SetPosition(xn::vec2 const &position)
{
  m_position = position;
}

void RotateWidget::Draw(xn::IRenderer *pRenderer)
{
  pRenderer->DrawCircle(m_position,
    s_state.ringDiameter,
    s_state.ringThickness,
    s_state.ringColour, 0);

  pRenderer->DrawFilledCircle(ButtonPosition(),
    s_state.buttonRadius,
    s_state.buttonColour[m_state], 0);

  if (m_state == HS_Active)
  {
    pRenderer->DrawFilledCircle(m_womble,
      s_state.wombleRadius,
      s_state.wombleColour, 0);
  }
}

float RotateWidget::SetMouse(xn::vec2 const &mouse)
{
  if (m_state == HS_Active)
  {
    xn::vec2 v = mouse - m_position;
    if (v.IsZero())
      v = xn::vec2(1.f, 0.f);
    m_womble = m_position + s_state.ringDiameter / 2.f * v;
  }
  else if (MouseHover(mouse))
  {
    m_state = HS_Hover;
  }
  else
  {
    m_state = HS_None;
  }
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

float RotateWidget::Angle() const
{
  return atan2f(m_womble.y(), m_womble.x());
}

xn::vec2 RotateWidget::ButtonPosition() const
{
  return m_position + xn::vec2(s_state.ringDiameter / 2.f, 0.f);
}

bool RotateWidget::MouseHover(xn::vec2 const &mouse)
{
  xn::vec2 offset = mouse - ButtonPosition();
  float radiusSq = s_state.buttonRadius;
  radiusSq *= radiusSq;
  return Dg::MagSq(offset) < radiusSq;
}