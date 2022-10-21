//#include "XornActions.h"
//#include "Project.h"
//
////------------------------------------------------------------------------------
//// Action_TransformPolygon
////------------------------------------------------------------------------------
//
//Action_TransformPolygon::Action_TransformPolygon(uint64_t polygonID, xn::Transform const &oldTransform, xn::Transform const &newTransform)
//  : m_polygonID(polygonID)
//  , m_newTransform(newTransform)
//  , m_oldTransform(oldTransform)
//{
//
//}
//
//bool Action_TransformPolygon::Do(ActionData *pData)
//{
//  SceneObject *pObj = pData->pProj->sceneObjects.GetSceneObject(m_polygonID);
//
//  if (pObj == nullptr)
//    return false;
//
//  pObj->transform = m_newTransform;
//  return true;
//}
//
//bool Action_TransformPolygon::Undo(ActionData *pData)
//{
//  SceneObject *pObj = pData->pProj->sceneObjects.GetSceneObject(m_polygonID);
//
//  if (pObj == nullptr)
//    return false;
//
//  pObj->transform = m_oldTransform;
//  return true;
//}
//
////------------------------------------------------------------------------------
//// Action_AddSceneObject
////------------------------------------------------------------------------------
//
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
//
////------------------------------------------------------------------------------
//// Action_RemoveSceneObject
////------------------------------------------------------------------------------
//
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
//
////------------------------------------------------------------------------------
//// Action_MovePoint
////------------------------------------------------------------------------------
//
//Action_MovePoint::Action_MovePoint(uint64_t polygonID, uint32_t pointID, xn::vec2 const &oldPoint, xn::vec2 const &newPoint)
//  : m_polygonID(polygonID)
//  , m_pointID(pointID)
//  , m_oldPoint(oldPoint)
//  , m_newPoint(newPoint)
//{
//
//}
//
//bool Action_MovePoint::Do(ActionData *pData)
//{
//  SceneObject *pObj = pData->pProj->sceneObjects.GetSceneObject(m_polygonID);
//
//  if (pObj == nullptr)
//    return false;
//
//  if (m_pointID >= pObj->polygon.Size())
//    return false;
//
//  *(pObj->polygon.PointsBegin() + m_pointID) = m_newPoint;
//
//  return true;
//}
//
//bool Action_MovePoint::Undo(ActionData *pData)
//{
//  SceneObject *pObj = pData->pProj->sceneObjects.GetSceneObject(m_polygonID);
//
//  if (pObj == nullptr)
//    return false;
//
//  if (m_pointID >= pObj->polygon.Size())
//    return false;
//
//  *(pObj->polygon.PointsBegin() + m_pointID) = m_oldPoint;
//
//  return true;
//}
//
////------------------------------------------------------------------------------
//// Action_MoveEdge
////------------------------------------------------------------------------------
//
//Action_MoveEdge::Action_MoveEdge(uint64_t polygonID, uint32_t pointID,
//  xn::vec2 const &oldPt0, xn::vec2 const &oldPt1, xn::vec2 const &newPt0, xn::vec2 const &newPt1)
//  : m_polygonID(polygonID)
//  , m_pointID(pointID)
//  , m_oldPoints{ oldPt0, oldPt1 }
//  , m_newPoints{ newPt0, newPt1 }
//{
//
//}
//
//bool Action_MoveEdge::Do(ActionData *pData)
//{
//  SceneObject *pObj = pData->pProj->sceneObjects.GetSceneObject(m_polygonID);
//
//  if (pObj == nullptr)
//    return false;
//
//  if (m_pointID + 1 >= pObj->polygon.Size())
//    return false;
//
//  *(pObj->polygon.PointsBegin() + m_pointID) = m_newPoints[0];
//  *(pObj->polygon.PointsBegin() + m_pointID + 1) = m_newPoints[1];
//
//  return true;
//}
//
//bool Action_MoveEdge::Undo(ActionData *pData)
//{
//  SceneObject *pObj = pData->pProj->sceneObjects.GetSceneObject(m_polygonID);
//
//  if (pObj == nullptr)
//    return false;
//
//  if (m_pointID + 1 >= pObj->polygon.Size())
//    return false;
//
//  *(pObj->polygon.PointsBegin() + m_pointID) = m_oldPoints[0];
//  *(pObj->polygon.PointsBegin() + m_pointID + 1) = m_oldPoints[1];
//
//  return true;
//}
//
////------------------------------------------------------------------------------
//// Action_AddPoint
////------------------------------------------------------------------------------
//
//Action_AddPoint::Action_AddPoint(uint64_t polygonID, uint32_t pointBefore, xn::vec2 const &newPoint)
//  : m_polygonID(polygonID)
//  , m_pointBefore(pointBefore)
//  , m_newPoint(newPoint)
//{
//
//}
//
//bool Action_AddPoint::Do(ActionData *pData)
//{
//  SceneObject *pObj = pData->pProj->sceneObjects.GetSceneObject(m_polygonID);
//
//  if (pObj == nullptr)
//    return false;
//
//  if (m_pointBefore > pObj->polygon.Size())
//    return false;
//
//  pObj->polygon.Insert(pObj->polygon.PointsBegin() + m_pointBefore, m_newPoint);
//
//  return true;
//}
//
//bool Action_AddPoint::Undo(ActionData *pData)
//{
//  SceneObject *pObj = pData->pProj->sceneObjects.GetSceneObject(m_polygonID);
//
//  if (pObj == nullptr)
//    return false;
//
//  if (m_pointBefore + 1 >= pObj->polygon.Size())
//    return false;
//
//  pObj->polygon.Erase(pObj->polygon.PointsBegin() + m_pointBefore + 1);
//
//  return true;
//}
//
////------------------------------------------------------------------------------
//// Action_RemovePoint
////------------------------------------------------------------------------------
//
//Action_RemovePoint::Action_RemovePoint(uint64_t polygonID, uint32_t pointID, xn::vec2 const &oldPoint)
//  : m_polygonID(polygonID)
//  , m_pointID(pointID)
//  , m_oldPoint(oldPoint)
//{
//
//}
//
//bool Action_RemovePoint::Do(ActionData *pData)
//{
//  SceneObject *pObj = pData->pProj->sceneObjects.GetSceneObject(m_polygonID);
//
//  if (pObj == nullptr)
//    return false;
//
//  if (m_pointID >= pObj->polygon.Size())
//    return false;
//
//  pObj->polygon.Erase(pObj->polygon.PointsBegin() + m_pointID);
//
//  return true;
//}
//
//bool Action_RemovePoint::Undo(ActionData *pData)
//{
//  SceneObject *pObj = pData->pProj->sceneObjects.GetSceneObject(m_polygonID);
//
//  if (pObj == nullptr)
//    return false;
//
//  if (m_pointID >= pObj->polygon.Size())
//    return false;
//
//  if (pObj->polygon.Size() == 0)
//  {
//    pObj->polygon.PushBack(m_oldPoint);
//  }
//  else
//  {
//    uint32_t index = m_pointID == 0 ? pObj->polygon.Size() - 1 : m_pointID;
//    pObj->polygon.Insert(pObj->polygon.PointsBegin() + index, m_oldPoint);
//  }
//
//  return true;
//}