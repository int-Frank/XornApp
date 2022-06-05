#include <sstream>

#include "XornAppMessages.h"

// TODO Can be taken from XornCore
#define MESSAGE_DEFINITION(t) uint32_t Message_ ## t::GetStaticType() {return (uint32_t)MessageType::t;}\
size_t Message_ ## t::Size() {return sizeof(Message_ ## t);}\
uint32_t Message_ ## t::GetType() const {return GetStaticType();}\
Message_ ## t * Message_ ## t::Create() {return new Message_ ## t();}

#define MESSAGE_DEFAULT_STRING(t) std::string Message_ ## t::ToString() const { return std::string(#t); }

MESSAGE_DEFINITION(MouseScroll)
MESSAGE_DEFINITION(ZoomCamera)
MESSAGE_DEFINITION(MoveCamera)

std::string Message_MouseScroll::ToString() const
{
  std::stringstream ss;
  ss << "MouseScroll: " << val;
  return ss.str();
}

std::string Message_ZoomCamera::ToString() const
{
  std::stringstream ss;
  ss << "ZoomCamera: " << val;
  return ss.str();
}

std::string Message_MoveCamera::ToString() const
{
  std::stringstream ss;
  ss << "MoveCamera: [" << v.x() << ", " << v.y() << "]";
  return ss.str();
}