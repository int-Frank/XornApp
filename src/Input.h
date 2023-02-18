#ifndef INPUT_H
#define INPUT_H

#define MOUSE_BUTTON_MOVE ::xn::MouseInput::RightButton
#define MOUSE_BUTTON_SELECT ::xn::MouseInput::LeftButton

enum ModKey_
{
  MK_shift  = 1 << 0,
  MK_ctrl   = 1 << 1,
  MK_alt    = 1 << 2
};

#endif