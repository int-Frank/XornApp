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
  , validPolygon(0xFF00FFFF, 2.f)
  , invalidPolygon(0xFF0000FF, 2.f)
  , selectedItem(0xFFFFFFFF, 4.f)
{
  defaultBoundary.PushBack(xn::vec2(50.f, 50.f));
  defaultBoundary.PushBack(xn::vec2(-50.f, 50.f));
  defaultBoundary.PushBack(xn::vec2(-50.f, -50.f));
  defaultBoundary.PushBack(xn::vec2(50.f, -50.f));
}