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

  static float const s_minCanvasSize;

  std::string m_name;
  MessageBus *m_pMsgBus;
  xn::Renderer *m_pRenderer;
  xn::vec2 m_size;
  xn::vec2 m_position;
  float m_scroll;
};

#endif