#ifndef MYEXCEPTION_H
#define MYEXCEPTION_H

#include <string>
#include <exception>

class MyException : public std::exception
{
public:

  MyException(std::string const &msg);
  char const *what() const override;

private:
  std::string m_msg;
};

#endif