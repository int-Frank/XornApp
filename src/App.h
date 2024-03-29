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
#include "ViewWindow.h"
#include "Scene.h"
#include "edit/IProjectController.h"

struct GLFWwindow;
class Logger;
class MessageBus;
class Renderer;

namespace xn
{
  class Module;
  class UIContext;
}

class App
{
  struct ModuleData
  {
    Plugin *pPlugin;
    xn::Module *pInstance;
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
  
  void FitViewToProject();

  void ShowMenuBar();
  void ShowControlWindow();
  void HandleModals();
  void HandleModules();

  void LoadPlugins();
  void OpenModule(uint32_t id, ModuleData *);
  void LogMessage(Message const *);

  void HandleMessages();
  void HandleMessage(Message_Undo *);
  void HandleMessage(Message_Redo *);
  void HandleMessage(Message_ZoomCamera *);
  void HandleMessage(Message_MouseButtonDown *);
  void HandleMessage(Message_MouseButtonUp *);
  void HandleMessage(Message_MouseMove *);
  void HandleMessage(Message_MouseScroll *);

  xn::vec2 ScreenToWorld(xn::vec2 const &, float w = 1.f);
  xn::Module *GetCurrentFocus();

  void Render();

private:

  GLFWwindow *m_pWindow;
  xn::UIContext *m_pUIContext;
  MessageBus *m_pMsgBus;
  Canvas *m_pCanvas;
  Renderer *m_pRenderer;
  Project *m_pProject;
  IProjectController *m_pProjectController;

  Dg::Map_AVL<uint32_t, ModuleData> m_registeredModules;
  ViewWindow m_cameraView;
  std::vector<xn::PolygonLoop> m_scenePolygonLoops;
  std::vector<std::shared_ptr<Modal>> m_modalStack;
  uint32_t m_activeModuleID;
  std::string m_saveFile;

  // Mouse dragging
  bool m_isMouseDragging;
  xn::vec2 m_mousePositionAnchor;
  xn::vec2 m_cameraPositionAnchor;

  bool m_projectDirty;  // To determine if we need to prompt to save the project
  bool m_showDemoWindow;
  bool m_shouldQuit;
};

#endif