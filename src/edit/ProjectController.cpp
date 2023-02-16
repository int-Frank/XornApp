
#include "IProjectController.h"
#include "ProjectControllerStateData.h"
#include "ProjectControllerState.h"
#include "ActionList.h"
#include "IRotateWidget.h"

class ProjectController : public IProjectController
{
public:

  ~ProjectController();
  ProjectController(Project *);

  void SetMatrix_World_Screen(xn::mat33 const &) override;

  void MouseMove(xn::vec2 const &) override;
  void MouseDown(ModKey, xn::vec2 const &) override;
  void MouseUp(ModKey, xn::vec2 const &) override;
  void KeyPress(Key, ModKey) override;

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
  m_pStateData->pRotate = nullptr;

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

void ProjectController::MouseDown(ModKey mod, xn::vec2 const &p)
{
  UPDATE_STATE(MouseDown(mod, p));
}

void ProjectController::MouseUp(ModKey mod, xn::vec2 const &p)
{
  UPDATE_STATE(MouseUp(mod, p));
}

void ProjectController::KeyPress(Key key, ModKey mod)
{
  UPDATE_STATE(KeyPress(key, mod));
}

void ProjectController::UpdateScene(xn::IScene *pScene)
{
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