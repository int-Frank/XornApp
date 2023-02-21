#include "XornActions.h"
#include "Project.h"

//------------------------------------------------------------------------------
// ActionGroup
//------------------------------------------------------------------------------

ActionGroup::~ActionGroup()
{
  for (auto pAction : m_actions)
    delete pAction;
}

void ActionGroup::AddAction(Action *pAction)
{
  m_actions.push_back(pAction);
}

bool ActionGroup::Do()
{
  for (auto pAction : m_actions)
    pAction->Do();
  return true;
}

bool ActionGroup::Undo()
{
  for (auto pAction : m_actions)
    pAction->Undo();
  return true;
}

//------------------------------------------------------------------------------
// Action_TransformPolygon
//------------------------------------------------------------------------------

Action_TransformPolygon::Action_TransformPolygon(ActionData const &data, PolygonID polygonID, xn::Transform const &oldTransform, xn::Transform const &newTransform)
  : ProjectAction(data)
  , m_polygonID(polygonID)
  , m_newTransform(newTransform)
  , m_oldTransform(oldTransform)
{

}

bool Action_TransformPolygon::Do()
{
  auto pLoop = m_actionData.pProject->loops.Get(m_polygonID);

  if (pLoop == nullptr)
    return false;

  pLoop->T_Model_World = m_newTransform;
  return true;
}

bool Action_TransformPolygon::Undo()
{
  auto pLoop = m_actionData.pProject->loops.Get(m_polygonID);

  if (pLoop == nullptr)
    return false;

  pLoop->T_Model_World = m_oldTransform;
  return true;
}

//------------------------------------------------------------------------------
// Action_AddSceneObject
//------------------------------------------------------------------------------

//Action_AddSceneObject::Action_AddSceneObject(SceneObject const &obj)
//  : m_newObject(obj)
//{
//
//}
//
//bool Action_AddSceneObject::Do(ActionData *pData)
//{
//  m_newObject.polygon.SetID(pData->pProj->sceneObjects.AddSceneObject(m_newObject));
//  return true;
//}
//
//bool Action_AddSceneObject::Undo(ActionData *pData)
//{
//  pData->pProj->sceneObjects.RemoveSceneObject(m_newObject.polygon.GetID());
//  return true;
//}

//------------------------------------------------------------------------------
// Action_RemoveSceneObject
//------------------------------------------------------------------------------

//Action_RemoveSceneObject::Action_RemoveSceneObject(uint64_t objectID)
//  : m_oldObject()
//{
//  m_oldObject.polygon.SetID(objectID);
//}
//
//bool Action_RemoveSceneObject::Do(ActionData *pData)
//{
//  pData->pProj->sceneObjects.RemoveSceneObject(m_oldObject.polygon.GetID());
//  return true;
//}
//
//bool Action_RemoveSceneObject::Undo(ActionData *pData)
//{
//  m_oldObject.polygon.SetID(pData->pProj->sceneObjects.AddSceneObject(m_oldObject));
//  return true;
//}

//------------------------------------------------------------------------------
// Action_MovePoint
//------------------------------------------------------------------------------

Action_MoveVertex::Action_MoveVertex(ActionData const &data, PolygonID polygonID, uint32_t pointIndex, xn::vec2 const &oldPoint, xn::vec2 const &newPoint)
  : ProjectAction(data)
  , m_polygonID(polygonID)
  , m_index(pointIndex)
  , m_oldPoint(oldPoint)
  , m_newPoint(newPoint)
{

}

bool Action_MoveVertex::Do()
{
  auto pLoop = m_actionData.pProject->loops.Get(m_polygonID);

  if (pLoop == nullptr)
    return false;

  if (m_index >= pLoop->vertices.size())
    return false;

  pLoop->vertices[m_index] = m_newPoint;

  return true;
}

bool Action_MoveVertex::Undo()
{
  auto pLoop = m_actionData.pProject->loops.Get(m_polygonID);

  if (pLoop == nullptr)
    return false;

  if (m_index >= pLoop->vertices.size())
    return false;

  pLoop->vertices[m_index] = m_oldPoint;

  return true;
}

//------------------------------------------------------------------------------
// Action_AddPoint
//------------------------------------------------------------------------------

Action_AddVertex::Action_AddVertex(ActionData const &data, PolygonID polygonID, uint32_t index, xn::vec2 const &newPoint)
  : ProjectAction(data)
  , m_polygonID(polygonID)
  , m_index(index)
  , m_newPoint(newPoint)
{

}

bool Action_AddVertex::Do()
{
  auto pLoop = m_actionData.pProject->loops.Get(m_polygonID);

  if (pLoop == nullptr)
    return false;

  if (m_index >= pLoop->vertices.size())
    return false;

  pLoop->vertices.insert(pLoop->vertices.begin() + m_index, m_newPoint);

  return true;
}

bool Action_AddVertex::Undo()
{
  auto pLoop = m_actionData.pProject->loops.Get(m_polygonID);

  if (pLoop == nullptr)
    return false;

  if (m_index >= pLoop->vertices.size())
    return false;

  pLoop->vertices.erase(pLoop->vertices.begin() + m_index);

  return true;
}

//------------------------------------------------------------------------------
// Action_RemovePoint
//------------------------------------------------------------------------------

Action_RemoveVertex::Action_RemoveVertex(ActionData const &data, PolygonID polygonID, uint32_t index, xn::vec2 const &oldPoint)
  : ProjectAction(data)
  , m_polygonID(polygonID)
  , m_index(index)
  , m_oldPoint(oldPoint)
{

}

bool Action_RemoveVertex::Do()
{
  auto pLoop = m_actionData.pProject->loops.Get(m_polygonID);

  if (pLoop == nullptr)
    return false;

  if (m_index >= pLoop->vertices.size())
    return false;

  pLoop->vertices.erase(pLoop->vertices.begin() + m_index);

  return true;
}

bool Action_RemoveVertex::Undo()
{
  auto pLoop = m_actionData.pProject->loops.Get(m_polygonID);

  if (pLoop == nullptr)
    return false;

  if (m_index >= pLoop->vertices.size())
    return false;

  pLoop->vertices.insert(pLoop->vertices.begin() + m_index, m_oldPoint);

  return true;
}