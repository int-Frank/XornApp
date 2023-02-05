#ifndef EDITSTATE_H
#define EDITSTATE_H

#include "xnCommon.h"
#include "xnGeometry.h"
#include "Project.h"

class EditState
{
public:

  virtual EditState *MouseMove(xn::vec2 const &) = 0;
  virtual EditState *MouseDown(xn::MouseInput, xn::vec2 const &) = 0;
  virtual EditState *MouseUp(xn::MouseInput, xn::vec2 const &) = 0;

private:
};

class EditStateIdle : public EditState
{
public:

private:

  std::vector<PolygonID> m_selectedPolygons;
};

class EditStateDrag : public EditState
{

};

class EditStateMoveVertex : public EditState
{
public:

private:
  PolygonID m_polygon;
  uint32_t m_vertexIndex;
};

class EditStateMoveLoops : public EditState
{
public:

private:

  std::vector<PolygonID> m_selectedPolygons;
};

class EditStateRotateLoops : public EditState
{
public:

private:

  std::vector<PolygonID> m_selectedPolygons;
};

#endif