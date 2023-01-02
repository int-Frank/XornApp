#ifndef CANVAS_H
#define CANVAS_H

#include <string>

#include "xnGeometry.h"
#include "xnIScene.h"

class MessageBus;
class Message;
class IRenderer;

class Canvas
{
public:

  Canvas(std::string const &name, MessageBus *, IRenderer *);
  ~Canvas();

  void SetPosition(xn::vec2 const &);
  void SetSize(xn::vec2 const &);

  IRenderer * GetRenderer();
  xn::mat33 Get_T_Camera_View() const;
  void BeginFrame();
  void EndFrame();

  void Handle(Message *);

private:

  class PIMPL;
  PIMPL *m_pimpl;
};

#endif