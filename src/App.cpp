
#include <filesystem>
#include <stdio.h>

#include "xnModule.h"
#include "App.h"
#include "Logger.h"
#include "ImGuiRenderer.h"
#include "DefaultData.h"
#include "glfw3.h"
#include "ImGuiUIContext.h"
#include "xnModuleInitData.h"
#include "xnXornMessages.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#define INVALID_ID 0xFFFFFFFFul
#define DISPATCH(t) case (uint32_t)MessageType::t:{HandleMessage((Message_ ## t*)pMsg); break;}
#define xnDISPATCH(t) case (uint32_t)xn::MessageType::t:{HandleMessage((xn::Message_ ## t*)pMsg); break;}
#define DISPATCH_TO_FOCUS_MODULE(t) case (uint32_t)xn::MessageType::t:{DispatchToFocus(pMsg); break;}
#define DISPATCH_TO_ALL_MODULES(t) case (uint32_t)xn::MessageType::t:{DispatchToAllModules(pMsg); break;}

xn::MessageBus *g_pMsgBus = nullptr;

static void glfw_error_callback(int error, const char *description)
{
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static xn::LineProperties MakeFocus(xn::LineProperties const &opt)
{
  xn::LineProperties result;
  result.thickness = opt.thickness + 2.f;

  float mult = .6f;
  for (int i = 0; i < 4; i++)
    result.clr.rgba8[i] = opt.clr.rgba8[i] + uint8_t(float(255 - opt.clr.rgba8[i]) * mult);

  return result;
}

App::App()
  : m_pWindow(nullptr)
  , m_registeredModules()
  , m_pUIContext(nullptr)
  , m_msgBus()
  , m_memMngr()
  , m_pCanvas(nullptr)
  , m_moduleFocusID(INVALID_ID)
  , m_camera()
  , m_modalStack()
  , m_pCurrentProject(Project::CreateDefaultProject())
  , m_saveFile()
  , m_sanitisedGeom()
  , m_geometryDirty(false)
  , m_projectDirty(false)
  , m_showDemoWindow(false)
  , m_shouldQuit(false)
{
  g_pMsgBus = &m_msgBus;

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

  m_pUIContext = new ImGuiUIContext(m_pWindow, true, glsl_version);
  m_pCanvas = new Canvas("Output", new ImGuiRenderer(), &m_msgBus);
  m_pCanvas->SetSize(xn::vec2(DefaultData::data.windowWidth, DefaultData::data.windowHeight));

  MakeDirty();
  LoadPlugins();
}

void App::Run()
{
  while (!m_shouldQuit && !glfwWindowShouldClose(m_pWindow))
  {
    glfwPollEvents();
    m_pUIContext->NewFrame();
    HandleMessages();
    ShowControlWindow();

    HandleModals();

    if (m_geometryDirty)
    {
      m_sanitisedGeom.polygons.clear();
      if (!m_pCurrentProject->GetSanitisedGeometry(&m_sanitisedGeom))
      {
        LOG_ERROR("Failed to sanitise geometry!");
        m_geometryDirty = false;
      }
    }

    for (auto &kv : m_registeredModules)
    {
      if (kv.second.pModule == nullptr)
        continue;

      kv.second.pModule->DoFrame(m_pUIContext);

      if (m_geometryDirty)
        kv.second.pModule->SetGeometry(m_sanitisedGeom);
    }

    m_geometryDirty = false;

    if (m_showDemoWindow)
      ImGui::ShowDemoWindow(&m_showDemoWindow);

    Render();
  }
}

App::~App()
{
  delete m_pCanvas;
  delete m_pUIContext;

  for (auto &kv : m_registeredModules)
  {
    if (kv.second.pModule != nullptr)
      kv.second.pPlugin->DestroyModule(&kv.second.pModule);
    delete kv.second.pPlugin;
  }

  glfwDestroyWindow(m_pWindow);
  glfwTerminate();
}

void App::MakeDirty()
{
  m_projectDirty = m_geometryDirty = true;
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
      if (ImGui::MenuItem("Add model..."))
        PushModal(new Modal_AddModel());
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Modules"))
    {
      for (auto &kv : m_registeredModules)
      {
        if (ImGui::MenuItem(kv.second.pPlugin->GetModuleName().c_str(), nullptr, kv.second.pModule != nullptr))
          OpenModule(kv.first, &kv.second);
      }

      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}

void App::ShowControlWindow()
{
  ImGui::Begin("Control", nullptr, ImGuiWindowFlags_MenuBar);

  ShowMenuBar();

  ImGui::Text("%s", std::filesystem::path(m_saveFile).stem().string().c_str());

  ImGui::Separator();
  if (m_pCurrentProject->sceneObjects.objectList.size() != 0)
  {
    ImGui::Combo("Selected geometry", &m_pCurrentProject->currentFocus, (char *)&m_pCurrentProject->sceneObjects.ToImGuiNameString()[0]);

    SceneObject *pObj = &m_pCurrentProject->sceneObjects.objectList[m_pCurrentProject->currentFocus];

    if (ImGui::DragFloat2("Scale", &pObj->transform.scale.x(), 0.01f, 0.001f, 100.f))
      MakeDirty();

    if (ImGui::DragFloat2("Position", &pObj->transform.position.x()))
      MakeDirty();

    float rotation = pObj->transform.rotation / Dg::Constants<float>::PI * 180.f;
    if (ImGui::SliderFloat("Rotation", &rotation, 0.f, 360.f, "%0.2f deg"))
      MakeDirty();
    pObj->transform.rotation = rotation * Dg::Constants<float>::PI / 180.f;

    if (ImGui::Button("Remove model"))
    {
      if (m_pCurrentProject->currentFocus == 0)
      {
        PushModal(new Modal_Alert("Cannot remove the boundary!"));
      }
      else
      {
        MakeDirty();
        m_pCurrentProject->RemoveCurrentFocus();
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
  if (pData->pModule == nullptr)
  {
    xn::ModuleInitData data{};
    data.ID = id;
    data.pLogger = GetLogger();
    data.pMemMngr = &m_memMngr;
    data.pMsgBus = &m_msgBus;
    data.name = pData->pPlugin->GetModuleName();

    pData->pModule = pData->pPlugin->CreateModule(&data);
    pData->pModule->SetGeometry(m_sanitisedGeom);
  }
}

void App::HandleMessages()
{
  xn::Message *pMsg = m_msgBus.PopMessage();
  while (pMsg != nullptr)
  {
    switch (pMsg->GetType())
    {
      xnDISPATCH(WindowClosed);
      xnDISPATCH(WindowGainedFocus);
      xnDISPATCH(WindowLostFocus);
      DISPATCH(MouseScroll);
      DISPATCH(ZoomCamera);
      DISPATCH(MoveCamera);
      DISPATCH_TO_FOCUS_MODULE(MouseDown);
      DISPATCH_TO_FOCUS_MODULE(MouseMove);
      DISPATCH_TO_FOCUS_MODULE(MouseUp);
    default:
      LOG_DEBUG("Message not handled: '%s'", pMsg->ToString().c_str());
    }

    delete pMsg;
    pMsg = m_msgBus.PopMessage();
  }
}

void App::Render()
{
  // TODO Set window size
  m_pCanvas->BeginFrame();

  xn::Renderer *pRenderer = m_pCanvas->GetRenderer();
  pRenderer->BeginDraw();

  m_camera.T_World_View = m_camera.T_Camera_World.ToMatrix33().GetInverse() * m_pCanvas->Get_T_Camera_View();
  int index = -1;
  for (auto const &obj : m_pCurrentProject->sceneObjects.objectList)
  {
    index++;
    xn::LineProperties opts = obj.valid ? DefaultData::data.validPolygon : DefaultData::data.invalidPolygon;

    if (index == m_pCurrentProject->currentFocus)
      opts = MakeFocus(opts);
    obj.geometry.Render(pRenderer, m_camera.T_World_View, opts, obj.transform);
  }

  for (auto &kv : m_registeredModules)
  {
    if (kv.second.pModule == nullptr)
      continue;

    kv.second.pModule->Render(pRenderer, m_camera.T_World_View);
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

void App::HandleMessage(xn::Message_WindowClosed *pMsg)
{
  try
  {
    ModuleData &data = m_registeredModules.at(pMsg->windowID);

    if (m_moduleFocusID == pMsg->windowID)
      m_moduleFocusID = INVALID_ID;

    data.pPlugin->DestroyModule(&data.pModule);
  }
  catch (std::exception e)
  {
    LOG_ERROR("Exception thrown when trying to close a module window: '%s'", e.what());
  }
}

void App::HandleMessage(xn::Message_WindowGainedFocus *pMsg)
{
  m_moduleFocusID = pMsg->windowID;
}

void App::HandleMessage(xn::Message_WindowLostFocus *pMsg)
{
  if (m_moduleFocusID == pMsg->windowID)
    m_moduleFocusID = INVALID_ID;
}

void App::HandleMessage(Message_MouseScroll *pMsg)
{
  m_pCanvas->Handle(pMsg);
}

void App::HandleMessage(Message_ZoomCamera *pMsg)
{
  m_camera.T_Camera_World.scale *= pMsg->val;
}

void App::HandleMessage(Message_MoveCamera *pMsg)
{
  m_camera.T_Camera_World.position.x() -= pMsg->v.x() * m_camera.T_Camera_World.scale.x();
  m_camera.T_Camera_World.position.y() += pMsg->v.y() * m_camera.T_Camera_World.scale.y();
}

void App::DispatchToFocus(xn::Message *pMsg)
{
  auto it = m_registeredModules.find(m_moduleFocusID);
  if (it != m_registeredModules.end())
  {
    xn::Module *pModule = it->second.pModule;
    if (pModule != nullptr)
      pModule->Handle(pMsg);
  }
}

void App::DispatchToAllModules(xn::Message *pMsg)
{
  for (auto &kv : m_registeredModules)
  {
    if (pMsg->QueryFlag(xn::Message::Flag::Handled))
      break;

    if (kv.second.pModule != nullptr)
      kv.second.pModule->Handle(pMsg);
  }
}

void App::SaveProject()
{
  if ((m_saveFile.size() < 5) || (m_saveFile.substr(m_saveFile.size() - 5, 5) != ".json"))
    m_saveFile += std::string(".json");

  if (!m_pCurrentProject->Write(m_saveFile))
  {
    m_projectDirty = false;
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

  Project *pTempProject = m_pCurrentProject;
  m_pCurrentProject = pNewProject;
  delete pTempProject;
  m_saveFile = filePath;
  m_geometryDirty = true;
  m_projectDirty = false;
}

void App::NewProject(std::string const &boundaryFile)
{
  xn::PolygonGroup geom;
  if (!geom.ReadFromOBJ(boundaryFile))
  {
    LOG_ERROR("Failed to read boundary file '%s' while creating new project. New project aborted", boundaryFile.c_str());
    return;
  }

  delete m_pCurrentProject;
  m_pCurrentProject = new Project();

  SceneObject obj;
  obj.geometry = geom;
  obj.name = DefaultData::data.defaultBoundaryName;

  m_pCurrentProject->sceneObjects.objectList.push_back(obj);
  m_pCurrentProject->currentFocus = 0;
  m_pCurrentProject->CompleteLoad();
  m_geometryDirty = true;
  m_projectDirty = true;
  m_saveFile.clear();
}

void App::SetSaveFile(std::string const &newFilePath)
{
  m_saveFile = newFilePath;
}

void App::AddModelFromFile(std::string const &filePath, std::string const &name)
{
  if (m_pCurrentProject->AddNewObject(filePath, name))
    MakeDirty();
}

void App::PushModal(Modal *pModal)
{
  m_modalStack.push_back(std::shared_ptr<Modal>(pModal));
}