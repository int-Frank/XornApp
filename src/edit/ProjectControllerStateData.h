#ifndef PROJECTCONTROLLERSTATEDATA_H
#define PROJECTCONTROLLERSTATEDATA_H

#include "DgSet_AVL.h"
#include "Project.h"

class IActionList;
class IRotateWidget;

#define INVALID_INDEX 0xFFFFFFFFul

class SceneState
{
public:

  SceneState()
    : selectedPolygons()
    , pRotate(nullptr)
  {

  }

  Dg::Set_AVL<PolygonID> selectedPolygons;
  IRotateWidget *pRotate;
  xn::vec2 rotateWorldPosition;
};

struct ProjectControllerStateData
{
  xn::mat33 T_World_View;
  xn::mat33 T_View_Screen;
  xn::mat33 T_Screen_View;
  xn::mat33 T_World_Screen;
  xn::mat33 T_Screen_World;
  SceneState sceneState;
  IActionList *pActions;
  Project *pProject;
};


#endif