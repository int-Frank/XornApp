#ifndef ACTIONLIST_H
#define ACTIONLIST_H

#include "Action.h"

typedef uint32_t ActionID;

class IActionList
{
public:

  virtual ~IActionList() {};
  virtual ActionID PushBack(Action *) = 0;
  virtual void Redo() = 0;
  virtual void Undo() = 0;
  virtual Action *GetAction(ActionID) const = 0;
  virtual void Clear() = 0;
};

IActionList *CreateActionList();

#endif