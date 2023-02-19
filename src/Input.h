#ifndef INPUT_H
#define INPUT_H

#define MOUSE_BUTTON_MOVE ::MouseInput::RightButton
#define MOUSE_BUTTON_SELECT ::MouseInput::LeftButton
#define MOUSE_BUTTON_MODULE ::MouseInput::MiddleButton

enum class MouseInput
{
  LeftButton,
  RightButton,
  MiddleButton,
  COUNT
};

#endif