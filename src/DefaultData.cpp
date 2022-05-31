#include "DefaultData.h"

DefaultData const DefaultData::data;

DefaultData::DefaultData()
  : windowWidth(1280)
  , windowHeight(720)
  , projectsPath("./projects/")
  , modelsPath("./assets/models/")
  , pluginsPath("./plugins/")
  , defaultBoundary()
  , defaultBoundaryName("Boundary")
  , buttonSize(80.f, 24.f)
  , validPolygon(0xFF00FFFF, 2.f)
  , invalidPolygon(0xFF0000FF, 2.f)
  , focusPolygon(0xFFFFFFFF, 4.f)
{
  xn::Polygon poly;
  poly.PushBack(xn::vec2(50.f, 50.f));
  poly.PushBack(xn::vec2(-50.f, 50.f));
  poly.PushBack(xn::vec2(-50.f, -50.f));
  poly.PushBack(xn::vec2(50.f, -50.f));

  defaultBoundary.polygons.push_back(poly);
}