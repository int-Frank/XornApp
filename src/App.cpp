
#include <filesystem>
#include <stdio.h>

#include "glad/glad.h"
#include "glfw3.h"

#include "xnModule.h"
#include "xnModuleInitData.h"

#include "App.h"
#include "Logger.h"
#include "IRenderer.h"
#include "DefaultData.h"
#include "ImGuiUIContext.h"
#include "MessageBus.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// Yuck global, but need this so ImGui can push messages.
MessageBus *g_pMsgBus = nullptr;

#define MAIN_WINDOW_NAME "Control"

#define INVALID_ID 0xFFFFFFFFul
#define DISPATCH(t) case (uint32_t)MessageType::t:{HandleMessage((Message_ ## t*)pMsg); break;}

static void glfw_error_callback(int error, const char *description)
{
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

App::App()
  : m_pWindow(nullptr)
  , m_registeredModules()
  , m_pUIContext(nullptr)
  , m_pMsgBus(nullptr)
  , m_pCanvas(nullptr)
  , m_pRenderer(nullptr)
  , m_pScene(nullptr)
  , m_activeModuleID(INVALID_ID)
  , m_cameraView()
  , m_modalStack()
  , m_pProject(nullptr)
  , m_pProjectController(nullptr)
  , m_saveFile()
  , m_isMouseDragging(false)
  , m_mousePositionAnchor(0.f, 0.f)
  , m_cameraPositionAnchor(0.f, 0.f)
  , m_geometryDirty(true)
  , m_projectDirty(false)
  , m_showDemoWindow(false)
  , m_shouldQuit(false)
{
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    throw std::exception("Failed to initialise GLFW");

  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
  m_pWindow = glfwCreateWindow((int)DefaultData::data.windowWidth, (int)DefaultData::data.windowHeight, "Adventures in 2D", NULL, NULL);
  if (m_pWindow == NULL)
    throw std::exception("Failed to initialise GLFW Window");

  glfwMakeContextCurrent(m_pWindow);
  glfwSwapInterval(1); // Enable vsync

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    throw std::exception("Failed to initialise glad");

  GLint major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);

  LOG_INFO("OpenGL context: %i.%i", major, minor);

  m_pMsgBus = new MessageBus();
  g_pMsgBus = m_pMsgBus;
  m_pUIContext = new ImGuiUIContext(m_pWindow, true, glsl_version);
  m_pRenderer = CreateRenderer();
  m_pCanvas = new Canvas("Output", m_pMsgBus);
  m_pCanvas->SetSize(xn::vec2(DefaultData::data.windowWidth, DefaultData::data.windowHeight));
  m_pScene = new Scene();

  NewProject();
  LoadPlugins();
}

void App::Run()
{
  while (!m_shouldQuit && !glfwWindowShouldClose(m_pWindow))
  {
    glfwPollEvents();

    int w, h;
    glfwGetWindowSize(m_pWindow, &w, &h);
    m_pCanvas->SetSize(xn::vec2((float)w, (float(h))));

    m_pUIContext->NewFrame();
    HandleMessages();
    ShowControlWindow();

    HandleModals();
    HandleModules();
    
    if (m_showDemoWindow)
      ImGui::ShowDemoWindow(&m_showDemoWindow);

    Render();
  }
}

App::~App()
{
  delete m_pCanvas;
  delete m_pUIContext;
  delete m_pProject;
  delete m_pScene;
  delete m_pMsgBus;
  delete m_pProjectController;
  g_pMsgBus = nullptr;

  for (auto &kv : m_registeredModules)
  {
    if (kv.second.pInstance != nullptr)
      kv.second.pPlugin->DestroyModule(&kv.second.pInstance);
    delete kv.second.pPlugin;
    delete kv.second.pScene;
  }

  glfwDestroyWindow(m_pWindow);
  glfwTerminate();
}

void App::ShowMenuBar()
{
  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("Menu"))
    {
      if (ImGui::MenuItem("New"))
      {
        PushModal(new Modal_NewProject());
        if (m_projectDirty)
          PushModal(new Modal_SavePrompt());
      }

      if (ImGui::MenuItem("Open", "Ctrl+O"))
      {
        PushModal(new Modal_Open());
        if (m_projectDirty)
          PushModal(new Modal_SavePrompt());
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Import"))
      {
        PushModal(new Modal_Import());
        if (m_projectDirty)
          PushModal(new Modal_SavePrompt());
      }

      if (ImGui::MenuItem("Export as .obj"))
        PushModal(new Modal_Export());

      ImGui::Separator();
      if (ImGui::MenuItem("Save", "Ctrl+S"))
      {
        if (m_saveFile.empty())
          PushModal(new Modal_SaveAs());
        else
          SaveProject();
      }

      if (ImGui::MenuItem("Save As.."))
        PushModal(new Modal_SaveAs());

      ImGui::Separator();
      if (ImGui::MenuItem("Exit", "Ctrl+X")) {}
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Options"))
    {
      ImGui::MenuItem("Show demo window", NULL, &m_showDemoWindow);
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Modules"))
    {
      for (auto &kv : m_registeredModules)
      {
        if (ImGui::MenuItem(kv.second.pPlugin->GetModuleName().c_str(), nullptr, kv.second.pInstance != nullptr))
          OpenModule(kv.first, &kv.second);
      }

      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}

void App::ShowControlWindow()
{
  ImGui::Begin(MAIN_WINDOW_NAME, nullptr, ImGuiWindowFlags_MenuBar);

  ShowMenuBar();

  ImGui::Text("%s", std::filesystem::path(m_saveFile).stem().string().c_str());
  ImGui::Separator();

  //ImGui::ColorEdit4("Line colour##" MAIN_WINDOW_NAME, m_lineColour, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaPreview);
  //ImGui::Separator();
  ImGui::Text("Mouse input gets sent to...");

  bool hasFocus = GetCurrentFocus() == nullptr;
  if (ImGui::Checkbox(MAIN_WINDOW_NAME, &hasFocus))
  {
    for (auto it = m_registeredModules.begin(); it != m_registeredModules.end(); it++)
        it->second.hasFocus = false;
  }

  for (auto it = m_registeredModules.begin(); it != m_registeredModules.end(); it++)
  {
    if (it->second.pInstance == nullptr)
      continue;

    if (ImGui::Checkbox(it->second.pPlugin->GetModuleName().c_str(), &it->second.hasFocus))
    {
      it->second.hasFocus = true;
      for (auto it2 = m_registeredModules.begin(); it2 != m_registeredModules.end(); it2++)
      {
        if (it->first != it2->first)
          it2->second.hasFocus = false;
      }
    }
  }

  ImGui::End();
}

void App::HandleModals()
{
  if (m_modalStack.size() == 0)
    return;

  size_t back = m_modalStack.size() - 1;
  if (!m_modalStack[back]->Show(this))
    m_modalStack.erase(m_modalStack.begin() + back);
}

void App::HandleModules()
{
  if (m_geometryDirty)
    m_scenePolygonLoops = m_pProject->loops.GetLoops();

  for (auto &kv : m_registeredModules)
  {
    if (kv.second.pInstance == nullptr)
      continue;

    if (m_geometryDirty)
      kv.second.pInstance->SetGeometry(m_scenePolygonLoops);

    kv.second.pInstance->DoFrame(m_pUIContext, kv.second.pScene);

    if (!kv.second.pInstance->IsOpen())
    {
      kv.second.pPlugin->DestroyModule(&kv.second.pInstance);
      delete kv.second.pScene;
      kv.second.pScene = nullptr;
    }
  }
  m_geometryDirty = false;
}

void App::LoadPlugins()
{
  auto pluginList = GetDirectoriesFromFolder(DefaultData::data.pluginsPath);
  uint32_t ID = 1;
  for (auto const &name : pluginList)
  {
    try
    {
      ModuleData data = {};
      data.pPlugin = new Plugin(DefaultData::data.pluginsPath + name + "/" + name + ".dll");
      m_registeredModules[ID] = data;
      ID++;
    }
    catch (std::exception e)
    {
      LOG_ERROR("Failed to load plugin '%s'. Error: %s", name.c_str(), e.what());
    }
  }
}

void App::OpenModule(uint32_t id, ModuleData *pData)
{
  if (pData->pInstance == nullptr)
  {
    xn::ModuleInitData data{};
    data.ID = id;
    data.pLogger = GetLogger();
    data.name = pData->pPlugin->GetModuleName();

    pData->pInstance = pData->pPlugin->CreateModule(&data);
    pData->pScene = new Scene();
    pData->pInstance->SetGeometry(m_scenePolygonLoops);
  }
}

void App::LogMessage(Message const *pMsg)
{
  std::string str = pMsg->ToString();
  LOG_DEBUG("MSG: %s", str.c_str());
}

void App::HandleMessages()
{
  Message *pMsg = m_pMsgBus->PopMessage();
  while (pMsg != nullptr)
  {
    //LogMessage(pMsg);

    switch (pMsg->GetType())
    {
      DISPATCH(MouseScroll);
      DISPATCH(ZoomCamera);
      DISPATCH(Undo);
      DISPATCH(Redo);
      DISPATCH(MouseButtonDown);
      DISPATCH(MouseButtonUp);
      DISPATCH(MouseMove);
    default:
      LOG_DEBUG("Message not handled: '%s'", pMsg->ToString().c_str());
    }

    delete pMsg;
    pMsg = m_pMsgBus->PopMessage();
  }
}

void App::Render()
{
  // TODO Set window size
  m_pCanvas->BeginFrame();
  m_pRenderer->BeginDraw();
  
  xn::vec2 renderSize = m_pCanvas->GetRenderRegionSize();

  uint32_t w = uint32_t(renderSize.x());
  uint32_t h = uint32_t(renderSize.y());

  m_pRenderer->SetResolution(uint32_t(renderSize.x()), uint32_t(renderSize.y()));
  m_cameraView.SetViewSize(renderSize);

  xn::mat33 T_View_World = m_cameraView.GetMatrix_View_World();
  xn::mat33 T_Screen_View = m_pCanvas->GetMatrix_Screen_View();
  xn::mat33 T_World_View = T_View_World.GetInverse();
  xn::mat33 T_World_Screen = xn::mat33(T_Screen_View * T_View_World).GetInverse();

  m_pRenderer->SetMatrix_World_View(T_World_View);
  m_pProjectController->SetMatrix_World_Screen(T_World_Screen);
  m_pProjectController->UpdateScene(m_pScene);

  m_pScene->Draw(m_pRenderer);
  m_pScene->Clear();

  // TODO sort module scenes to draw correctly.
  for (auto &kv : m_registeredModules)
  {
    if (kv.second.pInstance == nullptr)
      continue;

    kv.second.pScene->Draw(m_pRenderer);
    kv.second.pScene->Clear();
  }

  m_pRenderer->EndDraw();
  m_pCanvas->BlitImage((void*)m_pRenderer->GetTexture(), w, h);
  m_pCanvas->EndFrame();

  // Rendering
  m_pUIContext->Compose();
  int display_w, display_h;
  glfwGetFramebufferSize(m_pWindow, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  m_pUIContext->Draw();
  glfwSwapBuffers(m_pWindow);
}

xn::vec2 App::ScreenToWorld(xn::vec2 const &p, float w)
{
  xn::mat33 T_Screen_View = m_pCanvas->GetMatrix_Screen_View();
  xn::mat33 T_View_World = m_cameraView.GetMatrix_View_World();
  xn::vec3 pWorldSpace3(p.x(), p.y(), w);
  pWorldSpace3 = pWorldSpace3 * (T_Screen_View * T_View_World);
  return xn::vec2(pWorldSpace3.x(), pWorldSpace3.y());
}

xn::Module *App::GetCurrentFocus()
{
  for (auto it = m_registeredModules.begin(); it != m_registeredModules.end(); it++)
  {
    if (it->second.hasFocus)
      return it->second.pInstance;
  }
  return nullptr;
}

void App::HandleMessage(Message_ZoomCamera *pMsg)
{
  m_cameraView.Scale(pMsg->val);
}

void App::HandleMessage(Message_Undo *pMsg)
{
  m_pProjectController->Undo();
}

void App::HandleMessage(Message_Redo *pMsg)
{
  m_pProjectController->Redo();
}

void App::HandleMessage(Message_MouseButtonUp *pMsg)
{
  xn::Module *pFocus = GetCurrentFocus();

  if (pFocus)
  {
    pFocus->MouseUp(pMsg->button);
  }
  else
  {
    if (pMsg->button == MOUSE_BUTTON_MOVE)
    {
      m_isMouseDragging = false;
    }
    if (pMsg->button == MOUSE_BUTTON_SELECT)
    {
      m_pProjectController->MouseUp(pMsg->modState, pMsg->position);
    }
  }
}

void App::HandleMessage(Message_MouseButtonDown *pMsg)
{
  xn::Module *pFocus = GetCurrentFocus();

  if (pFocus)
  {
    pFocus->MouseDown(pMsg->button, ScreenToWorld(pMsg->position));
  }
  else
  {
    if (pMsg->button == MOUSE_BUTTON_MOVE)
    {
      m_mousePositionAnchor = pMsg->position;
      m_cameraPositionAnchor = m_cameraView.GetPosition();
      m_isMouseDragging = true;
    }
    if (pMsg->button == MOUSE_BUTTON_SELECT)
    {
      m_pProjectController->MouseDown(pMsg->modState, pMsg->position);
    }
  }
}

void App::HandleMessage(Message_MouseMove *pMsg)
{
  xn::Module *pFocus = GetCurrentFocus();

  if (pFocus)
  {
    pFocus->MouseMove(ScreenToWorld(pMsg->position));
  }
  else
  {
    if (m_isMouseDragging)
    {
      xn::vec2 vWorldSpace = ScreenToWorld(pMsg->position - m_mousePositionAnchor, 0.f);
      m_cameraView.SetPosition(m_cameraPositionAnchor - vWorldSpace);
    }
    else
    {
      m_pProjectController->MouseMove(pMsg->position);
    }
  }
}

void App::HandleMessage(Message_MouseScroll *pMsg)
{
    m_pCanvas->Handle(pMsg);
}

void App::SaveProject()
{
  if ((m_saveFile.size() < 5) || (m_saveFile.substr(m_saveFile.size() - 5, 5) != ".json"))
    m_saveFile += std::string(".json");

  if (!m_pProject->Write(m_saveFile))
  {
    LOG_ERROR("Failed to save project file '%s'", m_saveFile.c_str());
  }
  else
  {
    LOG_INFO("Saved to %s", m_saveFile.c_str());
  }
  m_projectDirty = false;
}

void App::FitViewToProject()
{
  xn::aabb aabb;
  if (!m_pProject->loops.GetAABB(&aabb))
    return;

  xn::vec2 size = aabb.Diagonal();
  xn::vec2 centre = aabb.GetCenter();

  float scale = size.x() > size.y() ? size.x() : size.y();
  scale *= 0.75f;

  m_cameraView.SetPosition(centre);
  m_cameraView.SetScale(scale);
  m_cameraView.SetViewSize(size);
}

void App::OpenProject(std::string const &filePath)
{
  Project *pNewProject = new Project();
  if (!pNewProject->Read(filePath))
  {
    LOG_ERROR("Failed to open project file '%s'", filePath.c_str());
    delete pNewProject;
    return;
  }

  delete m_pProject;
  m_pProject = pNewProject;
  m_saveFile = filePath;
  m_geometryDirty = true;
  m_projectDirty = false;

  FitViewToProject();

  delete m_pProjectController;
  m_pProjectController = CreateProjectController(m_pProject);
}

void App::ImportProject(std::string const &filePath)
{
  Project *pNewProject = new Project();
  if (!pNewProject->ReadFromOBJFile(filePath))
  {
    LOG_ERROR("Failed to import obj file '%s'", filePath.c_str());
    delete pNewProject;
    return;
  }

  delete m_pProject;
  m_pProject = pNewProject;
  m_saveFile = filePath + ".json";
  m_geometryDirty = true;
  m_projectDirty = false;

  FitViewToProject();

  delete m_pProjectController;
  m_pProjectController = CreateProjectController(m_pProject);
}

void App::NewProject()
{
  delete m_pProject;
  m_pProject = new Project();

  ScenePolygonLoop loop;
  loop.vertices = DefaultData::data.defaultBoundary;

  m_pProject->loops.Add(loop);

  m_geometryDirty = true;
  m_projectDirty = true;
  m_saveFile.clear();

  FitViewToProject();

  delete m_pProjectController;
  m_pProjectController = CreateProjectController(m_pProject);
}

void App::SetSaveFile(std::string const &newFilePath)
{
  m_saveFile = newFilePath;
}

void App::PushModal(Modal *pModal)
{
  m_modalStack.push_back(std::shared_ptr<Modal>(pModal));
}