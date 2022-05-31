
#include <Windows.h>
#include <string>

#include "appPlugin.h"
#include "a2dPlugin_API.h"
#include "a2dVersion.h"
#include "appLogger.h"

struct Plugin::PIMPL 
{
  HMODULE hDll;
  a2dPlugin_GetVersionFn GetVersionFn;
  a2dPlugin_CreateModuleFn CreateModuleFn;
  a2dPlugin_DestroyModuleFn DestroyModuleFn;
  a2dPlugin_GetModuleNameFn GetModuleNameFn;
};

Plugin::Plugin(std::string const &path)
  : m_pimpl(new PIMPL{ NULL, nullptr, nullptr, nullptr, nullptr })
{
  m_pimpl->hDll = LoadLibraryA(path.c_str());
  if (!m_pimpl->hDll || m_pimpl->hDll == INVALID_HANDLE_VALUE) 
  {
    m_pimpl->hDll = NULL;
    throw(std::string("Failed to load library '") + path + std::string("'"));
  }

  std::string errorStr;

  do
  {
    m_pimpl->GetVersionFn = (a2dPlugin_GetVersionFn)GetProcAddress(m_pimpl->hDll, "a2dPlugin_GetVersion");
    if (!m_pimpl->GetVersionFn)
    {
      errorStr = std::string("Failed to load function 'Plugin_GetVersion' from library '") + path + std::string("'");
      break;
    }

    m_pimpl->CreateModuleFn = (a2dPlugin_CreateModuleFn)GetProcAddress(m_pimpl->hDll, "a2dPlugin_CreateModule");
    if (!m_pimpl->CreateModuleFn)
    {
      errorStr = std::string("Failed to load function 'Plugin_CreateModule' from library '") + path + std::string("'");
      break;
    }

    m_pimpl->DestroyModuleFn = (a2dPlugin_DestroyModuleFn)GetProcAddress(m_pimpl->hDll, "a2dPlugin_DestroyModule");
    if (!m_pimpl->DestroyModuleFn)
    {
      errorStr = std::string("Failed to load function 'Plugin_DestroyModule' from library '") + path + std::string("'");
      break;
    }

    m_pimpl->GetModuleNameFn = (a2dPlugin_GetModuleNameFn)GetProcAddress(m_pimpl->hDll, "a2dPlugin_GetModuleName");
    if (!m_pimpl->GetModuleNameFn)
    {
      errorStr = std::string("Failed to load function 'Plugin_GetModuleName' from library '") + path + std::string("'");
      break;
    }
  } while (false);

  if (!errorStr.empty())
  {
    if (m_pimpl->hDll != NULL)
      FreeLibrary(m_pimpl->hDll);
    delete m_pimpl;
    throw(std::string(errorStr));
  }

  uint32_t pluginVersion = m_pimpl->GetVersionFn();
  if (pluginVersion != VERSION_AS_UINT)
    LOG_WARNING("Mismatched plugin version. Plugin '%s' is version: %s, this version: %s", path.c_str(), a2d::VersionNumberToString(pluginVersion), VERSION_AS_STRING);
}

Plugin::~Plugin()
{
  if (m_pimpl->hDll != NULL)
    FreeLibrary(m_pimpl->hDll);
  delete m_pimpl;
}

uint32_t Plugin::GetVersion() const
{
  return m_pimpl->GetVersionFn();
}

a2d::Module *Plugin::CreateModule(bool *pShow, a2d::Logger *pLogger) const
{
  return m_pimpl->CreateModuleFn(pShow, pLogger);
}

void Plugin::DestroyModule(a2d::Module **ppModule) const
{
  m_pimpl->DestroyModuleFn(ppModule);
}

std::string Plugin::GetModuleName() const
{
  return m_pimpl->GetModuleNameFn();
}
