#ifndef APP_H
#define APP_H

#include <vector>
#include <memory>

#include "Common.h"
#include "Modal.h"
#include "Camera.h"
#include "Project.h"
#include "Plugin.h"
#include "MemoryManager.h"
#include "xnMessageBus.h"
#include "xnXornMessages.h"
#include "XornAppMessages.h"
#include "DgMap_AVL.h"
#include "Canvas.h"

struct GLFWwindow;
class Logger;

namespace xn
{
  class Module;
  class UIContext;
}

class App
{
  struct ModuleData
  {
    xn::Module *pModule;
    Plugin *pPlugin;
  };

public:

  App();
  ~App();

  void Run();
  void SaveProject();
  void OpenProject(std::string const &filePath);
  void NewProject(std::string const &boundary);
  void SetSaveFile(std::string const &newFilePath);
  void AddModelFromFile(std::string const &fileName, std::string const &name);
  
  void MakeDirty();
  void PushModal(Modal *);

private:
  
  void ShowMenuBar();
  void ShowControlWindow();
  void HandleModals();

  void LoadPlugins();
  void OpenModule(uint32_t id, ModuleData *);
  void HandleMessages();

  void Render();

  void DispatchToFocus(xn::Message *pMsg);
  void DispatchToAllModules(xn::Message *pMsg);

  void HandleMessage(xn::Message_WindowClosed *pMsg);
  void HandleMessage(xn::Message_WindowGainedFocus *pMsg);
  void HandleMessage(xn::Message_WindowLostFocus *pMsg);
  void HandleMessage(Message_MouseScroll *pMsg);
  void HandleMessage(Message_ZoomCamera *pMsg);
  void HandleMessage(Message_MoveCamera *pMsg);

private:

  // Persistant data
  GLFWwindow *m_pWindow;
  Dg::Map_AVL<uint32_t, ModuleData> m_registeredModules;
  xn::UIContext *m_pUIContext;
  xn::MessageBus m_msgBus;
  SafeMemoryManager m_memMngr;
  Canvas *m_pCanvas;

  // Temp data
  uint32_t m_moduleFocusID;
  Camera m_camera;
  std::vector<std::shared_ptr<Modal>> m_modalStack;
  Project *m_pCurrentProject;
  std::string m_saveFile;
  xn::PolygonGroup m_sanitisedGeom;
  bool m_geometryDirty;
  bool m_projectDirty;
  bool m_showDemoWindow;
  bool m_shouldQuit;
};

#endif