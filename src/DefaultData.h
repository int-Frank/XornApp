#ifndef DEFAULTDATA_H
#define DEFAULTDATA_H

#include <string>
#include "Common.h"
#include "xnDraw.h"

struct DefaultData
{
  DefaultData();
  static DefaultData const data;

  float windowWidth;
  float windowHeight;

  std::string projectsPath;
  std::string pluginsPath;

  xn::PolygonLoop defaultBoundary;
  std::string defaultProjectName;

  ImVec2 buttonSize;
  xn::Draw::Stroke validPolygon;
  xn::Draw::Stroke invalidPolygon;
  xn::Draw::Stroke selectedItem;
};

#endif