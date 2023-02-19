#ifndef IPROJECTCONTROLLER_H
#define IPROJECTCONTROLLER_H

#include "xnCommon.h"
#include "xnGeometry.h"
#include "Project.h"
#include "Input.h"

class Renderer;

class IProjectController
{
public:

  virtual ~IProjectController() {}

  virtual void SetMatrices(xn::mat33 const &T_World_View, xn::mat33 const &T_View_Screen) = 0;

  virtual void MouseMove(uint32_t modState, xn::vec2 const &) = 0;
  virtual void MouseDown(uint32_t modState, xn::vec2 const &) = 0;
  virtual void MouseUp(uint32_t modState, xn::vec2 const &) = 0;

  virtual void DrawBackSprites(Renderer *) = 0;
  virtual void DrawFrontSprites(Renderer *) = 0;

  virtual void Undo() = 0;
  virtual void Redo() = 0;
};

IProjectController *CreateProjectController(Project *);

#endif