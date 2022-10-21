#ifndef XORNAPPMESSAGES_H
#define XORNAPPMESSAGES_H

#include "DgVector.h"
#include "Message.h"

enum class MessageType
{
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