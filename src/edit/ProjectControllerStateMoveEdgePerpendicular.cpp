
#include "ProjectControllerState.h"
#include "IRotateWidget.h"
#include "Input.h"
#include "DefaultData.h"
#include "DgQueryPointSegment.h"
#include "ActionList.h"
#include "XornActions.h"
#include "DgQueryPointLine.h"

ProjectControllerStateMoveEdgePerpendicular::ProjectControllerStateMoveEdgePerpendicular(ProjectControllerStateData* pState, xn::vec2 const& mousePosition, PolygonID polygonID, uint32_t vertexIndex)
  : ProjectControllerState(pState)
  , m_offset()
  , m_actionID(INVALID_ACTION_ID)
  , m_polygonID(polygonID)
  , m_index(vertexIndex)
{
  auto pLoop = m_pStateData->pProject->loops.Get(m_polygonID);
  xn::vec2 originalPoint = m_pStateData->pProject->loops.Get(polygonID)->vertices[vertexIndex];
  xn::vec2 screenPoint = Multiply(originalPoint, pLoop->T_Model_World * m_pStateData->T_World_Screen);
  m_offset = mousePosition - screenPoint;
}

ProjectControllerState* ProjectControllerStateMoveEdgePerpendicular::MouseMove(uint32_t modState, xn::vec2 const& mouse)
{
  if (m_actionID != INVALID_ACTION_ID)
  {
    auto topID = m_pStateData->pActions->TopID();
    if (topID != m_actionID)
      return new ProjectControllerStateIdle(m_pStateData);
    m_pStateData->pActions->Undo();
  }

  auto pLoop = m_pStateData->pProject->loops.Get(m_polygonID);

  uint32_t i0 = m_index;
  uint32_t i1 = (m_index + 1) % pLoop->GetLoop().Size();

  xn::vec2 p0 = pLoop->vertices[i0];
  xn::vec2 p1 = pLoop->vertices[i1];

  xn::vec2 v = p1 - p0;

  float len = Dg::Mag(v);
  if (len < Dg::Constants<float>::EPSILON)
  {
    return nullptr;
  }

  v = v / len;
  xn::vec2 u = xn::vec2(-v.y(), v.x());
  
  xn::mat33 m = xn::mat33(pLoop->T_Model_World * m_pStateData->T_World_Screen).GetInverse();
  xn::vec2 point = Multiply(mouse - m_offset, m);

  Dg::Line2<float> segmentLine(p0, v);
  Dg::CP2PointLine<float> query;
  xn::vec2 cp = query(point, segmentLine).cp;

  xn::vec2 w = point - cp;

  ActionGroup* pActionGroup = new ActionGroup();
  ActionData actionData(m_pStateData->pProject);

  auto pAction0 = new Action_MoveVertex(actionData, m_polygonID, i0, p0, p0 + w);
  auto pAction1 = new Action_MoveVertex(actionData, m_polygonID, i1, p1, p1 + w);
  pActionGroup->AddAction(pAction0);
  pActionGroup->AddAction(pAction1);

  m_actionID = m_pStateData->pActions->AddAndExecute(pActionGroup);

  return nullptr;
}

ProjectControllerState* ProjectControllerStateMoveEdgePerpendicular::MouseUp(uint32_t modState, xn::vec2 const&)
{
  return new ProjectControllerStateIdle(m_pStateData);
}