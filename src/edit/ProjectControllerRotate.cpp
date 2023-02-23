
#include "IRotateWidget.h"
#include "ProjectControllerState.h"

//------------------------------------------------------------------------
// ProjectControllerStateRotateSelected
//------------------------------------------------------------------------

ProjectControllerStateRotate::ProjectControllerStateRotate(ProjectControllerStateData *pState)
  : ProjectControllerState(pState)
{

}

ProjectControllerState *ProjectControllerStateRotate::MouseMove(uint32_t modState, xn::vec2 const &mouse)
{
  m_pStateData->sceneState.pRotate->SetMouse(mouse);
  return nullptr;
}

ProjectControllerState *ProjectControllerStateRotate::MouseUp(uint32_t modState, xn::vec2 const &mouse)
{
  m_pStateData->sceneState.pRotate->Unclick(mouse);
  return new ProjectControllerStateIdle(m_pStateData);
}