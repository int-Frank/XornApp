#ifndef DEFAULTDATA_H
#define DEFAULTDATA_H

#include <vector>
#include <string>
#include "Common.h"
#include "xnColour.h"

struct RenderData
{
  struct PolygonAspect
  {
    xn::Colour colour;
    float thickness;
  };

  struct VertexAspect
  {
    xn::Colour colour;
    float radius;
  };

  struct RotateWidgetAspect
  {
    xn::Colour colour;
    float anchorRadius;
    float buttonRadius;
    float lineThickness;
  };

  PolygonAspect polygonAspect[HS_COUNT];
  VertexAspect vertexAspect;
  VertexAspect splitVertexAspect;
  RotateWidgetAspect rotateWidgetAspect[HS_COUNT];
};

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

  RenderData renderData;
};

#endif