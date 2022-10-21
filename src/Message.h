#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <string>
#include <type_traits>

class Message
{
public:

  enum class Flag : uint32_t
  {
    Handled = 1 << 0
  };

public:

  Message();

  bool QueryFlag(Flag flag) const;
  void SetFlag(Flag flag, bool);

  virtual uint32_t GetType() const = 0;
  virtual std::string ToString() const = 0;

protected:
  uint32_t m_flags;
};

#define MESSAGE_HEADER(t) \
  class Message_ ## t : public ::Message {\
  public:\
    static size_t Size();\
    static uint32_t GetStaticType();\
    uint32_t GetType() const override;\
    std::string ToString() const override;\
    static Message_ ## t * Create();

#endif