#ifndef PLUGIN_H
#define PLUGIN_H

#include <stdint.h>
#include <string>

#include "xnModuleInitData.h"

namespace xn
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
  xn::Module *CreateModule(xn::ModuleInitData *) const;
  void DestroyModule(xn::Module **) const;
  std::string GetModuleName() const;

private:

  struct PIMPL;
  PIMPL *m_pimpl;
};

#endif