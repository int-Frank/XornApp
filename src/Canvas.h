#ifndef CANVAS_H
#define CANVAS_H

#include <string>

#include "xnGeometry.h"

class MessageBus;
class Message;

namespace xn
{
  class Renderer;
}

class Canvas
{
public:

  Canvas(std::string const &name, MessageBus *, xn::Renderer *pRenderer);
  ~Canvas();

  void SetPosition(xn::vec2 const &);
  void SetSize(xn::vec2 const &);

  xn::mat33 Get_T_Camera_View() const;
  xn::Renderer *GetRenderer();
  void BeginFrame();
  void EndFrame();

  void Handle(Message *);

private:

  class PIMPL;
  PIMPL *m_pimpl;
};

#endif