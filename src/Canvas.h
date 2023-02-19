#ifndef CANVAS_H
#define CANVAS_H

#include <string>

#include "xnGeometry.h"

class MessageBus;
class Message;

class Canvas
{
public:

  Canvas(std::string const &name, MessageBus *);
  ~Canvas();

  void SetSize(xn::vec2 const &);
  xn::vec2 GetRenderRegionSize() const; // Slightly less than the window size

  xn::mat33 GetMatrix_Screen_View() const;

  void BeginFrame();
  void BlitImage(void *, uint32_t w, uint32_t h);
  void EndFrame();

  void Handle(Message *);

private:

  class PIMPL;
  PIMPL *m_pimpl;
};

#endif