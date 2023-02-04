#include "DefaultData.h"

DefaultData const DefaultData::data;

DefaultData::DefaultData()
  : windowWidth(1280)
  , windowHeight(720)
  , projectsPath("./projects/")
  , pluginsPath("./plugins/")
  , defaultBoundary()
  , defaultProjectName("My project")
  , buttonSize(80.f, 24.f)
  , backgroundColour(0xFF323232)
  , polygonColour(0xFF00FFFF)
  , polygonThickness(2.f)
{
  defaultBoundary.push_back(xn::vec2(50.f, 50.f));
  defaultBoundary.push_back(xn::vec2(-50.f, 50.f));
  defaultBoundary.push_back(xn::vec2(-50.f, -50.f));
  defaultBoundary.push_back(xn::vec2(50.f, -50.f));
}