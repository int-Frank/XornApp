#ifndef APP_H
#define APP_H

#include <list>
#include <vector>
#include <memory>

#include "appCommon.h"
#include "appModal.h"
#include "appCamera.h"
#include "appProject.h"
#include "appPlugin.h"

struct GLFWwindow;
class Logger;

namespace a2d
{
  class Module;
  class UIContext;
}

class App
{
  struct ModuleData
  {
    bool show;
    a2d::Module *pModule;
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
  void ShowOutputWindow();
  void ShowControlWindow();
  void HandleModals();

  void LoadPlugins();
  void OpenModule(ModuleData *);

private:

  // Persistant data
  GLFWwindow *m_pWindow;
  std::list<ModuleData> m_registeredModules; // TODO this should be a Dg::DoublyLinkedList, but the Dg::DoublyLinkedList needs to std::move instead of memcpy when moving objects around under the hood
  a2d::UIContext *m_pUIContext;

  // Temp data
  Camera m_camera;
  std::vector<std::shared_ptr<Modal>> m_modalStack;
  Project *m_pCurrentProject;
  std::string m_saveFile;
  a2d::Geometry m_sanitisedGeom;
  a2d::Renderer *m_pRenderer;
  bool m_geometryDirty;
  bool m_projectDirty;
  bool m_showDemoWindow;
  bool m_shouldQuit;
};

#endif