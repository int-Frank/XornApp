#ifndef DEFAULTDATA_H
#define DEFAULTDATA_H

#include <vector>
#include <string>
#include "Common.h"
#include "xnColour.h"

struct DefaultData
{
  DefaultData();
  static DefaultData const data;

  float windowWidth;
  float windowHeight;

  std::string projectsPath;
  std::string pluginsPath;

  std::vector<xn::vec2> defaultBoundary;
  std::string defaultProjectName;

  ImVec2 buttonSize;
  xn::Colour backgroundColour;
  xn::Colour polygonColour;
  float polygonThickness;
};

#endif