
#include "IProjectController.h"
#include "ProjectControllerStateData.h"
#include "ProjectControllerState.h"
#include "ActionList.h"
#include "IRotateWidget.h"
#include "DefaultData.h"
#include "Renderer.h"

class ProjectController : public IProjectController
{
public:

  ~ProjectController();
  ProjectController(Project *);

  void SetMatrices(xn::mat33 const &T_World_View, xn::mat33 const &T_View_Screen) override;

  void MouseMove(uint32_t modState, xn::vec2 const &) override;
  void MouseDown(uint32_t modState, xn::vec2 const &) override;
  void MouseUp(uint32_t modState, xn::vec2 const &) override;

  void DrawBackSprites(Renderer *) override;
  void DrawFrontSprites(Renderer *) override;

  void Undo() override;
  void Redo() override;

private:

  void SetRotateWidget();

  ProjectControllerState *m_pState;
  ProjectControllerStateData *m_pStateData;
};

IProjectController *CreateProjectController(Project *pProject)
{
  return new ProjectController(pProject);
}

ProjectController::ProjectController(Project *pProject)
  : m_pState(nullptr)
  , m_pStateData(nullptr)
{
  m_pStateData = new ProjectControllerStateData{};
  m_pStateData->T_World_View.Identity();
  m_pStateData->T_View_Screen.Identity();
  m_pStateData->T_World_Screen.Identity();
  m_pStateData->T_Screen_World.Identity();
  m_pStateData->pProject = pProject;
  m_pStateData->pActions = CreateActionList();

  m_pState = new ProjectControllerStateIdle(m_pStateData);
}

ProjectController::~ProjectController()
{
  delete m_pStateData->pActions;
  delete m_pStateData;
  delete m_pState;
}

void ProjectController::SetMatrices(xn::mat33 const &T_World_View, xn::mat33 const &T_View_Screen)
{
  m_pStateData->T_World_View = T_World_View;
  m_pStateData->T_View_Screen = T_View_Screen;
  m_pStateData->T_World_Screen = T_World_View * T_View_Screen;
  m_pStateData->T_Screen_World = m_pStateData->T_World_Screen.GetInverse();
}

#define UPDATE_STATE(fn) auto pState = m_pState->fn;\
if (pState != nullptr)\
{\
  delete m_pState;\
  m_pState = pState;\
}

void ProjectController::MouseMove(uint32_t modState, xn::vec2 const &p)
{
  UPDATE_STATE(MouseMove(modState, p));
}

void ProjectController::MouseDown(uint32_t modState, xn::vec2 const &p)
{
  UPDATE_STATE(MouseDown(modState, p));
}

void ProjectController::MouseUp(uint32_t modState, xn::vec2 const &p)
{
  UPDATE_STATE(MouseUp(modState, p));
}

void  ProjectController::DrawFrontSprites(Renderer *pRenderer)
{
  pRenderer->SetViewMatrix(m_pStateData->T_World_View);
  for (auto it = m_pStateData->pProject->loops.Begin(); it != m_pStateData->pProject->loops.End(); it++)
  {
    if (m_pStateData->sceneState.selectedPolygons.exists(it->first))
    {
      auto polygon = it->second.GetTransformed();
      pRenderer->DrawPolygon(polygon,
        DefaultData::data.renderData.polygonAspect[HS_Active].thickness,
        DefaultData::data.renderData.polygonAspect[HS_Active].colour, 0);

      std::vector<xn::vec2> vertices;
      std::vector<xn::vec2> midVertices;

      for (auto edge_it = polygon.cEdgesBegin(); edge_it != polygon.cEdgesEnd(); edge_it++)
      {
        auto seg = edge_it.ToSegment();
        midVertices.push_back(seg.GetCenter());
        vertices.push_back(seg.GetP0());
      }

      pRenderer->DrawFilledCircleGroup(vertices,
        DefaultData::data.renderData.vertexAspect.radius,
        DefaultData::data.renderData.vertexAspect.colour, 0);

      pRenderer->DrawFilledCircleGroup(midVertices,
        DefaultData::data.renderData.splitVertexAspect.radius,
        DefaultData::data.renderData.splitVertexAspect.colour, 0);
    }
  }

  //m_pState->UpdateScene(pScene);

  SetRotateWidget();
  if (m_pStateData->sceneState.pRotate != nullptr)
  {
    pRenderer->SetViewMatrix(m_pStateData->T_View_Screen.GetInverse());
    m_pStateData->sceneState.pRotate->Draw(pRenderer);
  }
}

void ProjectController::DrawBackSprites(Renderer *pRenderer)
{
  pRenderer->SetViewMatrix(m_pStateData->T_World_View);
  for (auto it = m_pStateData->pProject->loops.Begin(); it != m_pStateData->pProject->loops.End(); it++)
  {
    if (m_pStateData->sceneState.selectedPolygons.exists(it->first))
      continue;

    auto polygon = it->second.GetTransformed();
    auto state = it->first == m_pStateData->sceneState.hoverPolygon ? HS_Hover : HS_None;

    pRenderer->DrawPolygon(polygon,
      DefaultData::data.renderData.polygonAspect[state].thickness,
      DefaultData::data.renderData.polygonAspect[state].colour, 0);
  }
}

void ProjectController::SetRotateWidget()
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

  if (minPoint.x() == FLT_MAX)
  {
    delete m_pStateData->sceneState.pRotate;
    m_pStateData->sceneState.pRotate = nullptr;
    return;
  }

  xn::vec2 centre = (minPoint + maxPoint) / 2.f;
  xn::vec3 centre3 = Dg::ToVector3(centre, 1.f);
  centre3 = centre3 * m_pStateData->T_World_Screen;
  centre = Dg::ToVector2(centre3);

  if (m_pStateData->sceneState.pRotate == nullptr)
    m_pStateData->sceneState.pRotate = CreateRotateWidget(centre);
  else
    m_pStateData->sceneState.pRotate->SetPosition(centre);
}

void ProjectController::Undo()
{
  m_pState->Undo();
}

void ProjectController::Redo()
{
  m_pState->Redo();
}