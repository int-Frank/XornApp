#include "ProjectControllerState.h"


ProjectControllerStateTransition::ProjectControllerStateTransition(ProjectControllerStateData *pState, ProjectControllerState *pNextState, PolygonID id)
  : ProjectControllerState(pState)
  , m_hoverPolygon(id)
  , m_pNextState(pNextState)
{

}

ProjectControllerStateTransition::~ProjectControllerStateTransition()
{
  delete m_pNextState;
}

ProjectControllerState *ProjectControllerStateTransition::MouseMove(uint32_t modState, xn::vec2 const &mouse)
{
  ProjectControllerState* pNextState = m_pNextState;
  m_pNextState = nullptr;
  return pNextState;
}

ProjectControllerState *ProjectControllerStateTransition::MouseUp(uint32_t modState, xn::vec2 const &mouse)
{
  m_pStateData->sceneState.selectedPolygons.clear();
  m_pStateData->sceneState.selectedPolygons.insert(m_hoverPolygon);
  return new ProjectControllerStateIdle(m_pStateData);
}
