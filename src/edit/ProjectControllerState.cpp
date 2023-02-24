#include "ProjectControllerState.h"
#include "IRotateWidget.h"

void ProjectControllerState::SetRotateWidget()
{
  if (m_pStateData->sceneState.selectedPolygons.empty())
  {
    delete m_pStateData->sceneState.pRotate;
    m_pStateData->sceneState.pRotate = nullptr;
    return;
  }

  if (m_pStateData->sceneState.pRotate == nullptr)
  {
    xn::vec2 minPoint(FLT_MAX, FLT_MAX);
    xn::vec2 maxPoint(-FLT_MAX, -FLT_MAX);

    for (auto it = m_pStateData->pProject->loops.Begin(); it != m_pStateData->pProject->loops.End(); it++)
    {
      if (!m_pStateData->sceneState.selectedPolygons.exists(it->first))
        continue;

      auto polygon = it->second.GetTransformed();
      for (auto it = polygon.cPointsBegin(); it != polygon.cPointsEnd(); it++)
      {
        if (it->x() < minPoint.x()) minPoint.x() = it->x();
        if (it->x() > maxPoint.x()) maxPoint.x() = it->x();
        if (it->y() < minPoint.y()) minPoint.y() = it->y();
        if (it->y() > maxPoint.y()) maxPoint.y() = it->y();
      }
    }

    m_pStateData->sceneState.rotateWorldPosition = (minPoint + maxPoint) / 2.f;
    m_pStateData->sceneState.pRotate = CreateRotateWidget(xn::vec2(0.f, 0.f));
  }

  xn::vec2 screenPosition = Multiply(m_pStateData->sceneState.rotateWorldPosition, m_pStateData->T_World_Screen);
  m_pStateData->sceneState.pRotate->SetPosition(screenPosition);
}