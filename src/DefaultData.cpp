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
  , renderData{}
{
  renderData.polygonAspect[HS_None].colour = 0xFF00FFFF;
  renderData.polygonAspect[HS_None].thickness = 1.5f;
  renderData.polygonAspect[HS_Hover].colour = 0xFF88FFFF;
  renderData.polygonAspect[HS_Hover].thickness = 2.2f;
  renderData.polygonAspect[HS_Acitve].colour = 0xFFFFFFFF;
  renderData.polygonAspect[HS_Acitve].thickness = 2.2f;

  renderData.vertexAspect.colour = 0xFFF2B629;
  renderData.vertexAspect.radius = 14.f;

  renderData.splitVertexAspect.colour = 0xAAF2B629;
  renderData.splitVertexAspect.radius = 13.f;

  renderData.rotateWidgetAspect[HS_None].colour = 0xFF00FFFF;
  renderData.rotateWidgetAspect[HS_None].anchorRadius = 3.f;
  renderData.rotateWidgetAspect[HS_None].buttonRadius = 20.f;
  renderData.rotateWidgetAspect[HS_None].lineThickness = 1.5f;

  renderData.rotateWidgetAspect[HS_Hover].colour = 0xFFFF00FF;
  renderData.rotateWidgetAspect[HS_Hover].anchorRadius = 3.f;
  renderData.rotateWidgetAspect[HS_Hover].buttonRadius = 25.f;
  renderData.rotateWidgetAspect[HS_Hover].lineThickness = 2.f;

  renderData.rotateWidgetAspect[HS_Acitve].colour = 0xFFFFFFFF;
  renderData.rotateWidgetAspect[HS_Acitve].anchorRadius = 3.f;
  renderData.rotateWidgetAspect[HS_Acitve].buttonRadius = 25.f;
  renderData.rotateWidgetAspect[HS_Acitve].lineThickness = 1.5f;

  defaultBoundary.push_back(xn::vec2(50.f, 50.f));
  defaultBoundary.push_back(xn::vec2(-50.f, 50.f));
  defaultBoundary.push_back(xn::vec2(-50.f, -50.f));
  defaultBoundary.push_back(xn::vec2(50.f, -50.f));
}