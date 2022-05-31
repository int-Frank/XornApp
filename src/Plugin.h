#ifndef PLUGIN_H
#define PLUGIN_H

#include <stdint.h>
#include <string>

namespace a2d
{
  class Module;
  class Logger;
}

class Plugin
{
  Plugin(Plugin const &) = delete;
  Plugin & operator=(Plugin const &) = delete;

public:

  Plugin(std::string const &path);
  ~Plugin();

  uint32_t GetVersion() const;
  a2d::Module *CreateModule(bool *pShow, a2d::Logger *) const;
  void DestroyModule(a2d::Module **) const;
  std::string GetModuleName() const;

private:

  struct PIMPL;
  PIMPL *m_pimpl;
};

#endif