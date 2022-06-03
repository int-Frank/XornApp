
#include <Windows.h>
#include <string>

#include "Plugin.h"
#include "xnPluginAPI.h"
#include "xnVersion.h"
#include "Logger.h"

struct Plugin::PIMPL 
{
  HMODULE hDll;
  xnPlugin_GetVersionFn GetVersionFn;
  xnPlugin_CreateModuleFn CreateModuleFn;
  xnPlugin_DestroyModuleFn DestroyModuleFn;
  xnPlugin_GetModuleNameFn GetModuleNameFn;
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
    m_pimpl->GetVersionFn = (xnPlugin_GetVersionFn)GetProcAddress(m_pimpl->hDll, "xnPlugin_GetVersion");
    if (!m_pimpl->GetVersionFn)
    {
      errorStr = std::string("Failed to load function 'Plugin_GetVersion' from library '") + path + std::string("'");
      break;
    }

    m_pimpl->CreateModuleFn = (xnPlugin_CreateModuleFn)GetProcAddress(m_pimpl->hDll, "xnPlugin_CreateModule");
    if (!m_pimpl->CreateModuleFn)
    {
      errorStr = std::string("Failed to load function 'Plugin_CreateModule' from library '") + path + std::string("'");
      break;
    }

    m_pimpl->DestroyModuleFn = (xnPlugin_DestroyModuleFn)GetProcAddress(m_pimpl->hDll, "xnPlugin_DestroyModule");
    if (!m_pimpl->DestroyModuleFn)
    {
      errorStr = std::string("Failed to load function 'Plugin_DestroyModule' from library '") + path + std::string("'");
      break;
    }

    m_pimpl->GetModuleNameFn = (xnPlugin_GetModuleNameFn)GetProcAddress(m_pimpl->hDll, "xnPlugin_GetModuleName");
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
    LOG_WARNING("Mismatched plugin version. Plugin '%s' is version: %s, this version: %s", path.c_str(), xn::VersionNumberToString(pluginVersion), VERSION_AS_STRING);
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

xn::Module *Plugin::CreateModule(xn::ModuleInitData *pData) const
{
  return m_pimpl->CreateModuleFn(pData);
}

void Plugin::DestroyModule(xn::Module **ppModule) const
{
  m_pimpl->DestroyModuleFn(ppModule);
}

std::string Plugin::GetModuleName() const
{
  return m_pimpl->GetModuleNameFn();
}
