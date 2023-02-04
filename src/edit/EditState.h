#ifndef EDITSTATE_H
#define EDITSTATE_H

#include "xnGeometry.h"

class EditState
{
public:

  EditState *MouseMove(xn::vec2 const &);
  EditState *MouseDown(xn::vec2 const &);
  EditState *MouseUp(xn::vec2 const &);

private:
};

class EditStateMoveCamera : public EditState
{

};

class EditStateSelectVertex : public EditState
{

};

class EditStateMoveVertex : public EditState
{

};

class EditStateSelectLoops : public EditState
{

};

class EditStateMoveLoops : public EditState
{

};

class EditStateRotateLoops : public EditState
{

};

#endif