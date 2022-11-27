
#include <fstream>
#include <filesystem>
#include <sstream>

#include "DefaultData.h"
#include "DgMatrix33.h"
#include "Common.h"

bool FileExists(std::string const &filePath)
{
  std::ifstream f(filePath.c_str());
  return f.good();
}

ImVec2 ToImGui(xn::vec2 const& v)
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

std::vector<std::string> GetOBJList()
{
  return GetFilesFromFolder(DefaultData::data.projectsPath, ".obj");
}

uint32_t BuildPolygonID(uint32_t group, uint32_t index)
{
  return ((group & 0xFFFF) << 16) | (index & 0xFFFF);
}

uint32_t GetGroupFromID(uint32_t id)
{
  return id >> 16;
}

uint32_t GetIndexFromID(uint32_t id)
{
  return id & 0xFFFF;
}

static bool ReadPointsFromOBJ(std::string const &file, std::vector<xn::vec2> &out)
{
  std::ifstream ifs(file);
  if (!ifs.good())
    return false;

  std::string line;
  while (std::getline(ifs, line))
  {
    std::istringstream iss(line);
    std::string tag;
    iss >> tag;

    if (tag == "v")
    {
      xn::vec2 v;
      if (!(iss >> v.x() >> v.y()))
        return false;

      out.push_back(v);
    }
  }

  return true;
}

bool ReadPolygonFromOBJ(std::string const &file, xn::PolygonWithHoles *pOut)
{
  std::vector<xn::vec2> points;
  if (!ReadPointsFromOBJ(file, points))
    return false;

  std::ifstream ifs(file);
  if (!ifs.good())
    return false;

  std::string line;
  while (std::getline(ifs, line))
  {
    std::istringstream iss(line);
    std::string tag;
    iss >> tag;

    if (tag == "l")
    {
      xn::PolygonLoop polygon;
      int index;
      while (iss >> index)
        polygon.PushBack(points[index - 1]);
      pOut->Push(polygon);
    }
  }

  return true;
}