#include "MyException.h"

MyException::MyException(std::string const &msg)
{
  m_msg = msg;
}

char const *MyException::what() const
{
  return m_msg.c_str();
}