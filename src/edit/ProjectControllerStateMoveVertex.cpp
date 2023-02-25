
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
  , m_actionID(INVALID_ACTION_ID)
  , m_polygonID(polygonID)
  , m_index(vertexIndex)
{
  auto pLoop = m_pStateData->pProject->loops.Get(m_polygonID);
  xn::vec2 originalPoint = m_pStateData->pProject->loops.Get(polygonID)->vertices[vertexIndex];
  xn::vec2 screenPoint = Multiply(originalPoint, pLoop->T_Model_World * m_pStateData->T_World_Screen);
  m_offset = mousePosition - screenPoint;
}

ProjectControllerState *ProjectControllerStateMoveVertex::MouseMove(uint32_t modState, xn::vec2 const &mouse)
{
  if (m_actionID != INVALID_ACTION_ID)
  {
    auto topID = m_pStateData->pActions->TopID();
    if (topID != m_actionID)
      return new ProjectControllerStateIdle(m_pStateData);
    m_pStateData->pActions->Undo();
  }

  auto pLoop = m_pStateData->pProject->loops.Get(m_polygonID);

  xn::mat33 m = xn::mat33(pLoop->T_Model_World * m_pStateData->T_World_Screen).GetInverse();
  xn::vec2 originalPoint = m_pStateData->pProject->loops.Get(m_polygonID)->vertices[m_index];
  xn::vec2 point = Multiply(mouse - m_offset, m);

  ActionData actionData(m_pStateData->pProject);
  auto pAction = new Action_MoveVertex(actionData, m_polygonID, m_index, originalPoint, point);
  m_actionID = m_pStateData->pActions->AddAndExecute(pAction);

  return nullptr;
}

ProjectControllerState *ProjectControllerStateMoveVertex::MouseUp(uint32_t modState, xn::vec2 const &)
{
  return new ProjectControllerStateIdle(m_pStateData);
}