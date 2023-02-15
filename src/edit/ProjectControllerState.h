#ifndef PROJECTCONTROLLERSTATE_H
#define PROJECTCONTROLLERSTATE_H

#include "xnIScene.h"
#include "xnCommon.h"

#include "ProjectControllerStateData.h"
#include "Input.h"

class ProjectControllerState
{
public:

  ProjectControllerState(ProjectControllerStateData *pData)
    : m_pStateData(pData)
  {

  }

  virtual ProjectControllerState *MouseMove(xn::vec2 const &) { return nullptr; }
  virtual ProjectControllerState *MouseDown(xn::MouseInput, ModKey, xn::vec2 const &) { return nullptr; }
  virtual ProjectControllerState *MouseUp(xn::MouseInput, ModKey, xn::vec2 const &) { return nullptr; }
  virtual ProjectControllerState *KeyPress(Key, ModKey) { return nullptr; }

  virtual void UpdateScene(xn::IScene *) = 0;

  virtual void Undo() {};
  virtual void Redo() {};

protected:
  ProjectControllerStateData *m_pStateData;
};

class ProjectControllerStateIdle : public ProjectControllerState
{
public:

  ProjectControllerStateIdle(ProjectControllerStateData *);

  ProjectControllerState *MouseMove(xn::vec2 const &) override;
  ProjectControllerState *MouseDown(xn::MouseInput, ModKey, xn::vec2 const &) override;
  void UpdateScene(xn::IScene *) override;

private:

  bool PolygonUnderMouse(xn::vec2 const &, PolygonID *, uint32_t *vertex) const;
  PolygonID m_hoverPolygon;
};

class ProjectControllerStateMultiSelect : public ProjectControllerState
{
public:

  ProjectControllerStateMultiSelect(ProjectControllerStateData *, xn::vec2 const &mouseAnchor);

  ProjectControllerState *MouseMove(xn::vec2 const &) override;
  ProjectControllerState *MouseUp(xn::MouseInput, ModKey, xn::vec2 const &) override;
  void UpdateScene(xn::IScene *) override {}

private:

  xn::vec2 m_mouseAnchor;
};

class ProjectControllerStateDragSelected : public ProjectControllerState
{
public:

  ProjectControllerStateDragSelected(ProjectControllerStateData *, xn::vec2 const &mouseAnchor);

  ProjectControllerState *MouseMove(xn::vec2 const &) override;
  ProjectControllerState *MouseUp(xn::MouseInput, ModKey, xn::vec2 const &) override;
  void UpdateScene(xn::IScene *) override {}

private:

  xn::vec2 m_mouseAnchor;
};

class ProjectControllerStateMoveVertex : public ProjectControllerState
{
public:

  ProjectControllerStateMoveVertex(ProjectControllerStateData *, PolygonID, uint32_t vertexIndex);

  ProjectControllerState *MouseMove(xn::vec2 const &) override;
  ProjectControllerState *MouseUp(xn::MouseInput, ModKey, xn::vec2 const &) override;
  void UpdateScene(xn::IScene *) override {}

private:
  PolygonID m_polygon;
  uint32_t m_vertexIndex;
};

class ProjectControllerStateRotateSelected : public ProjectControllerState
{
public:

  ProjectControllerStateRotateSelected(ProjectControllerStateData *);
  ProjectControllerState *MouseMove(xn::vec2 const &) override;
  ProjectControllerState *MouseUp(xn::MouseInput, ModKey, xn::vec2 const &) override;
  void UpdateScene(xn::IScene *) override {}

};

#endif