
#include <filesystem>

#include "Modal.h"
#include "imgui.h"
#include "App.h"
#include "DefaultData.h"
#include "Common.h"

Modal::Modal(std::string const &name)
  : m_name(name)
  , m_first(true)
  , m_stayOpen(true)
{

}

bool Modal::Show(App *pApp)
{
  if (m_first)
  {
    ImGui::OpenPopup(m_name.c_str());
    m_first = false;
  }

  if (ImGui::BeginPopupModal(m_name.c_str(), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove))
  {
    _Show(pApp);
    ImGui::EndPopup();
  }
  return m_stayOpen;
}

void Modal::Close()
{
  m_stayOpen = false;
  ImGui::CloseCurrentPopup();
}

Modal_SavePrompt::Modal_SavePrompt()
  : Modal("Save current project?##Modal_SavePrompt")
{

}

void Modal_SavePrompt::_Show(App *pApp)
{
  if (ImGui::Button("Yes##Modal_SavePrompt", DefaultData::data.buttonSize))
  {
    pApp->PushModal(new Modal_SaveAs());
    Close();
  }
  ImGui::SameLine();
  if (ImGui::Button("No##Modal_SavePrompt", DefaultData::data.buttonSize))
    Close();
}

Modal_SaveAs::Modal_SaveAs()
  : Modal("Save as...##Modal_SaveAs")
{

}

void Modal_SaveAs::_Show(App *pApp)
{
  std::string saveFilePath;
  std::vector<std::string> files = GetProjectList();

  std::vector<char const *> pointers;
  for (std::string const &str : files)
    pointers.push_back(str.c_str());

  static int s_index = 0;
  const size_t bufSize = 256;
  static char buf[bufSize] = {};
  if (ImGui::ListBox("Project files##Modal_SaveAs", &s_index, pointers.data(), (int)pointers.size()))
    strcpy_s(buf, pointers[s_index]);

  ImGui::InputText("Project name##Modal_SaveAs", buf, bufSize);

  if (ImGui::Button("Save##Modal_SaveAs", DefaultData::data.buttonSize))
  {
    saveFilePath = std::string(DefaultData::data.projectsPath) + std::string(buf);
    if (FileExists(saveFilePath))
    {
      pApp->PushModal(new Modal_Overwrite(saveFilePath));
    }
    else
    {
      pApp->SetSaveFile(saveFilePath);
      pApp->SaveProject();
    }
    Close();
  }

  ImGui::SameLine();
  if (ImGui::Button("Cancel##Modal_SaveAs", DefaultData::data.buttonSize))
    Close();
}

Modal_Overwrite::Modal_Overwrite(std::string const &filePath)
  : Modal("Overwrite?##Modal_Overwrite")
  , m_filePath(filePath)
{

}

void Modal_Overwrite::_Show(App *pApp)
{
  if (ImGui::Button("Ok##Modal_Overwrite", DefaultData::data.buttonSize))
  {
    pApp->SetSaveFile(m_filePath);
    pApp->SaveProject();
    Close();
  }

  ImGui::SameLine();
  if (ImGui::Button("No##Modal_Overwrite", DefaultData::data.buttonSize))
  {
    pApp->PushModal(new Modal_SaveAs());
    Close();
  }
}

Modal_Open::Modal_Open()
  : Modal("Open##Modal_Open")
{

}

void Modal_Open::_Show(App *pApp)
{
  std::vector<std::string> files = GetProjectList();

  std::vector<char const *> pointers;
  for (std::string const &str : files)
    pointers.push_back(str.c_str());

  static int s_index = 0;
  ImGui::ListBox("Project files", &s_index, pointers.data(), (int)pointers.size());

  if (ImGui::Button("Open##Modal_Open", DefaultData::data.buttonSize))
  {
    if (files.size() > 0)
    {
      pApp->OpenProject(std::string(DefaultData::data.projectsPath) + files[s_index]);
      Close();
    }
  }

  ImGui::SameLine();
  if (ImGui::Button("Cancel##Modal_Open", DefaultData::data.buttonSize))
    Close();
}

Modal_AddModel::Modal_AddModel()
  : Modal("Add model##Modal_AddModel")
{

}

void Modal_AddModel::_Show(App *pApp)
{
  std::vector<std::string> filesFromAssets = GetModelListFromAssets();

  std::vector<char const *> pointers;
  for (std::string const &str : filesFromAssets)
    pointers.push_back(str.c_str());

  static int s_index = 0;
  static const size_t bufSize = 128;
  static char buf[bufSize] = {};
  ImGui::ListBox("Model files##Modal_AddModel", &s_index, pointers.data(), (int)pointers.size());

  static bool s_useCustomName = false;
  if (!s_useCustomName)
    strcpy_s(buf, std::filesystem::path(pointers[s_index]).stem().string().c_str());

  if (ImGui::InputText("Name", buf, bufSize))
    s_useCustomName = true;

  if (ImGui::Button("Open##Modal_AddModel", DefaultData::data.buttonSize))
  {
    if (pointers.size() > 0)
    {
      pApp->AddModelFromFile(std::string(DefaultData::data.modelsPath) + filesFromAssets[s_index], std::string(buf));
      Close();
    }

    s_useCustomName = false;
    s_index = 0;
  }

  ImGui::SameLine();
  if (ImGui::Button("Cancel##Modal_AddModel", DefaultData::data.buttonSize))
  {
    Close();
    s_useCustomName = false;
  }
}

Modal_NewProject::Modal_NewProject()
  : Modal("New project##Modal_Open")
{

}

void Modal_NewProject::_Show(App *pApp)
{
  std::vector<std::string> files = GetModelListFromAssets();

  std::vector<char const *> pointers;
  for (std::string const &str : files)
    pointers.push_back(str.c_str());

  static int s_index = 0;
  ImGui::ListBox("Choose a boundary", &s_index, pointers.data(), (int)pointers.size());

  if (ImGui::Button("Create##Modal_NewProject", DefaultData::data.buttonSize))
  {
    if (files.size() > 0)
    {
      pApp->NewProject(std::string(DefaultData::data.modelsPath) + files[s_index]);
      Close();
    }
  }

  ImGui::SameLine();
  if (ImGui::Button("Cancel##Modal_NewProject", DefaultData::data.buttonSize))
    Close();
}

Modal_Export::Modal_Export()
  : Modal("Export##Modal_Export")
{

}

void Modal_Export::_Show(App *pApp)
{
  if (ImGui::Button("Cancel##Modal_Export", DefaultData::data.buttonSize))
    Close();
}

Modal_Alert::Modal_Alert(std::string const &msg)
  : Modal("Alert!##Modal_Alert")
  , m_msg(msg)
{

}

void Modal_Alert::_Show(App *pApp)
{
  for (int i = 0; i < 4; i++)
    ImGui::Spacing();

  ImGui::Text("%s", m_msg.c_str());

  for (int i = 0; i < 5; i++)
    ImGui::Spacing();

  if (ImGui::Button("Close##Modal_Alert", DefaultData::data.buttonSize))
    Close();
}