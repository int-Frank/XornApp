
#include "IRotateWidget.h"
#include "xnColour.h"

class RotateWidget : public IRotateWidget
{
  struct Aspect
  {
    xn::Colour colour;
    float anchorRadius;
    float buttonRadius;
    float lineThickness;
  };

public:

  RotateWidget(xn::vec2 const &anchor, xn::vec2 const &button);

  void Draw(xn::IScene *);

  void SetPositions(xn::vec2 const &anchor, xn::vec2 const &button);
  float SetMouse(xn::vec2 const &);
  bool MouseDown(xn::vec2 const &);
  void Unclick();

private:

  float Angle();
  bool MouseHover(xn::vec2 const &) const;

  enum State
  {
    Idle = 0,
    Hover = 1,
    Acitve = 2,
    COUNT
  };

  xn::vec2 m_anchor;
  xn::vec2 m_buttonPosition;
  xn::vec2 m_mouseOffset;
  State m_state;
  static Aspect const s_state[COUNT];
};

RotateWidget::Aspect const RotateWidget::s_state[RotateWidget::COUNT] =
{
  {0xFFFFFFFF, 3.f, 10.f, 1.f},
  {0xFF00FFFF, 4.f, 14.f, 1.5f},
  {0xFFFF00FF, 4.f, 14.f, 1.5f}
};

IRotateWidget *CreateRotateWidget(xn::vec2 const &anchor, xn::vec2 const &button)
{
  return new RotateWidget(anchor, button);
}

RotateWidget::RotateWidget(xn::vec2 const &anchor, xn::vec2 const &button)
  : m_anchor(anchor)
  , m_buttonPosition(button)
  , m_mouseOffset()
  , m_state(Idle)
{

}

void RotateWidget::Draw(xn::IScene *pScene)
{
}

void RotateWidget::SetPositions(xn::vec2 const &anchor, xn::vec2 const &button)
{
  m_anchor = anchor;
  m_buttonPosition = button;

}

float RotateWidget::SetMouse(xn::vec2 const &pos)
{
  if (m_state == Acitve)
  {

  }
  else if (MouseHover(pos))
  {
    m_state = Hover;
  }
  else
  {
    m_state = Idle;
  }
  return Angle();
}

bool RotateWidget::MouseDown(xn::vec2 const &)
{
  return false;
}

void RotateWidget::Unclick()
{
}

float RotateWidget::Angle()
{
  return 0.0f;
}

bool RotateWidget::MouseHover(xn::vec2 const &) const
{
  return false;
}