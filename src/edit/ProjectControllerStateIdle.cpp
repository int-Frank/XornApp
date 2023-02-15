
#include "ProjectControllerState.h"
#include "IRotateWidget.h"
#include "Input.h"
#include "DefaultData.h"
#include "DgQueryPointSegment.h"
#include "Logger.h"

//------------------------------------------------------------------------
// ProjectControllerStateIdle
//------------------------------------------------------------------------

ProjectControllerStateIdle::ProjectControllerStateIdle(ProjectControllerStateData *pData)
  : ProjectControllerState(pData)
  , m_hoverPolygon(INVALID_POLYGON_ID)
{

}

ProjectControllerState *ProjectControllerStateIdle::MouseMove(xn::vec2 const &mouse)
{
  m_hoverPolygon = INVALID_POLYGON_ID;
  uint32_t vertexID;
  PolygonUnderMouse(mouse, &m_hoverPolygon, &vertexID);

  return nullptr;
}

ProjectControllerState *ProjectControllerStateIdle::MouseDown(xn::MouseInput button, ModKey mod, xn::vec2 const &mouse)
{
  if (button != MOUSE_BUTTON_SELECT)
    return nullptr;

  PolygonID polyID;
  uint32_t vertexIndex;

  if (m_pStateData->pRotate != nullptr && m_pStateData->pRotate->MouseDown(mouse))
    return new ProjectControllerStateRotateSelected(m_pStateData);

  if (!PolygonUnderMouse(mouse, &polyID, &vertexIndex))
    return new ProjectControllerStateMultiSelect(m_pStateData, mouse);

  if (vertexIndex != 0xFFFFFFFF)
    return new ProjectControllerStateMoveVertex(m_pStateData, polyID, vertexIndex);

  bool selectedExists = m_pStateData->selectedPolygons.exists(polyID);

  if (mod == MK_shift)
  {
    if (selectedExists)
      m_pStateData->selectedPolygons.erase(polyID);
    else
      m_pStateData->selectedPolygons.insert(polyID);
    return nullptr;
  }

  if (!selectedExists)
  {
    m_pStateData->selectedPolygons.clear();
    m_pStateData->selectedPolygons.insert(polyID);
  }

  return new ProjectControllerStateDragSelected(m_pStateData, mouse);
}

void ProjectControllerStateIdle::UpdateScene(xn::IScene *pScene)
{
  for (auto it = m_pStateData->pProject->loops.Begin(); it != m_pStateData->pProject->loops.End(); it++)
  {
    if (m_pStateData->selectedPolygons.exists(it->first))
    {

    }
    else if (it->first == m_hoverPolygon)
    {
      pScene->AddPolygon(it->second.GetTransformed(),
        DefaultData::data.renderData.polygonAspect[HS_Hover].thickness,
        DefaultData::data.renderData.polygonAspect[HS_Hover].colour, 0, 0);
    }
    else
    {
      pScene->AddPolygon(it->second.GetTransformed(),
        DefaultData::data.renderData.polygonAspect[HS_None].thickness,
        DefaultData::data.renderData.polygonAspect[HS_None].colour, 0, 0);
    }
  }
}

bool ProjectControllerStateIdle::PolygonUnderMouse(xn::vec2 const &mouse, PolygonID *pID, uint32_t *vertex) const
{
  for (auto loop_it = m_pStateData->pProject->loops.Begin(); loop_it != m_pStateData->pProject->loops.End(); loop_it++)
  {
    auto loop = loop_it->second.GetTransformed();
    for (auto edge = loop.cEdgesBegin(); edge != loop.cEdgesEnd(); edge++)
    {
      auto seg = edge.ToSegment();
      xn::vec3 p0_3(seg.GetP0().x(), seg.GetP0().y(), 1.f);
      xn::vec3 p1_3(seg.GetP1().x(), seg.GetP1().y(), 1.f);

      auto p0 = p0_3 * m_pStateData->T_World_Screen;
      auto p1 = p1_3 * m_pStateData->T_World_Screen;

      seg.Set(p0, p1);

      Dg::CP2PointSegment<float> query;
      auto result = query(mouse, seg);
      float dist = Dg::MagSq(result.cp - mouse);
      float threshold = DefaultData::data.renderData.polygonAspect[HS_Hover].thickness;
      threshold *= threshold;
      if (dist < threshold)
      {
        *pID = loop_it->first;
        *vertex = 0;
        return true;
      }
    }
  }

  return false;
}


//------------------------------------------------------------------------
// ProjectControllerStateMultiSelect
//------------------------------------------------------------------------

ProjectControllerStateMultiSelect::ProjectControllerStateMultiSelect(ProjectControllerStateData *pState, xn::vec2 const &mouseAnchor)
  : ProjectControllerState(pState)
{

}

ProjectControllerState *ProjectControllerStateMultiSelect::MouseMove(xn::vec2 const &)
{
  return nullptr;
}

ProjectControllerState *ProjectControllerStateMultiSelect::MouseUp(xn::MouseInput, ModKey, xn::vec2 const &)
{
  return nullptr;
}

//------------------------------------------------------------------------
// ProjectControllerStateDragSelected
//------------------------------------------------------------------------

ProjectControllerStateDragSelected::ProjectControllerStateDragSelected(ProjectControllerStateData *pState, xn::vec2 const &mouseAnchor)
  : ProjectControllerState(pState)
{

}

ProjectControllerState *ProjectControllerStateDragSelected::MouseMove(xn::vec2 const &)
{
  return nullptr;
}

ProjectControllerState *ProjectControllerStateDragSelected::MouseUp(xn::MouseInput, ModKey, xn::vec2 const &)
{
  return nullptr;
}

//------------------------------------------------------------------------
// ProjectControllerStateMoveVertex
//------------------------------------------------------------------------

ProjectControllerStateMoveVertex::ProjectControllerStateMoveVertex(ProjectControllerStateData *pState, PolygonID, uint32_t vertexIndex)
  : ProjectControllerState(pState)
{

}

ProjectControllerState *ProjectControllerStateMoveVertex::MouseMove(xn::vec2 const &)
{
  return nullptr;
}

ProjectControllerState *ProjectControllerStateMoveVertex::MouseUp(xn::MouseInput, ModKey, xn::vec2 const &)
{
  return nullptr;
}

//------------------------------------------------------------------------
// ProjectControllerStateRotateSelected
//------------------------------------------------------------------------

ProjectControllerStateRotateSelected::ProjectControllerStateRotateSelected(ProjectControllerStateData *pState)
  : ProjectControllerState(pState)
{

}

ProjectControllerState *ProjectControllerStateRotateSelected::MouseMove(xn::vec2 const &)
{
  return nullptr;
}

ProjectControllerState *ProjectControllerStateRotateSelected::MouseUp(xn::MouseInput, ModKey, xn::vec2 const &)
{
  return nullptr;
}