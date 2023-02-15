#ifndef DEFAULTDATA_H
#define DEFAULTDATA_H

#include <vector>
#include <string>
#include "Common.h"
#include "xnColour.h"

enum HoverState
{
  HS_None = 0,
  HS_Hover = 1,
  HS_Acitve = 2,
  HS_COUNT
};

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
  VertexAspect vertexAspect[HS_COUNT];
  VertexAspect splitVertexAspect[HS_COUNT];
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