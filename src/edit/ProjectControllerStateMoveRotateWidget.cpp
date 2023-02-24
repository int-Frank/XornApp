
#include "ProjectControllerState.h"
#include "IRotateWidget.h"
#include "Input.h"

ProjectControllerStateMoveRotateWidget::ProjectControllerStateMoveRotateWidget(ProjectControllerStateData *pState, xn::vec2 const &mousePosition)
  : ProjectControllerState(pState)
  , m_offset()
{
  m_offset = mousePosition - pState->sceneState.pRotate->GetPosition();
}

ProjectControllerState *ProjectControllerStateMoveRotateWidget::MouseMove(uint32_t modState, xn::vec2 const &mouse)
{
  xn::vec2 screenPosition = mouse - m_offset;
  m_pStateData->sceneState.rotateWorldPosition = Multiply(screenPosition, m_pStateData->T_Screen_World);
  SetRotateWidget();
  return nullptr;
}

ProjectControllerState *ProjectControllerStateMoveRotateWidget::MouseUp(uint32_t modState, xn::vec2 const &)
{
  return new ProjectControllerStateIdle(m_pStateData);
}