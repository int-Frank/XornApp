#ifndef APP_H
#define APP_H

#include <vector>
#include <memory>

#include "Common.h"
#include "Modal.h"
#include "Project.h"
#include "Plugin.h"
#include "XornAppMessages.h"
#include "DgMap_AVL.h"
#include "Canvas.h"
#include "ActionList.h"

struct GLFWwindow;
class Logger;
class MessageBus;

namespace xn
{
  class Module;
  class UIContext;
}

class App
{
  struct ModuleData
  {
    xn::Module *pInstance;
    Plugin *pPlugin;
    bool hasFocus;
  };

public:

  App();
  ~App();

  void Run();
  void SaveProject();
  void OpenProject(std::string const &filePath);
  void ImportProject(std::string const &filePath);
  void NewProject();
  void SetSaveFile(std::string const &newFilePath);
  
  void PushModal(Modal *);

private:
  
  void ShowMenuBar();
  void ShowControlWindow();
  void HandleModals();
  void HandleModules();

  void LoadPlugins();
  void OpenModule(uint32_t id, ModuleData *);
  void LogMessage(Message const *);

  void HandleMessages();
  void HandleMessage(Message_ZoomCamera *);
  void HandleMessage(Message_MouseButtonDown *);
  void HandleMessage(Message_MouseButtonUp *);
  void HandleMessage(Message_MouseMove *);
  void HandleMessage(Message_MouseScroll *);

  xn::vec2 ViewToWorld(xn::vec2 const &);
  xn::Module *GetCurrentFocus();

  void Render();

private:

  GLFWwindow *m_pWindow;
  xn::UIContext *m_pUIContext;
  MessageBus *m_pMsgBus;
  Canvas *m_pCanvas;
  Project *m_pProject;

  Dg::Map_AVL<uint32_t, ModuleData> m_registeredModules;
  ActionList m_actions;
  xn::Transform m_T_Camera_World;
  xn::PolygonWithHoles m_scenePolygon;
  std::vector<std::shared_ptr<Modal>> m_modalStack;
  uint32_t m_activeModuleID;
  std::string m_saveFile;

  bool m_isMouseDragging;
  xn::vec2 m_mousePosition;

  bool m_geometryDirty;  // To determine if we need to recalculate geometry for the modules
  bool m_projectDirty;   // To determine if we should prompt to save project
  bool m_showDemoWindow;
  bool m_shouldQuit;
};

#endif