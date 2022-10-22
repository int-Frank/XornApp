
#include <filesystem>
#include <stdio.h>

#include "xnModule.h"
#include "App.h"
#include "Logger.h"
#include "ImGuiRenderer.h"
#include "DefaultData.h"
#include "glfw3.h"
#include "ImGuiUIContext.h"
#include "MessageBus.h"
#include "xnModuleInitData.h"

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
  , m_actions()
  , m_activeModuleID(INVALID_ID)
  , m_T_Camera_World()
  , m_modalStack()
  , m_pProject(nullptr)
  , m_saveFile()
  , m_isMouseDragging(false)
  , m_mousePosition(0.f, 0.f)
  , m_hasFocus(true)
  , m_geometryDirty(true)
  , m_projectDirty(false)
  , m_showDemoWindow(false)
  , m_shouldQuit(false)
{
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    throw std::exception("Failed to initialise GLFW");

  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
  m_pWindow = glfwCreateWindow((int)DefaultData::data.windowWidth, (int)DefaultData::data.windowHeight, "Adventures in 2D", NULL, NULL);
  if (m_pWindow == NULL)
    throw std::exception("Failed to initialise GLFW Window");

  glfwMakeContextCurrent(m_pWindow);
  glfwSwapInterval(1); // Enable vsync

  m_pMsgBus = new MessageBus();
  g_pMsgBus = m_pMsgBus;
  m_pUIContext = new ImGuiUIContext(m_pWindow, true, glsl_version);
  m_pCanvas = new Canvas("Output", m_pMsgBus, new ImGuiRenderer());
  m_pCanvas->SetSize(xn::vec2(DefaultData::data.windowWidth, DefaultData::data.windowHeight));

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
  delete m_pMsgBus;
  g_pMsgBus = nullptr;

  for (auto &kv : m_registeredModules)
  {
    if (kv.second.pInstance != nullptr)
      kv.second.pPlugin->DestroyModule(&kv.second.pInstance);
    delete kv.second.pPlugin;
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
  ImGui::Text("Mouse input gets sent to...");
  if (ImGui::Checkbox(MAIN_WINDOW_NAME, &m_hasFocus))
  {
    m_hasFocus = true;
    for (auto it = m_registeredModules.begin(); it != m_registeredModules.end(); it++)
        it->second.hasFocus = false;
  }

  for (auto it = m_registeredModules.begin(); it != m_registeredModules.end(); it++)
  {
    if (it->second.pInstance == nullptr)
      continue;

    if (ImGui::Checkbox(it->second.pPlugin->GetModuleName().c_str(), &it->second.hasFocus))
    {
      m_hasFocus = false;
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
    m_scenePolygon = m_pProject->loops.BuildScenePolygon();

  for (auto &kv : m_registeredModules)
  {
    if (kv.second.pInstance == nullptr)
      continue;

    if (m_geometryDirty)
      kv.second.pInstance->SetGeometry(m_scenePolygon);

    kv.second.pInstance->DoFrame(m_pUIContext);

    if (!kv.second.pInstance->IsOpen())
      kv.second.pPlugin->DestroyModule(&kv.second.pInstance);
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
    pData->pInstance->SetGeometry(m_scenePolygon);
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
  xn::Renderer *pRenderer = m_pCanvas->GetRenderer();
  pRenderer->BeginDraw();

  xn::mat33 T_World_View = m_T_Camera_World.ToMatrix33().GetInverse() * m_pCanvas->Get_T_Camera_View();

  for (auto it = m_pProject->loops.Begin(); it != m_pProject->loops.End(); it++)
  {
    auto opts = it->second.flags.QueryFlag(ScenePolygonLoopFlag::Invalid) ? DefaultData::data.invalidPolygon : DefaultData::data.validPolygon;
    auto T_Model_World = it->second.T_Model_World.ToMatrix33();
    auto T_Model_View = T_Model_World * T_World_View;
    auto loop = it->second.loop.GetTransformed(T_Model_View);
    loop.Render(pRenderer, opts);
  }

  for (auto &kv : m_registeredModules)
  {
    if (kv.second.pInstance == nullptr)
      continue;

    kv.second.pInstance->Render(pRenderer, T_World_View);
  }
  pRenderer->EndDraw();
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

xn::vec2 App::ViewToWorld(xn::vec2 const &p)
{
  xn::Renderer *pRenderer = m_pCanvas->GetRenderer();
  xn::mat33 T_World_View = m_T_Camera_World.ToMatrix33().GetInverse() * m_pCanvas->Get_T_Camera_View();
  xn::vec3 p1 = xn::vec3(p.x(), p.y(), 1.f) * T_World_View.GetInverse();
  return xn::vec2(p1.x(), p1.y());
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
  m_T_Camera_World.scale *= pMsg->val;
}

void App::HandleMessage(Message_MouseButtonUp *pMsg)
{
  if (m_hasFocus)
  {
    if (pMsg->button == xn::MouseInput::LeftButton)
    {
      m_isMouseDragging = false;
    }
  }
  else
  {
    GetCurrentFocus()->MouseUp(pMsg->button);
  }
}

void App::HandleMessage(Message_MouseButtonDown *pMsg)
{
  if (m_hasFocus)
  {
    if (pMsg->button == xn::MouseInput::LeftButton)
    {
      m_mousePosition = pMsg->position;
      m_isMouseDragging = true;
    }
  }
  else
  {
    GetCurrentFocus()->MouseDown(pMsg->button, ViewToWorld(pMsg->position));
  }
}

void App::HandleMessage(Message_MouseMove *pMsg)
{
  if (m_hasFocus)
  {
    if (m_isMouseDragging)
    {
      xn::vec2 delta = pMsg->position - m_mousePosition;
      m_mousePosition = pMsg->position;
      m_T_Camera_World.translation.x() -= delta.x() * m_T_Camera_World.scale.x();
      m_T_Camera_World.translation.y() += delta.y() * m_T_Camera_World.scale.y();
    }
  }
  else
  {
    GetCurrentFocus()->MouseMove(ViewToWorld(pMsg->position));
  }
}

void App::HandleMessage(Message_MouseScroll *pMsg)
{
  if (m_hasFocus)
  {
    m_pCanvas->Handle(pMsg);
  }
  else
  {
    GetCurrentFocus()->MouseScroll(pMsg->val);
  }
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
}

void App::NewProject()
{
  delete m_pProject;
  m_pProject = new Project();

  ScenePolygonLoop loop;
  loop.loop = DefaultData::data.defaultBoundary;

  m_pProject->loops.Add(loop);

  m_geometryDirty = true;
  m_projectDirty = true;
  m_saveFile.clear();
}

void App::SetSaveFile(std::string const &newFilePath)
{
  m_saveFile = newFilePath;
}

void App::PushModal(Modal *pModal)
{
  m_modalStack.push_back(std::shared_ptr<Modal>(pModal));
}