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

  renderData.vertexAspect[HS_None].colour = 0xFF334455;
  renderData.vertexAspect[HS_None].radius = 5.f;
  renderData.vertexAspect[HS_Hover].colour = 0xFF995511;
  renderData.vertexAspect[HS_Hover].radius = 5.f;
  renderData.vertexAspect[HS_Acitve].colour = 0xFF0F0F0F;
  renderData.vertexAspect[HS_Acitve].radius = 5.f;

  renderData.splitVertexAspect[HS_None].colour = 0x44334455;
  renderData.splitVertexAspect[HS_None].radius = 4.f;
  renderData.splitVertexAspect[HS_Hover].colour = 0x44995511;
  renderData.splitVertexAspect[HS_Hover].radius = 4.f;
  renderData.splitVertexAspect[HS_Acitve].colour = 0x440F0F0F;
  renderData.splitVertexAspect[HS_Acitve].radius = 4.f;

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