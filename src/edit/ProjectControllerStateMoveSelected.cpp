
#include "ProjectControllerState.h"
#include "Input.h"
#include "DefaultData.h"
#include "XornActions.h"

ProjectControllerStateMoveSelected::ProjectControllerStateMoveSelected(ProjectControllerStateData *pState, xn::vec2 const &mouseAnchor)
  : ProjectControllerState(pState)
  , m_actionID(INVALID_ACTION_ID)
  , m_mouseAnchor(mouseAnchor)
{

}

ProjectControllerState *ProjectControllerStateMoveSelected::MouseMove(uint32_t modState, xn::vec2 const &mouse)
{
  if (m_actionID != INVALID_ACTION_ID)
  {
    auto topID = m_pStateData->pActions->TopID();
    if (topID != m_actionID)
      return new ProjectControllerStateIdle(m_pStateData);
    m_pStateData->pActions->Undo();
  }

  xn::vec2 vector = mouse - m_mouseAnchor;
  xn::vec3 vector3 = Dg::ToVector3(vector, 0.f);
  vector3 = vector3 * m_pStateData->T_Screen_World;
  vector = Dg::ToVector2(vector3);

  ActionGroup *pActionGroup = new ActionGroup();
  ActionData actionData(m_pStateData->pProject);

  for (auto it = m_pStateData->sceneState.selectedPolygons.cbegin_rand(); it != m_pStateData->sceneState.selectedPolygons.cend_rand(); it++)
  {
    xn::mat33 oldTransform = m_pStateData->pProject->loops.Get(*it)->T_Model_World;

    xn::mat33 translation;
    translation.Translation(vector);
    xn::mat33 newTransform = oldTransform * translation;

    Action_TransformPolygon *pAction = new Action_TransformPolygon(actionData , *it, oldTransform, newTransform);
    pActionGroup->AddAction(pAction);
  }

  m_actionID = m_pStateData->pActions->AddAndExecute(pActionGroup);

  return nullptr;
}

ProjectControllerState *ProjectControllerStateMoveSelected::MouseUp(uint32_t modState, xn::vec2 const &)
{
  return new ProjectControllerStateIdle(m_pStateData);
}