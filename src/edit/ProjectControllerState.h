#ifndef PROJECTCONTROLLERSTATE_H
#define PROJECTCONTROLLERSTATE_H

#include "xnCommon.h"

#include "ProjectControllerStateData.h"
#include "Input.h"
#include "ActionList.h"

class Renderer;

class ProjectControllerState
{
public:

  ProjectControllerState(ProjectControllerStateData *pData)
    : m_pStateData(pData)
  {

  }

  virtual ProjectControllerState *MouseMove(uint32_t modState, xn::vec2 const &) { return nullptr; }
  virtual ProjectControllerState *MouseDown(uint32_t modState, xn::vec2 const &) { return nullptr; }
  virtual ProjectControllerState *MouseUp(uint32_t modState, xn::vec2 const &) { return nullptr; }
  
  virtual void Render(Renderer *) {}

  virtual void Undo() {};
  virtual void Redo() {};

protected:
  ProjectControllerStateData *m_pStateData;
};

class ProjectControllerStateIdle : public ProjectControllerState
{
public:

  ProjectControllerStateIdle(ProjectControllerStateData *);

  ProjectControllerState *MouseMove(uint32_t modState, xn::vec2 const &) override;
  ProjectControllerState *MouseDown(uint32_t modState, xn::vec2 const &) override;
  
  void Undo() override;
  void Redo() override;

private:

  PolygonID PolygonUnderMouse(xn::vec2 const &) const;
  bool VertexUnderMouse(xn::vec2 const &, PolygonID *, uint32_t *) const;
  bool SplitVertexUnderMouse(xn::vec2 const &, PolygonID *, uint32_t *) const;
};

class ProjectControllerStateMultiSelect : public ProjectControllerState
{
public:

  ProjectControllerStateMultiSelect(ProjectControllerStateData *, xn::vec2 const &mouseAnchor);

  ProjectControllerState *MouseMove(uint32_t modState, xn::vec2 const &) override;
  ProjectControllerState *MouseUp(uint32_t modState, xn::vec2 const &) override;
  void Render(Renderer *) override {}

private:

  xn::vec2 m_mouseAnchor;
};

class ProjectControllerStateMoveSelected : public ProjectControllerState
{
public:

  ProjectControllerStateMoveSelected(ProjectControllerStateData *, xn::vec2 const &mouseAnchor);

  ProjectControllerState *MouseMove(uint32_t modState, xn::vec2 const &) override;
  ProjectControllerState *MouseUp(uint32_t modState, xn::vec2 const &) override;

private:
  
  ActionID m_actionID;
  xn::vec2 m_mouseAnchor;
};

class ProjectControllerStateMoveVertex : public ProjectControllerState
{
public:

  ProjectControllerStateMoveVertex(ProjectControllerStateData *, xn::vec2 const & offset, PolygonID, uint32_t index);

  ProjectControllerState *MouseMove(uint32_t modState, xn::vec2 const &) override;
  ProjectControllerState *MouseUp(uint32_t modState, xn::vec2 const &) override;
 
private:

  xn::vec2 m_offset;
  ActionID m_actionID;
  PolygonID m_polygonID;
  uint32_t m_index;
};

class ProjectControllerStateRotateSelected : public ProjectControllerState
{
public:

  ProjectControllerStateRotateSelected(ProjectControllerStateData *);
  ProjectControllerState *MouseMove(uint32_t modState, xn::vec2 const &) override;
  ProjectControllerState *MouseUp(uint32_t modState, xn::vec2 const &) override;

};

#endif