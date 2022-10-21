
#include <sstream>

#include "Message.h"

Message::Message()
  : m_flags(0)
{

}

bool Message::QueryFlag(Flag flag) const
{
  return (m_flags & (uint32_t)flag) != 0;
}

void Message::SetFlag(Flag flag, bool on)
{
  uint32_t u = (uint32_t)flag;
  if (on)
    m_flags = (m_flags & ~u);
  else
    m_flags = (m_flags | u);
}