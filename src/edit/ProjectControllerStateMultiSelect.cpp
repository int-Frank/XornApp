
#include "DgQueryPointAABB.h"

#include "ProjectControllerState.h"
#include "Renderer.h"
#include "IRotateWidget.h"

ProjectControllerStateMultiSelect::ProjectControllerStateMultiSelect(ProjectControllerStateData *pState, xn::vec2 const &mouseAnchor)
  : ProjectControllerState(pState)
  , m_mouse0(mouseAnchor)
  , m_mouse1(mouseAnchor)
{

}

void ProjectControllerStateMultiSelect::Render(Renderer *pRenderer)
{
  static int d = 0;
  d++;

  xn::vec2 p0 = m_box.Min();
  xn::vec2 p1 = m_box.Max();

  std::vector<xn::vec2> renderBox;
  renderBox.push_back(p0);
  renderBox.push_back(xn::vec2(p0.x(), p1.y()));
  renderBox.push_back(p1);
  renderBox.push_back(xn::vec2(p1.x(), p0.y()));

  pRenderer->SetViewMatrix(m_pStateData->T_Screen_View);
  pRenderer->DrawFilledConvexPolygon(renderBox, 0x33FFAAAA, 0);
  pRenderer->DrawPolygon(renderBox, 2.f, 0xDDFFAAAA, xn::RF_RoundedEndPoints);
}

ProjectControllerState *ProjectControllerStateMultiSelect::MouseMove(uint32_t modState, xn::vec2 const &mouse)
{
  m_mouse1 = mouse;
  m_box = xn::aabb(m_mouse0, m_mouse0);
  m_box += m_mouse1;

  return nullptr;
}

ProjectControllerState *ProjectControllerStateMultiSelect::MouseUp(uint32_t modState, xn::vec2 const &mouse)
{
  m_mouse1 = Multiply(mouse, m_pStateData->T_Screen_World);
  m_mouse0 = Multiply(m_mouse0, m_pStateData->T_Screen_World);

  m_box = xn::aabb(m_mouse0, m_mouse0);
  m_box += m_mouse1;

  if ((modState & xn::MK_shift) == 0)
    m_pStateData->sceneState.selectedPolygons.clear();

  for (auto it = m_pStateData->pProject->loops.Begin(); it != m_pStateData->pProject->loops.End(); it++)
  {
    auto polygon = it->second.GetTransformed();
    if (m_pStateData->sceneState.selectedPolygons.exists(it->first))
      continue;

    bool inside = true;
    for (auto it_vert = polygon.cPointsBegin(); it_vert != polygon.cPointsEnd(); it_vert++)
    {
      Dg::TI2PointAABB<float> query;
      auto result = query(*it_vert, m_box);
      if (!result.isIntersecting)
      {
        inside = false;
        break;
      }
    }
    if (inside)
      m_pStateData->sceneState.selectedPolygons.insert(it->first);
  }

  SetRotateWidget();
  return new ProjectControllerStateIdle(m_pStateData);
}