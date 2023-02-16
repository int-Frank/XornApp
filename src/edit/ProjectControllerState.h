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
  virtual ProjectControllerState *MouseDown(ModKey, xn::vec2 const &) { return nullptr; }
  virtual ProjectControllerState *MouseUp(ModKey, xn::vec2 const &) { return nullptr; }
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
  ProjectControllerState *MouseDown(ModKey, xn::vec2 const &) override;
  void UpdateScene(xn::IScene *) override;

private:

  PolygonID PolygonUnderMouse(xn::vec2 const &) const;
  xn::vec2 *VertexUnderMouse(xn::vec2 const &) const;
  xn::vec2 *SplitVertexUnderMouse(xn::vec2 const &) const;
  PolygonID m_hoverPolygon;
};

class ProjectControllerStateMultiSelect : public ProjectControllerState
{
public:

  ProjectControllerStateMultiSelect(ProjectControllerStateData *, xn::vec2 const &mouseAnchor);

  ProjectControllerState *MouseMove(xn::vec2 const &) override;
  ProjectControllerState *MouseUp(ModKey, xn::vec2 const &) override;
  void UpdateScene(xn::IScene *) override {}

private:

  xn::vec2 m_mouseAnchor;
};

class ProjectControllerStateDragSelected : public ProjectControllerState
{
public:

  ProjectControllerStateDragSelected(ProjectControllerStateData *, xn::vec2 const &mouseAnchor);

  ProjectControllerState *MouseMove(xn::vec2 const &) override;
  ProjectControllerState *MouseUp(ModKey, xn::vec2 const &) override;
  void UpdateScene(xn::IScene *) override {}

private:

  xn::vec2 m_mouseAnchor;
};

class ProjectControllerStateMoveVertex : public ProjectControllerState
{
public:

  ProjectControllerStateMoveVertex(ProjectControllerStateData *, xn::vec2 const & offset, xn::vec2 *);

  ProjectControllerState *MouseMove(xn::vec2 const &) override;
  ProjectControllerState *MouseUp(ModKey, xn::vec2 const &) override;
  void UpdateScene(xn::IScene *) override {}

private:

  xn::vec2 m_offset;
  xn::vec2 *m_pVertex;
};

class ProjectControllerStateRotateSelected : public ProjectControllerState
{
public:

  ProjectControllerStateRotateSelected(ProjectControllerStateData *);
  ProjectControllerState *MouseMove(xn::vec2 const &) override;
  ProjectControllerState *MouseUp(ModKey, xn::vec2 const &) override;
  void UpdateScene(xn::IScene *) override {}

};

#endif