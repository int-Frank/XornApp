#include "ProjectControllerState.h"


ProjectControllerStateTransition::ProjectControllerStateTransition(ProjectControllerStateData *pState, xn::vec2 const &mouseAnchor, PolygonID id)
  : ProjectControllerState(pState)
  , m_polygonUnderMouse(id)
  , m_mouseAnchor(mouseAnchor)
{

}

ProjectControllerState *ProjectControllerStateTransition::MouseMove(uint32_t modState, xn::vec2 const &mouse)
{
  return new ProjectControllerStateMoveSelected(m_pStateData, m_mouseAnchor);
}

ProjectControllerState *ProjectControllerStateTransition::MouseUp(uint32_t modState, xn::vec2 const &mouse)
{
  m_pStateData->sceneState.selectedPolygons.clear();
  m_pStateData->sceneState.selectedPolygons.insert(m_polygonUnderMouse);
  return new ProjectControllerStateIdle(m_pStateData);
}
