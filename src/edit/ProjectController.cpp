
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
  m_pStateData->T_Screen_View.Identity();
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
  m_pStateData->T_Screen_View = T_View_Screen.GetInverse();
  m_pStateData->T_World_Screen = T_World_View * T_View_Screen;
  m_pStateData->T_Screen_World = m_pStateData->T_World_Screen.GetInverse();

  m_pState->SetRotateWidget();
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

  std::vector<xn::seg> edges;
  std::vector<xn::vec2> vertices;
  std::vector<xn::vec2> midVertices;

  for (auto it = m_pStateData->pProject->loops.Begin(); it != m_pStateData->pProject->loops.End(); it++)
  {
    if (m_pStateData->sceneState.selectedPolygons.exists(it->first))
    {
      auto polygon = it->second.GetTransformed();
      for (auto edge_it = polygon.cEdgesBegin(); edge_it != polygon.cEdgesEnd(); edge_it++)
        edges.push_back(edge_it.ToSegment());

      for (auto edge_it = polygon.cEdgesBegin(); edge_it != polygon.cEdgesEnd(); edge_it++)
      {
        auto seg = edge_it.ToSegment();
        midVertices.push_back(seg.GetCenter());
        vertices.push_back(seg.GetP0());
      }
    }
  }

  pRenderer->DrawLineGroup(edges,
    DefaultData::data.renderData.polygonAspect[HS_Active].thickness,
    DefaultData::data.renderData.polygonAspect[HS_Active].colour, 0);

  pRenderer->DrawFilledCircleGroup(vertices,
    DefaultData::data.renderData.vertexAspect.radius,
    DefaultData::data.renderData.vertexAspect.colour, 0);

  pRenderer->DrawFilledCircleGroup(midVertices,
    DefaultData::data.renderData.splitVertexAspect.radius,
    DefaultData::data.renderData.splitVertexAspect.colour, 0);

  m_pState->Render(pRenderer);

  if (m_pStateData->sceneState.pRotate != nullptr)
  {
    pRenderer->SetViewMatrix(m_pStateData->T_Screen_View);
    m_pStateData->sceneState.pRotate->Draw(pRenderer);
  }
}

void ProjectController::DrawBackSprites(Renderer *pRenderer)
{
  pRenderer->SetViewMatrix(m_pStateData->T_World_View);
  std::vector<xn::seg> edges;
  for (auto it = m_pStateData->pProject->loops.Begin(); it != m_pStateData->pProject->loops.End(); it++)
  {
    if (m_pStateData->sceneState.selectedPolygons.exists(it->first))
      continue;

    auto polygon = it->second.GetTransformed();
    for (auto edge_it = polygon.cEdgesBegin(); edge_it != polygon.cEdgesEnd(); edge_it++)
      edges.push_back(edge_it.ToSegment());
  }

  pRenderer->DrawLineGroup(edges,
    DefaultData::data.renderData.polygonAspect[HS_None].thickness,
    DefaultData::data.renderData.polygonAspect[HS_None].colour, 0);
}

void ProjectController::Undo()
{
  m_pState->Undo();
}

void ProjectController::Redo()
{
  m_pState->Redo();
}