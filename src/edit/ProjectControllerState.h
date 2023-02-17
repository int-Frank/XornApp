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
  bool VertexUnderMouse(xn::vec2 const &, PolygonID *, uint32_t *) const;
  bool SplitVertexUnderMouse(xn::vec2 const &, PolygonID *, uint32_t *) const;
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

class ProjectControllerStateMoveSelected : public ProjectControllerState
{
public:

  ProjectControllerStateMoveSelected(ProjectControllerStateData *, xn::vec2 const &mouseAnchor);

  ProjectControllerState *MouseMove(xn::vec2 const &) override;
  ProjectControllerState *MouseUp(ModKey, xn::vec2 const &) override;
  void UpdateScene(xn::IScene *) override;

private:

  Dg::Map_AVL<PolygonID, xn::Transform> m_transforms;
  xn::vec2 m_mouseAnchor;
};

class ProjectControllerStateMoveVertex : public ProjectControllerState
{
public:

  ProjectControllerStateMoveVertex(ProjectControllerStateData *, xn::vec2 const & offset, PolygonID, uint32_t index);

  ProjectControllerState *MouseMove(xn::vec2 const &) override;
  ProjectControllerState *MouseUp(ModKey, xn::vec2 const &) override;
  void UpdateScene(xn::IScene *) override {}

private:

  xn::vec2 m_offset;
  PolygonID m_polygonID;
  uint32_t m_index;
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