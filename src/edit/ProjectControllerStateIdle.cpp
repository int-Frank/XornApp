
#include "ProjectControllerState.h"
#include "IRotateWidget.h"
#include "Input.h"
#include "DefaultData.h"
#include "DgQueryPointSegment.h"
#include "Logger.h"

//------------------------------------------------------------------------
// ProjectControllerStateIdle
//------------------------------------------------------------------------

ProjectControllerStateIdle::ProjectControllerStateIdle(ProjectControllerStateData *pData)
  : ProjectControllerState(pData)
  , m_hoverPolygon(INVALID_POLYGON_ID)
{

}

ProjectControllerState *ProjectControllerStateIdle::MouseMove(xn::vec2 const &mouse)
{
  m_hoverPolygon = PolygonUnderMouse(mouse);
  return nullptr;
}

ProjectControllerState *ProjectControllerStateIdle::MouseDown(ModKey mod, xn::vec2 const &mouse)
{
  if (m_pStateData->pRotate != nullptr && m_pStateData->pRotate->MouseDown(mouse))
    return new ProjectControllerStateRotateSelected(m_pStateData);

  xn::vec2 *pVertex = VertexUnderMouse(mouse);
  if (pVertex != nullptr)
    return new ProjectControllerStateMoveVertex(m_pStateData, mouse, pVertex);

  PolygonID polyID = PolygonUnderMouse(mouse);
  if (polyID != INVALID_POLYGON_ID)
  {
    bool selectedExists = m_pStateData->selectedPolygons.exists(polyID);
    if (mod == MK_shift)
    {
      if (selectedExists)
        m_pStateData->selectedPolygons.erase(polyID);
      else
        m_pStateData->selectedPolygons.insert(polyID);
      return nullptr;
    }

    m_pStateData->selectedPolygons.clear();
    m_pStateData->selectedPolygons.insert(polyID);
    return new ProjectControllerStateDragSelected(m_pStateData, mouse);
  }

  return new ProjectControllerStateMultiSelect(m_pStateData, mouse);
}

void ProjectControllerStateIdle::UpdateScene(xn::IScene *pScene)
{
  for (auto it = m_pStateData->pProject->loops.Begin(); it != m_pStateData->pProject->loops.End(); it++)
  {
    if (m_pStateData->selectedPolygons.exists(it->first))
    {
      pScene->AddPolygon(it->second.GetTransformed(),
        DefaultData::data.renderData.polygonAspect[HS_Acitve].thickness,
        DefaultData::data.renderData.polygonAspect[HS_Acitve].colour, 0, 0);
    }
    else if (it->first == m_hoverPolygon)
    {
      pScene->AddPolygon(it->second.GetTransformed(),
        DefaultData::data.renderData.polygonAspect[HS_Hover].thickness,
        DefaultData::data.renderData.polygonAspect[HS_Hover].colour, 0, 0);
    }
    else
    {
      pScene->AddPolygon(it->second.GetTransformed(),
        DefaultData::data.renderData.polygonAspect[HS_None].thickness,
        DefaultData::data.renderData.polygonAspect[HS_None].colour, 0, 0);
    }
  }
}

PolygonID ProjectControllerStateIdle::PolygonUnderMouse(xn::vec2 const &mouse) const
{
  for (auto loop_it = m_pStateData->pProject->loops.Begin(); loop_it != m_pStateData->pProject->loops.End(); loop_it++)
  {
    auto loop = loop_it->second.GetTransformed();
    for (auto edge = loop.cEdgesBegin(); edge != loop.cEdgesEnd(); edge++)
    {
      auto seg = edge.ToSegment();
      xn::vec3 p0_3(seg.GetP0().x(), seg.GetP0().y(), 1.f);
      xn::vec3 p1_3(seg.GetP1().x(), seg.GetP1().y(), 1.f);

      auto p0 = p0_3 * m_pStateData->T_World_Screen;
      auto p1 = p1_3 * m_pStateData->T_World_Screen;

      seg.Set(p0, p1);

      Dg::CP2PointSegment<float> query;
      auto result = query(mouse, seg);
      float dist = Dg::MagSq(result.cp - mouse);
      float threshold = DefaultData::data.renderData.polygonAspect[HS_Hover].thickness;
      threshold *= threshold;
      if (dist < threshold)
        return loop_it->first;
    }
  }

  return INVALID_POLYGON_ID;
}

xn::vec2 *ProjectControllerStateIdle::VertexUnderMouse(xn::vec2 const &mouse) const
{
  for (auto loop_it = m_pStateData->pProject->loops.Begin(); loop_it != m_pStateData->pProject->loops.End(); loop_it++)
  {
    if (!m_pStateData->selectedPolygons.exists(loop_it->first))
      continue;

    auto loop = loop_it->second.GetTransformed();
    for (auto vertex_it = loop.PointsBegin(); vertex_it != loop.PointsEnd(); vertex_it++)
    {
      xn::vec3 vertex3(vertex_it->x(), vertex_it->y(), 1.f);
      vertex3 = vertex3 * m_pStateData->T_World_Screen;
      xn::vec2 vertex = xn::vec2(vertex3.x(), vertex3.y());

      float dist = Dg::MagSq(vertex - mouse);
      float threshold = DefaultData::data.renderData.vertexAspect[HS_Hover].radius;
      threshold *= threshold;
      if (dist < threshold)
        return &(*vertex_it);
    }
  }

  return nullptr;
}

xn::vec2 *ProjectControllerStateIdle::SplitVertexUnderMouse(xn::vec2 const &mouse) const
{
  for (auto loop_it = m_pStateData->pProject->loops.Begin(); loop_it != m_pStateData->pProject->loops.End(); loop_it++)
  {
    if (!m_pStateData->selectedPolygons.exists(loop_it->first))
      continue;

    auto loop = loop_it->second.GetTransformed();
    for (auto vertex = loop.PointsBegin(); vertex != loop.PointsEnd(); vertex++)
    {
      auto vNext = vertex;
      vNext++;
      if (vNext == loop.PointsEnd())
        vNext = loop.PointsBegin();

      xn::vec2 centreWorld = (*vertex + *vNext) / 2.f;
      xn::vec3 centreScreen3(centreWorld.x(), centreWorld.y(), 1.f);
      centreScreen3 = centreScreen3 * m_pStateData->T_World_Screen;
      xn::vec2 centreScreen = xn::vec2(centreScreen3.x(), centreScreen3.y());

      float dist = Dg::MagSq(centreScreen - mouse);
      float threshold = DefaultData::data.renderData.splitVertexAspect[HS_Hover].radius;
      threshold *= threshold;
      if (dist < threshold)
      {
        auto newVertex = loop.Insert(vertex, centreWorld);
        return &(*newVertex);
      }
    }
  }

  return nullptr;
}


//------------------------------------------------------------------------
// ProjectControllerStateMultiSelect
//------------------------------------------------------------------------

ProjectControllerStateMultiSelect::ProjectControllerStateMultiSelect(ProjectControllerStateData *pState, xn::vec2 const &mouseAnchor)
  : ProjectControllerState(pState)
{

}

ProjectControllerState *ProjectControllerStateMultiSelect::MouseMove(xn::vec2 const &)
{
  return nullptr;
}

ProjectControllerState *ProjectControllerStateMultiSelect::MouseUp(ModKey, xn::vec2 const &)
{
  return nullptr;
}

//------------------------------------------------------------------------
// ProjectControllerStateDragSelected
//------------------------------------------------------------------------

ProjectControllerStateDragSelected::ProjectControllerStateDragSelected(ProjectControllerStateData *pState, xn::vec2 const &mouseAnchor)
  : ProjectControllerState(pState)
{

}

ProjectControllerState *ProjectControllerStateDragSelected::MouseMove(xn::vec2 const &)
{
  return nullptr;
}

ProjectControllerState *ProjectControllerStateDragSelected::MouseUp(ModKey, xn::vec2 const &)
{
  return new ProjectControllerStateIdle(m_pStateData);
}

//------------------------------------------------------------------------
// ProjectControllerStateMoveVertex
//------------------------------------------------------------------------

ProjectControllerStateMoveVertex::ProjectControllerStateMoveVertex(ProjectControllerStateData *pState, xn::vec2 const &mousePosition, xn::vec2 *pVertex)
  : ProjectControllerState(pState)
  , m_offset(*pVertex - mousePosition)
  , m_pVertex(pVertex)
{

}

ProjectControllerState *ProjectControllerStateMoveVertex::MouseMove(xn::vec2 const &)
{
  return nullptr;
}

ProjectControllerState *ProjectControllerStateMoveVertex::MouseUp(ModKey, xn::vec2 const &)
{
  return new ProjectControllerStateIdle(m_pStateData);
}

//------------------------------------------------------------------------
// ProjectControllerStateRotateSelected
//------------------------------------------------------------------------

ProjectControllerStateRotateSelected::ProjectControllerStateRotateSelected(ProjectControllerStateData *pState)
  : ProjectControllerState(pState)
{

}

ProjectControllerState *ProjectControllerStateRotateSelected::MouseMove(xn::vec2 const &)
{
  return nullptr;
}

ProjectControllerState *ProjectControllerStateRotateSelected::MouseUp(ModKey, xn::vec2 const &)
{
  return nullptr;
}