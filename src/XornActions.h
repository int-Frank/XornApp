#ifndef XORNACTIONS_H
#define XORNACTIONS_H

#include <stdint.h>
#include <string>

#include "Action.h"
#include "xnGeometry.h"
#include "Project.h"

class ActionGroup : public Action
{
public:

  ~ActionGroup();
  void AddAction(Action *);

  bool Do() override;
  bool Undo() override;

private:
  std::vector<Action *> m_actions;
};

class Action_TransformPolygon : public ProjectAction
{
public:

  ~Action_TransformPolygon() {};
  Action_TransformPolygon(ActionData const &, PolygonID polygonID, xn::Transform const &oldTransform, xn::Transform const &newTransform);

  bool Do() override;
  bool Undo() override;

private:

  PolygonID m_polygonID;
  xn::Transform m_oldTransform;
  xn::Transform m_newTransform;
};

//class Action_AddSceneObject : public Action
//{
//public:
//
//  Action_AddSceneObject(SceneObject const &);
//
//  bool Do(ActionData *) override;
//  bool Undo(ActionData *) override;
//
//private:
//
//  SceneObject m_newObject;
//};
//
//class Action_RemoveSceneObject : public Action
//{
//public:
//
//  Action_RemoveSceneObject(uint64_t objectID);
//
//  bool Do(ActionData *) override;
//  bool Undo(ActionData *) override;
//
//private:
//
//  SceneObject m_oldObject;
//};

class Action_MoveVertex : public ProjectAction
{
public:

  ~Action_MoveVertex() {};
  Action_MoveVertex(ActionData const &, PolygonID polygonID, uint32_t pointIndex, xn::vec2 const &oldPoint, xn::vec2 const &newPoint);

  bool Do() override;
  bool Undo() override;

private:

  PolygonID m_polygonID;
  uint32_t m_index;
  xn::vec2 m_oldPoint;
  xn::vec2 m_newPoint;
};

class Action_AddVertex : public ProjectAction
{
public:

  ~Action_AddVertex() {}
  Action_AddVertex(ActionData const &, PolygonID polygonID, uint32_t index, xn::vec2 const &newPoint);

  bool Do() override;
  bool Undo() override;

private:

  PolygonID m_polygonID;
  uint32_t m_index;
  xn::vec2 m_newPoint;
};

class Action_RemoveVertex : public ProjectAction
{
public:

  ~Action_RemoveVertex() {}
  Action_RemoveVertex(ActionData const &, PolygonID polygonID, uint32_t index, xn::vec2 const &oldPoint);

  bool Do() override;
  bool Undo() override;

private:

  PolygonID m_polygonID;
  uint32_t m_index;
  xn::vec2 m_oldPoint;
};

#endif