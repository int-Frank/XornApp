
#include "ProjectControllerState.h"
#include "Input.h"
#include "DefaultData.h"

ProjectControllerStateMoveSelected::ProjectControllerStateMoveSelected(ProjectControllerStateData *pState, xn::vec2 const &mouseAnchor)
  : ProjectControllerState(pState)
  , m_transforms()
  , m_mouseAnchor(mouseAnchor)
{
  for (auto it = m_pStateData->pProject->loops.Begin(); it != m_pStateData->pProject->loops.End(); it++)
  {
    if (m_pStateData->sceneState.selectedPolygons.exists(it->first))
      m_transforms.insert(Dg::Pair<PolygonID, xn::Transform>(it->first, it->second.T_Model_World));
  }
}

ProjectControllerState *ProjectControllerStateMoveSelected::MouseMove(xn::vec2 const &mouse)
{
  // TODO Add action!

  return nullptr;
}

ProjectControllerState *ProjectControllerStateMoveSelected::MouseUp(ModKey, xn::vec2 const &)
{
  return new ProjectControllerStateIdle(m_pStateData);
}

void ProjectControllerStateMoveSelected::UpdateScene(xn::IScene *pScene)
{
  /*for (auto it = m_pStateData->pProject->loops.Begin(); it != m_pStateData->pProject->loops.End(); it++)
  {
    if (m_pStateData->sceneState.selectedPolygons.exists(it->first))
    {
      pScene->AddPolygon(it->second.GetTransformed(),
        DefaultData::data.renderData.polygonAspect[HS_Acitve].thickness,
        DefaultData::data.renderData.polygonAspect[HS_Acitve].colour, 0, 0);
    }
    else
    {
      pScene->AddPolygon(it->second.GetTransformed(),
        DefaultData::data.renderData.polygonAspect[HS_None].thickness,
        DefaultData::data.renderData.polygonAspect[HS_None].colour, 0, 0);
    }
  }*/
}