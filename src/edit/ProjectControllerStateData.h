#ifndef PROJECTCONTROLLERSTATEDATA_H
#define PROJECTCONTROLLERSTATEDATA_H

#include "DgSet_AVL.h"
#include "Project.h"

class IActionList;
class IRotateWidget;

struct ProjectControllerStateData
{
  xn::mat33 T_World_Screen;
  Dg::Set_AVL<PolygonID> selectedPolygons;
  IActionList *pActions;
  Project *pProject;
  IRotateWidget *pRotate;
};


#endif