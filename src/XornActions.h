//#ifndef XORNACTIONS_H
//#define XORNACTIONS_H
//
//#include <stdint.h>
//#include <string>
//
//#include "Action.h"
//#include "xnGeometry.h"
//
//class Action_TransformPolygon : public Action
//{
//public:
//
//  Action_TransformPolygon(uint64_t polygonID, xn::Transform const &oldTransform, xn::Transform const &newTransform);
//
//  bool Do(ActionData *) override;
//  bool Undo(ActionData *) override;
//
//private:
//
//  uint64_t m_polygonID;
//  xn::Transform m_oldTransform;
//  xn::Transform m_newTransform;
//};
//
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
//
//class Action_MovePoint : public Action
//{
//public:
//
//  Action_MovePoint(uint64_t polygonID, uint32_t pointID, xn::vec2 const &oldPoint, xn::vec2 const &newPoint);
//
//  bool Do(ActionData *) override;
//  bool Undo(ActionData *) override;
//
//private:
//
//  uint64_t m_polygonID;
//  uint32_t m_pointID;
//  xn::vec2 m_oldPoint;
//  xn::vec2 m_newPoint;
//};
//
//class Action_MoveEdge : public Action
//{
//public:
//
//  Action_MoveEdge(uint64_t polygonID, uint32_t pointID, 
//    xn::vec2 const &oldPt0, xn::vec2 const &oldPt1, xn::vec2 const &newPt0, xn::vec2 const &newPt1);
//
//  bool Do(ActionData *) override;
//  bool Undo(ActionData *) override;
//
//private:
//
//  uint64_t m_polygonID;
//  uint32_t m_pointID;
//  xn::vec2 m_oldPoints[2];
//  xn::vec2 m_newPoints[2];
//};
//
//class Action_AddPoint : public Action
//{
//public:
//
//  Action_AddPoint(uint64_t polygonID, uint32_t pointBefore, xn::vec2 const &newPoint);
//
//  bool Do(ActionData *) override;
//  bool Undo(ActionData *) override;
//
//private:
//
//  uint64_t m_polygonID;
//  uint32_t m_pointBefore;
//  xn::vec2 m_newPoint;
//};
//
//class Action_RemovePoint : public Action
//{
//public:
//
//  Action_RemovePoint(uint64_t polygonID, uint32_t pointID, xn::vec2 const &oldPoint);
//
//  bool Do(ActionData *) override;
//  bool Undo(ActionData *) override;
//
//private:
//
//  uint64_t m_polygonID;
//  uint32_t m_pointID;
//  xn::vec2 m_oldPoint;
//};
//
//#endif