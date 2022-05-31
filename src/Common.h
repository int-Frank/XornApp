#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <string>

#include "a2dGeometry.h"
#include "imgui.h"

//--------------------------------------------------------------------------------
// Helper functions
//--------------------------------------------------------------------------------

bool FileExists(std::string const &);
std::vector<std::string> GetFilesFromFolder(std::string const &path, std::string const &ext = "");
std::vector<std::string> GetDirectoriesFromFolder(std::string const &path);

std::vector<std::string> GetModelListFromAssets();
std::vector<std::string> GetProjectList();

ImVec2 ToImGui(a2d::vec2 const& v);

//--------------------------------------------------------------------------------
// Classes
//--------------------------------------------------------------------------------

struct GlobalData
{
  float scroll;
};

extern GlobalData g_globalData;



#endif