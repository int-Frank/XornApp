
#include "IProjectController.h"
#include "ProjectControllerStateData.h"
#include "ProjectControllerState.h"
#include "ActionList.h"
#include "IRotateWidget.h"
#include "DefaultData.h"

class ProjectController : public IProjectController
{
public:

  ~ProjectController();
  ProjectController(Project *);

  void SetMatrix_World_Screen(xn::mat33 const &) override;

  void MouseMove(xn::vec2 const &) override;
  void MouseDown(uint32_t modState, xn::vec2 const &) override;
  void MouseUp(uint32_t modState, xn::vec2 const &) override;

  void UpdateScene(xn::IScene *) override;

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
  m_pStateData->T_World_Screen.Identity();
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

void ProjectController::SetMatrix_World_Screen(xn::mat33 const &m)
{
  m_pStateData->T_World_Screen = m;
}

#define UPDATE_STATE(f) auto pState = m_pState->f;\
if (pState != nullptr)\
{\
  delete m_pState;\
  m_pState = pState;\
}

void ProjectController::MouseMove(xn::vec2 const &p)
{
  UPDATE_STATE(MouseMove(p));
}

void ProjectController::MouseDown(uint32_t modState, xn::vec2 const &p)
{
  UPDATE_STATE(MouseDown(modState, p));
}

void ProjectController::MouseUp(uint32_t modState, xn::vec2 const &p)
{
  UPDATE_STATE(MouseUp(modState, p));
}

void ProjectController::UpdateScene(xn::IScene *pScene)
{
  for (auto it = m_pStateData->pProject->loops.Begin(); it != m_pStateData->pProject->loops.End(); it++)
  {
    auto polygon = it->second.GetTransformed();

    if (m_pStateData->sceneState.selectedPolygons.exists(it->first))
    {
      pScene->AddPolygon(polygon,
        DefaultData::data.renderData.polygonAspect[HS_Acitve].thickness,
        DefaultData::data.renderData.polygonAspect[HS_Acitve].colour, 0, 0);

      std::vector<xn::vec2> vertices;
      std::vector<xn::vec2> midVertices;

      for (auto edge_it = polygon.cEdgesBegin(); edge_it != polygon.cEdgesEnd(); edge_it++)
      {
        auto seg = edge_it.ToSegment();
        midVertices.push_back(seg.GetCenter());
        vertices.push_back(seg.GetP0());
      }

      pScene->AddFilledCircleGroup(vertices,
        DefaultData::data.renderData.vertexAspect.radius,
        DefaultData::data.renderData.vertexAspect.colour, 0, 0);

      pScene->AddFilledCircleGroup(midVertices,
        DefaultData::data.renderData.splitVertexAspect.radius,
        DefaultData::data.renderData.splitVertexAspect.colour, 0, 0);
    }
    else if (it->first == m_pStateData->sceneState.hoverPolygon)
    {
      pScene->AddPolygon(polygon,
        DefaultData::data.renderData.polygonAspect[HS_Hover].thickness,
        DefaultData::data.renderData.polygonAspect[HS_Hover].colour, 0, 0);
    }
    else
    {
      pScene->AddPolygon(polygon,
        DefaultData::data.renderData.polygonAspect[HS_None].thickness,
        DefaultData::data.renderData.polygonAspect[HS_None].colour, 0, 0);
    }
  }

  m_pState->UpdateScene(pScene);
}

void ProjectController::Undo()
{
  m_pState->Undo();
}

void ProjectController::Redo()
{
  m_pState->Redo();
}