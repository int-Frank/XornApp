#ifndef DEFAULTDATA_H
#define DEFAULTDATA_H

#include <string>
#include "appCommon.h"
#include "a2dLineProperties.h"

struct DefaultData
{
  DefaultData();
  static DefaultData const data;

  float windowWidth;
  float windowHeight;

  std::string projectsPath;
  std::string modelsPath;
  std::string pluginsPath;

  a2d::Geometry defaultBoundary;
  std::string defaultBoundaryName;

  ImVec2 buttonSize;
  a2d::LineProperties validPolygon;
  a2d::LineProperties invalidPolygon;
  a2d::LineProperties focusPolygon;
};

#endif