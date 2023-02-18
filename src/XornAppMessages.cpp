#include <sstream>

#include "XornAppMessages.h"

#define MESSAGE_DEFINITION(t) uint32_t Message_ ## t::GetStaticType() {return (uint32_t)MessageType::t;}\
size_t Message_ ## t::Size() {return sizeof(Message_ ## t);}\
uint32_t Message_ ## t::GetType() const {return GetStaticType();}\
Message_ ## t * Message_ ## t::Create() {return new Message_ ## t();}

#define MESSAGE_DEFAULT_STRING(t) std::string Message_ ## t::ToString() const { return std::string(#t); }

MESSAGE_DEFINITION(MouseScroll)
MESSAGE_DEFINITION(MouseButtonDown)
MESSAGE_DEFINITION(MouseButtonUp)
MESSAGE_DEFINITION(MouseMove)
MESSAGE_DEFINITION(ZoomCamera)
MESSAGE_DEFINITION(Undo)
MESSAGE_DEFINITION(Redo)

MESSAGE_DEFAULT_STRING(Undo)
MESSAGE_DEFAULT_STRING(Redo)

std::string Message_MouseScroll::ToString() const
{
  std::stringstream ss;
  ss << "MouseScroll: " << val;
  return ss.str();
}

std::string Message_MouseButtonDown::ToString() const
{
  std::stringstream ss;
  ss << "MouseButtonDown: " << (int)button << "; [" << position.x() << ", " << position.y() << "]";
  return ss.str();
}

std::string Message_MouseButtonUp::ToString() const
{
  std::stringstream ss;
  ss << "MouseButtonUp: " << (int)button << "; [" << position.x() << ", " << position.y() << "]";
  return ss.str();
}

std::string Message_MouseMove::ToString() const
{
  std::stringstream ss;
  ss << "MouseMove: [" << position.x() << ", " << position.y() << "]";
  return ss.str();
}

std::string Message_ZoomCamera::ToString() const
{
  std::stringstream ss;
  ss << "ZoomCamera: " << val;
  return ss.str();
}