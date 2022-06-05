#ifndef XORNAPPMESSAGES_H
#define XORNAPPMESSAGES_H

#include "DgVector.h"
#include "xnMessage.h"

enum class MessageType
{
  XORNAPP_START = (uint32_t)xn::MessageType::XORN_END,

  MouseScroll,
  ZoomCamera,
  MoveCamera,
};

MESSAGE_HEADER(MouseScroll)
  float val;
};

MESSAGE_HEADER(ZoomCamera)
  float val;
};

MESSAGE_HEADER(MoveCamera)
  Dg::Vector2<float> v;
};

#endif