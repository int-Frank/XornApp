#ifndef CANVAS_H
#define CANVAS_H

#include <string>

#include "xnGeometry.h"

namespace xn
{
  class MessageBus;
  class Renderer;
  class Message;
}

class Canvas
{
public:

  Canvas(std::string const &name, xn::Renderer *, xn::MessageBus *);
  ~Canvas();

  void SetPosition(xn::vec2 const &);
  void SetSize(xn::vec2 const &);

  xn::mat33 Get_T_Camera_View() const;
  xn::Renderer *GetRenderer() const;
  void BeginFrame();
  void EndFrame();
  void Handle(xn::Message *);

private:

  static float const s_minCanvasSize;

  std::string m_name;
  xn::MessageBus *m_pMsgBus;
  xn::Renderer *m_pRenderer;
  xn::vec2 m_size;
  xn::vec2 m_position;
  float m_scroll;
};

#endif