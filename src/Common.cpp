
#include <fstream>
#include <filesystem>

#include "appDefaultData.h"
#include "DgMatrix33.h"
#include "appCommon.h"

GlobalData g_globalData = {};

bool FileExists(std::string const &filePath)
{
  std::ifstream f(filePath.c_str());
  return f.good();
}

ImVec2 ToImGui(a2d::vec2 const& v)
{
  return ImVec2(v.x(), v.y());
}

std::vector<std::string> GetFilesFromFolder(std::string const &path, std::string const &ext)
{
  std::vector<std::string> files;
  for (const auto &entry : std::filesystem::directory_iterator(path))
  {
    if (std::filesystem::is_directory(entry))
      continue;

    if (!ext.empty() && entry.path().extension() != ext)
      continue;

    files.push_back(entry.path().filename().string());
  }

  return files;
}

std::vector<std::string> GetDirectoriesFromFolder(std::string const &path)
{
  std::vector<std::string> files;
  for (const auto &entry : std::filesystem::directory_iterator(path))
  {
    if (std::filesystem::is_directory(entry))
      files.push_back(entry.path().filename().string());
  }

  return files;
}

std::vector<std::string> GetProjectList()
{
  return GetFilesFromFolder(DefaultData::data.projectsPath, ".json");
}

std::vector<std::string> GetModelListFromAssets()
{
  return GetFilesFromFolder(DefaultData::data.modelsPath, ".obj");
}