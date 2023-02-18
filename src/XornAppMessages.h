#ifndef XORNAPPMESSAGES_H
#define XORNAPPMESSAGES_H

#include "DgVector.h"
#include "Message.h"
#include "xnCommon.h"

enum class MessageType
{
  MouseScroll,
  MouseButtonDown,
  MouseButtonUp,
  MouseMove,

  ZoomCamera,
  Undo,
  Redo
};

MESSAGE_HEADER(MouseButtonDown)
  xn::MouseInput button;
  Dg::Vector2<float> position;
};

MESSAGE_HEADER(MouseButtonUp)
  xn::MouseInput button;
  Dg::Vector2<float> position;
};

MESSAGE_HEADER(MouseMove)
  Dg::Vector2<float> position;
};

MESSAGE_HEADER(MouseScroll)
  float val;
};

MESSAGE_HEADER(ZoomCamera)
  float val;
};

MESSAGE_HEADER(Undo)
  int dummy;
};

MESSAGE_HEADER(Redo)
  int dummy;
};

#endif