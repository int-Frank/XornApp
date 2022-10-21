
#ifndef MESSAGEBUS_H
#define MESSAGEBUS_H

#include <type_traits>

#include "Message.h"

class MessageBus
{
public:

  MessageBus();
  ~MessageBus();

  void Post(Message *);
  Message *PopMessage();

  template<typename T>
  T *NewMessage()
  {
    static_assert(std::is_base_of<Message, T>::value, "Type must be a subclass of Message!");
    return T::Create();
  }

private:

  class PIMPL;
  PIMPL *m_pimpl;
};

#endif