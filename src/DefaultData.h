#ifndef DEFAULTDATA_H
#define DEFAULTDATA_H

#include <string>
#include "Common.h"
#include "xnLineProperties.h"

struct DefaultData
{
  DefaultData();
  static DefaultData const data;

  float windowWidth;
  float windowHeight;

  std::string projectsPath;
  std::string modelsPath;
  std::string pluginsPath;

  xn::PolygonGroup defaultBoundary;
  std::string defaultBoundaryName;

  ImVec2 buttonSize;
  xn::LineProperties validPolygon;
  xn::LineProperties invalidPolygon;
  xn::LineProperties focusPolygon;
};

#endif