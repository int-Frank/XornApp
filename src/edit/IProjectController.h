#ifndef IPROJECTCONTROLLER_H
#define IPROJECTCONTROLLER_H

#include "xnCommon.h"
#include "xnGeometry.h"
#include "Project.h"
#include "edit/Input.h"

namespace xn
{
  class IScene;
}

class IProjectController
{
public:

  virtual ~IProjectController() {}

  virtual void SetMatrix_World_Screen(xn::mat33 const &) = 0;

  virtual void MouseMove(xn::vec2 const &) = 0;
  virtual void MouseDown(ModKey, xn::vec2 const &) = 0;
  virtual void MouseUp(ModKey, xn::vec2 const &) = 0;
  virtual void KeyPress(Key, ModKey) = 0;

  virtual void UpdateScene(xn::IScene *) = 0;

  virtual void Undo() = 0;
  virtual void Redo() = 0;
};

IProjectController *CreateProjectController(Project *);

#endif