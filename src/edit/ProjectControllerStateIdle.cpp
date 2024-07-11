
#include "ProjectControllerState.h"
#include "IRotateWidget.h"
#include "Input.h"
#include "DefaultData.h"
#include "DgQueryPointSegment.h"
#include "ActionList.h"
#include "XornActions.h"
#include "Renderer.h"

ProjectControllerStateIdle::ProjectControllerStateIdle(ProjectControllerStateData *pData)
  : ProjectControllerState(pData)
  , m_hoverPolygon(INVALID_POLYGON_ID)
{

}

ProjectControllerState *ProjectControllerStateIdle::MouseMove(uint32_t modState, xn::vec2 const &mouse)
{
  if (m_pStateData->sceneState.pRotate != nullptr)
    m_pStateData->sceneState.pRotate->SetMouse(mouse);
  uint32_t unused = 0;
  m_hoverPolygon = PolygonUnderMouse(mouse, &unused);
  return nullptr;
}

ProjectControllerState *ProjectControllerStateIdle::MouseDown(uint32_t modState, xn::vec2 const &mouse)
{
  m_hoverPolygon = INVALID_POLYGON_ID;

  if (m_pStateData->sceneState.pRotate != nullptr)
  {
    auto button = m_pStateData->sceneState.pRotate->MouseDown(mouse);
    if (button == IRotateWidget::Button::Rotate)
      return new ProjectControllerStateRotate(m_pStateData);
    if (button == IRotateWidget::Button::Move)
      return new ProjectControllerStateMoveRotateWidget(m_pStateData, mouse);
  }

  uint32_t activeVertex;
  PolygonID activeVertexPolygon;

  if (VertexUnderMouse(mouse, &activeVertexPolygon, &activeVertex))
    return new ProjectControllerStateMoveVertex(m_pStateData, mouse, activeVertexPolygon, activeVertex);

  uint32_t vertexIndex = 0;
  m_hoverPolygon = PolygonUnderMouse(mouse, &vertexIndex);
  if (m_hoverPolygon != INVALID_POLYGON_ID)
  {
    bool selectedExists = m_pStateData->sceneState.selectedPolygons.exists(m_hoverPolygon);
    if (HAS_MOD_SHIFT(modState))
    {
      if (selectedExists)
        m_pStateData->sceneState.selectedPolygons.erase(m_hoverPolygon);
      else
        m_pStateData->sceneState.selectedPolygons.insert(m_hoverPolygon);
      return nullptr;
    }

    if (selectedExists)
    {
      if (HAS_MOD_CTRL(modState) && m_pStateData->sceneState.selectedPolygons.size() == 1)
      {
        auto pNextState = new ProjectControllerStateMoveEdgePerpendicular(m_pStateData, mouse, m_hoverPolygon, vertexIndex);
        return new ProjectControllerStateTransition(m_pStateData, pNextState, m_hoverPolygon);
      }

      auto pNextState = new ProjectControllerStateMoveSelected(m_pStateData, mouse);
      return new ProjectControllerStateTransition(m_pStateData, pNextState, m_hoverPolygon);
    }

    m_pStateData->sceneState.selectedPolygons.clear();
    m_pStateData->sceneState.selectedPolygons.insert(m_hoverPolygon);

    if (HAS_MOD_CTRL(modState))
    {
      auto pNextState = new ProjectControllerStateMoveEdgePerpendicular(m_pStateData, mouse, m_hoverPolygon, vertexIndex);
      return new ProjectControllerStateTransition(m_pStateData, pNextState, m_hoverPolygon);
    }

    return new ProjectControllerStateMoveSelected(m_pStateData, mouse);
  }

  return new ProjectControllerStateMultiSelect(m_pStateData, mouse);
}

PolygonID ProjectControllerStateIdle::PolygonUnderMouse(xn::vec2 const &mouse, uint32_t *pVertexIndex) const
{
  for (auto loop_it = m_pStateData->pProject->loops.Begin(); loop_it != m_pStateData->pProject->loops.End(); loop_it++)
  {
    auto loop = loop_it->second.GetTransformed();
    uint32_t vertexIndex = 0;
    for (auto edge = loop.cEdgesBegin(); edge != loop.cEdgesEnd(); edge++, vertexIndex++)
    {
      auto seg = edge.ToSegment();

      xn::vec2 p0 = Multiply(seg.GetP0(), m_pStateData->T_World_Screen);
      xn::vec2 p1 = Multiply(seg.GetP1(), m_pStateData->T_World_Screen);

      seg.Set(p0, p1);

      Dg::CP2PointSegment<float> query;
      auto result = query(mouse, seg);
      float dist = Dg::MagSq(result.cp - mouse);
      float threshold = DefaultData::data.renderData.polygonAspect[HS_Hover].thickness;
      threshold *= threshold;
      if (dist < threshold)
      {
        *pVertexIndex = vertexIndex;
        return loop_it->first;
      }
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
      xn::vec2 vertex = Multiply(loop_it->second.vertices[i], loop_it->second.T_Model_World * m_pStateData->T_World_Screen);

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

      xn::vec2 pointWorld = (vertex + vNext) / 2.f;
      xn::vec2 pointScreen = Multiply(pointWorld, loop_it->second.T_Model_World * m_pStateData->T_World_Screen);

      float dist = Dg::MagSq(pointScreen - mouse);
      float threshold = DefaultData::data.renderData.splitVertexAspect.radius;
      threshold *= threshold;
      if (dist < threshold)
      {
        ActionData actionData(m_pStateData->pProject);

        auto index = (i + 1) % loop_it->second.vertices.size();
        auto pAction = new Action_AddVertex(actionData, loop_it->first, index, pointWorld);
        m_pStateData->pActions->AddAndExecute(pAction);
        *pPolygonID = loop_it->first;
        *pVertexIndex = index;
        return true;
      }
    }
  }

  return nullptr;
}

void ProjectControllerStateIdle::Render(Renderer *pRenderer)
{
  if (m_hoverPolygon == INVALID_POLYGON_ID || m_pStateData->sceneState.selectedPolygons.exists(m_hoverPolygon))
    return;

  auto polygon = m_pStateData->pProject->loops.Get(m_hoverPolygon)->GetTransformed();
  pRenderer->DrawPolygon(polygon,
    DefaultData::data.renderData.polygonAspect[HS_Hover].thickness,
    DefaultData::data.renderData.polygonAspect[HS_Hover].colour, 0);
}

void ProjectControllerStateIdle::Undo() 
{
  m_pStateData->pActions->Undo();
}

void ProjectControllerStateIdle::Redo() 
{
  m_pStateData->pActions->Redo();
}