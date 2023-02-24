
#include "IRotateWidget.h"
#include "ProjectControllerState.h"
#include "XornActions.h"

//------------------------------------------------------------------------
// ProjectControllerStateRotateSelected
//------------------------------------------------------------------------

ProjectControllerStateRotate::ProjectControllerStateRotate(ProjectControllerStateData *pState)
  : ProjectControllerState(pState)
  , m_actionID(INVALID_ACTION_ID)
{

}

ProjectControllerState *ProjectControllerStateRotate::MouseMove(uint32_t modState, xn::vec2 const &mouse)
{
  if (m_actionID != INVALID_ACTION_ID)
  {
    auto topID = m_pStateData->pActions->TopID();
    if (topID != m_actionID)
      return new ProjectControllerStateIdle(m_pStateData);
    m_pStateData->pActions->Undo();
  }

  m_pStateData->sceneState.pRotate->SetMouse(mouse);
  float angle = m_pStateData->sceneState.pRotate->Angle();

  xn::mat33 t0, t1, r;
  t0.Translation(m_pStateData->sceneState.rotateWorldPosition);
  t1.Translation(-m_pStateData->sceneState.rotateWorldPosition);
  r.Rotation(-angle);

  ActionGroup *pActionGroup = new ActionGroup();
  ActionData actionData(m_pStateData->pProject);

  for (auto it = m_pStateData->sceneState.selectedPolygons.cbegin_rand(); it != m_pStateData->sceneState.selectedPolygons.cend_rand(); it++)
  {
    xn::mat33 oldTransform = m_pStateData->pProject->loops.Get(*it)->T_Model_World;
    xn::mat33 newTransform = oldTransform * t1 * r * t0;
    Action_TransformPolygon *pAction = new Action_TransformPolygon(actionData, *it, oldTransform, newTransform);
    pActionGroup->AddAction(pAction);
  }

  m_actionID = m_pStateData->pActions->AddAndExecute(pActionGroup);

  return nullptr;
}

ProjectControllerState *ProjectControllerStateRotate::MouseUp(uint32_t modState, xn::vec2 const &mouse)
{
  m_pStateData->sceneState.pRotate->Unclick(mouse);
  return new ProjectControllerStateIdle(m_pStateData);
}