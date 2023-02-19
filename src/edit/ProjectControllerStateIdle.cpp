
#include "ProjectControllerState.h"
#include "IRotateWidget.h"
#include "Input.h"
#include "DefaultData.h"
#include "DgQueryPointSegment.h"
#include "ActionList.h"
#include "XornActions.h"

//------------------------------------------------------------------------
// ProjectControllerStateIdle
//------------------------------------------------------------------------

ProjectControllerStateIdle::ProjectControllerStateIdle(ProjectControllerStateData *pData)
  : ProjectControllerState(pData)
{

}

ProjectControllerState *ProjectControllerStateIdle::MouseMove(uint32_t modState, xn::vec2 const &mouse)
{
  m_pStateData->sceneState.hoverPolygon = PolygonUnderMouse(mouse);
  return nullptr;
}

ProjectControllerState *ProjectControllerStateIdle::MouseDown(uint32_t modState, xn::vec2 const &mouse)
{
  m_pStateData->sceneState.hoverPolygon = INVALID_POLYGON_ID;

  if (m_pStateData->sceneState.pRotate != nullptr && m_pStateData->sceneState.pRotate->MouseDown(mouse))
    return new ProjectControllerStateRotateSelected(m_pStateData);

  uint32_t activeVertex;
  PolygonID activeVertexPolygon;

  if (VertexUnderMouse(mouse, &activeVertexPolygon, &activeVertex))
    return new ProjectControllerStateMoveVertex(m_pStateData, mouse, activeVertexPolygon, activeVertex);

  m_pStateData->sceneState.hoverPolygon = PolygonUnderMouse(mouse);
  if (m_pStateData->sceneState.hoverPolygon != INVALID_POLYGON_ID)
  {
    bool selectedExists = m_pStateData->sceneState.selectedPolygons.exists(m_pStateData->sceneState.hoverPolygon);
    if ((modState & xn::MK_shift) != 0)
    {
      if (selectedExists)
        m_pStateData->sceneState.selectedPolygons.erase(m_pStateData->sceneState.hoverPolygon);
      else
        m_pStateData->sceneState.selectedPolygons.insert(m_pStateData->sceneState.hoverPolygon);
      return nullptr;
    }

    m_pStateData->sceneState.selectedPolygons.clear();
    m_pStateData->sceneState.selectedPolygons.insert(m_pStateData->sceneState.hoverPolygon);
    return new ProjectControllerStateMoveSelected(m_pStateData, mouse);
  }

  return new ProjectControllerStateMultiSelect(m_pStateData, mouse);
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

bool ProjectControllerStateIdle::VertexUnderMouse(xn::vec2 const &mouse, PolygonID *pPolygonID, uint32_t *pVertexIndex) const
{
  if (SplitVertexUnderMouse(mouse, pPolygonID, pVertexIndex))
    return true;

  for (auto loop_it = m_pStateData->pProject->loops.Begin(); loop_it != m_pStateData->pProject->loops.End(); loop_it++)
  {
    if (!m_pStateData->sceneState.selectedPolygons.exists(loop_it->first))
      continue;

    for (uint32_t i = 0; i < (uint32_t)loop_it->second.vertices.size(); i++)
    {
      auto vertex = loop_it->second.vertices[i];
      xn::vec3 vertex3(vertex.x(), vertex.y(), 1.f);
      vertex3 = vertex3 * (loop_it->second.T_Model_World.ToMatrix33() * m_pStateData->T_World_Screen);
      vertex = xn::vec2(vertex3.x(), vertex3.y());

      float dist = Dg::MagSq(vertex - mouse);
      float threshold = DefaultData::data.renderData.vertexAspect.radius;
      threshold *= threshold;
      if (dist < threshold)
      {
        *pPolygonID = loop_it->first;
        *pVertexIndex = i;
        return true;
      }
    }
  }

  return nullptr;
}

bool ProjectControllerStateIdle::SplitVertexUnderMouse(xn::vec2 const &mouse, PolygonID *pPolygonID, uint32_t *pVertexIndex) const
{
  for (auto loop_it = m_pStateData->pProject->loops.Begin(); loop_it != m_pStateData->pProject->loops.End(); loop_it++)
  {
    if (!m_pStateData->sceneState.selectedPolygons.exists(loop_it->first))
      continue;

    for (uint32_t i = 0; i < (uint32_t)loop_it->second.vertices.size(); i++)
    {
      auto vertex = loop_it->second.vertices[i];
      auto vNext = loop_it->second.vertices[(i + 1) % loop_it->second.vertices.size()];

      xn::vec2 centreWorld = (vertex + vNext) / 2.f;
      xn::vec3 centreScreen3(centreWorld.x(), centreWorld.y(), 1.f);
      centreScreen3 = centreScreen3 * (loop_it->second.T_Model_World.ToMatrix33() * m_pStateData->T_World_Screen);
      xn::vec2 centreScreen = xn::vec2(centreScreen3.x(), centreScreen3.y());

      float dist = Dg::MagSq(centreScreen - mouse);
      float threshold = DefaultData::data.renderData.splitVertexAspect.radius;
      threshold *= threshold;
      if (dist < threshold)
      {
        ActionData actionData(m_pStateData->pProject);

        auto index = (i + 1) % loop_it->second.vertices.size();
        auto pAction = new Action_AddVertex(actionData, loop_it->first, index, centreWorld);
        m_pStateData->pActions->AddAndExecute(pAction);
        *pPolygonID = loop_it->first;
        *pVertexIndex = index;
        return true;
      }
    }
  }

  return nullptr;
}

void ProjectControllerStateIdle::Undo() 
{
  m_pStateData->pActions->Undo();
}

void ProjectControllerStateIdle::Redo() 
{
  m_pStateData->pActions->Redo();
}

//------------------------------------------------------------------------
// ProjectControllerStateMultiSelect
//------------------------------------------------------------------------

ProjectControllerStateMultiSelect::ProjectControllerStateMultiSelect(ProjectControllerStateData *pState, xn::vec2 const &mouseAnchor)
  : ProjectControllerState(pState)
{

}

ProjectControllerState *ProjectControllerStateMultiSelect::MouseMove(uint32_t modState, xn::vec2 const &)
{
  return nullptr;
}

ProjectControllerState *ProjectControllerStateMultiSelect::MouseUp(uint32_t modState, xn::vec2 const &)
{
  return nullptr;
}

//------------------------------------------------------------------------
// ProjectControllerStateRotateSelected
//------------------------------------------------------------------------

ProjectControllerStateRotateSelected::ProjectControllerStateRotateSelected(ProjectControllerStateData *pState)
  : ProjectControllerState(pState)
{

}

ProjectControllerState *ProjectControllerStateRotateSelected::MouseMove(uint32_t modState, xn::vec2 const &)
{
  return nullptr;
}

ProjectControllerState *ProjectControllerStateRotateSelected::MouseUp(uint32_t modState, xn::vec2 const &)
{
  return nullptr;
}