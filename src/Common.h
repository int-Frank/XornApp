#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <string>

#include "xnGeometry.h"
#include "imgui.h"

enum HoverState
{
  HS_None = 0,
  HS_Hover = 1,
  HS_Acitve = 2,
  HS_COUNT
};

//--------------------------------------------------------------------------------
// Helper functions
//--------------------------------------------------------------------------------

bool FileExists(std::string const &);
std::vector<std::string> GetFilesFromFolder(std::string const &path, std::string const &ext = "");
std::vector<std::string> GetDirectoriesFromFolder(std::string const &path);

std::vector<std::string> GetProjectList();
std::vector<std::string> GetOBJList();

ImVec2 ToImGui(xn::vec2 const& v);

#endif