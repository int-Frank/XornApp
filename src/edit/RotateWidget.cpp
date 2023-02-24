
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

    xn::Colour rotateButtonColour;
    float rotateButtonSize;

    xn::Colour moveButtonColour;
    float moveButtonSize;

    xn::Colour wombleColour;
    float wombleRadius;
  };

public:

  RotateWidget(xn::vec2 const &anchor);

  void Draw(xn::IRenderer *) override;
  void SetPosition(xn::vec2 const &position) override;
  xn::vec2 GetPosition() const override;
  void SetMouse(xn::vec2 const &) override;
  Button MouseDown(xn::vec2 const &) override;
  void Unclick(xn::vec2 const &) override;
  float Angle() const override;

private:

  bool MouseHoverRotate(xn::vec2 const &);
  bool MouseHoverMove(xn::vec2 const &);
  xn::vec2 ButtonPosition() const;

  xn::vec2 m_position;
  xn::vec2 m_womble;
  bool m_isRotating;
  static Aspect const s_state;
};

RotateWidget::Aspect const RotateWidget::s_state =
{
  0x55FFFF55, 80.f, 4.f,
  0xFF7272F8, 12.f,
  0xFF7272F8, 8.f,
  0xFF00FF00, 10.f
};

IRotateWidget *CreateRotateWidget(xn::vec2 const &anchor)
{
  return new RotateWidget(anchor);
}

RotateWidget::RotateWidget(xn::vec2 const &anchor)
  : m_position(anchor)
  , m_womble()
  , m_isRotating(false)
{

}

void RotateWidget::SetPosition(xn::vec2 const &position)
{
  m_position = position;
  if (!m_isRotating)
    m_womble = m_position + xn::vec2(s_state.ringDiameter / 2.f, 0.f);
}

xn::vec2 RotateWidget::GetPosition() const
{
  return m_position;
}

void RotateWidget::Draw(xn::IRenderer *pRenderer)
{
  pRenderer->DrawCircle(m_position,
    s_state.ringDiameter,
    s_state.ringThickness,
    s_state.ringColour, 0);

  pRenderer->DrawFilledCircle(ButtonPosition(),
    s_state.rotateButtonSize,
    s_state.rotateButtonColour, 0);

  pRenderer->DrawFilledCircle(m_position,
    s_state.moveButtonSize,
    s_state.moveButtonColour, 0);

  if (m_isRotating)
  {
    pRenderer->DrawFilledCircle(m_womble,
      s_state.wombleRadius,
      s_state.wombleColour, 0);
  }
}

void RotateWidget::SetMouse(xn::vec2 const &mouse)
{
  if (m_isRotating)
  {
    xn::vec2 v = mouse - m_position;
    if (v.IsZero())
      v = xn::vec2(1.f, 0.f);
    v = Dg::Normalize(v);
    m_womble = m_position + s_state.ringDiameter / 2.f * v;
  }
}

RotateWidget::Button RotateWidget::MouseDown(xn::vec2 const &mouse)
{
  if (MouseHoverRotate(mouse))
  {
    m_isRotating = true;
    return Button::Rotate;
  }

  if (MouseHoverMove(mouse))
    return Button::Move;

  return Button::None;
}

void RotateWidget::Unclick(xn::vec2 const &mouse)
{
  m_isRotating = false;
  m_womble = ButtonPosition();
  SetMouse(mouse);
}

float RotateWidget::Angle() const
{
  xn::vec2 v = m_womble - m_position;
  return atan2f(v.y(), v.x());
}

xn::vec2 RotateWidget::ButtonPosition() const
{
  return m_position + xn::vec2(s_state.ringDiameter / 2.f, 0.f);
}

bool RotateWidget::MouseHoverRotate(xn::vec2 const &mouse)
{
  xn::vec2 offset = mouse - ButtonPosition();
  float radiusSq = s_state.rotateButtonSize / 2.f;
  radiusSq *= radiusSq;
  return Dg::MagSq(offset) < radiusSq;
}

bool RotateWidget::MouseHoverMove(xn::vec2 const &mouse)
{
  xn::vec2 offset = mouse - m_position;
  float radiusSq = s_state.rotateButtonSize / 2.f;
  radiusSq *= radiusSq;
  return Dg::MagSq(offset) < radiusSq;
}