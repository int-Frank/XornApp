
#include "ProjectControllerState.h"
#include "IRotateWidget.h"
#include "Input.h"
#include "DefaultData.h"
#include "DgQueryPointSegment.h"
#include "ActionList.h"
#include "XornActions.h"

ProjectControllerStateMoveVertex::ProjectControllerStateMoveVertex(ProjectControllerStateData *pState, xn::vec2 const &mousePosition, PolygonID polygonID, uint32_t vertexIndex)
  : ProjectControllerState(pState)
  , m_offset()
  , m_originalPoint(pState->pProject->loops.Get(polygonID)->vertices[vertexIndex])
  , m_actionID(INVALID_ACTION_ID)
  , m_polygonID(polygonID)
  , m_index(vertexIndex)
  , m_first(true)
{
  xn::vec2 screenPoint = m_originalPoint;
  xn::vec3 screenPoint3 = Dg::ToVector3(screenPoint, 1.f);
  auto pLoop = m_pStateData->pProject->loops.Get(m_polygonID);
  xn::mat33 m = pLoop->T_Model_World.ToMatrix33() * m_pStateData->T_World_Screen;
  screenPoint3 = screenPoint3 * m;
  screenPoint = Dg::ToVector2(screenPoint3);

  m_offset = mousePosition - screenPoint;
}

ProjectControllerState *ProjectControllerStateMoveVertex::MouseMove(xn::vec2 const &mouse)
{
  if (!m_first)
  {
    auto topID = m_pStateData->pActions->TopID();
    if (topID != m_actionID)
      return new ProjectControllerStateIdle(m_pStateData);
    m_pStateData->pActions->Undo();
  }
  m_first = false;

  xn::vec2 point = mouse - m_offset;
  xn::vec3 point3 = Dg::ToVector3(point, 1.f);
  auto pLoop = m_pStateData->pProject->loops.Get(m_polygonID);
  xn::mat33 m = pLoop->T_Model_World.ToMatrix33() * m_pStateData->T_World_Screen;
  point3 = point3 * m.GetInverse();
  point = Dg::ToVector2(point3);

  ActionData actionData(m_pStateData->pProject);
  auto pAction = new Action_MoveVertex(actionData, m_polygonID, m_index, m_originalPoint, point);
  m_actionID = m_pStateData->pActions->AddAndExecute(pAction);

  return nullptr;
}

ProjectControllerState *ProjectControllerStateMoveVertex::MouseUp(uint32_t modState, xn::vec2 const &)
{
  return new ProjectControllerStateIdle(m_pStateData);
}