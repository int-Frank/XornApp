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

  virtual ~ProjectControllerState() {}

  ProjectControllerState(ProjectControllerStateData *pData)
    : m_pStateData(pData)
  {

  }

  void SetRotateWidget();

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
  
  void Render(Renderer *);

  void Undo() override;
  void Redo() override;

private:

  PolygonID PolygonUnderMouse(xn::vec2 const &, uint32_t* pVertexIndex) const;
  bool VertexUnderMouse(xn::vec2 const &, PolygonID *, uint32_t *) const;
  bool SplitVertexUnderMouse(xn::vec2 const &, PolygonID *, uint32_t *) const;

  PolygonID m_hoverPolygon;
};

class ProjectControllerStateTransition : public ProjectControllerState
{
public:

  ~ProjectControllerStateTransition();

  ProjectControllerStateTransition(ProjectControllerStateData *, ProjectControllerState* pNextState, PolygonID hoverPolygon);

  ProjectControllerState *MouseMove(uint32_t modState, xn::vec2 const &) override;
  ProjectControllerState *MouseUp(uint32_t modState, xn::vec2 const &) override;

private:

  PolygonID m_hoverPolygon;
  ProjectControllerState* m_pNextState;
};

class ProjectControllerStateMultiSelect : public ProjectControllerState
{
public:

  ProjectControllerStateMultiSelect(ProjectControllerStateData *, xn::vec2 const &mouseAnchor);

  ProjectControllerState *MouseMove(uint32_t modState, xn::vec2 const &) override;
  ProjectControllerState *MouseUp(uint32_t modState, xn::vec2 const &) override;
  void Render(Renderer *) override;

private:

  xn::aabb m_box;

  xn::vec2 m_mouse0;
  xn::vec2 m_mouse1;
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

class ProjectControllerStateMoveEdgePerpendicular : public ProjectControllerState
{
public:

  ProjectControllerStateMoveEdgePerpendicular(ProjectControllerStateData*, xn::vec2 const& mousePosition, PolygonID polygonID, uint32_t vertexIndex);

  ProjectControllerState* MouseMove(uint32_t modState, xn::vec2 const&) override;
  ProjectControllerState* MouseUp(uint32_t modState, xn::vec2 const&) override;

private:

  xn::vec2 m_offset;
  ActionID m_actionID;
  PolygonID m_polygonID;
  uint32_t m_index;
};

class ProjectControllerStateRotate : public ProjectControllerState
{
public:

  ProjectControllerStateRotate(ProjectControllerStateData *);
  ProjectControllerState *MouseMove(uint32_t modState, xn::vec2 const &) override;
  ProjectControllerState *MouseUp(uint32_t modState, xn::vec2 const &) override;

private:

  ActionID m_actionID;
};

class ProjectControllerStateMoveRotateWidget : public ProjectControllerState
{
public:

  ProjectControllerStateMoveRotateWidget(ProjectControllerStateData *, xn::vec2 const &offset);
  ProjectControllerState *MouseMove(uint32_t modState, xn::vec2 const &) override;
  ProjectControllerState *MouseUp(uint32_t modState, xn::vec2 const &) override;

private:

  xn::vec2 m_offset;
};

#endif