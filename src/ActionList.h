#ifndef ACTIONLIST_H
#define ACTIONLIST_H

#include "Action.h"

typedef uint32_t ActionID;
#define INVALID_ACTION_ID 0xFFFFFFFFul

class IActionList
{
public:

  virtual ~IActionList() {};
  virtual ActionID AddAndExecute(Action *) = 0;
  virtual ActionID TopID() const = 0;
  virtual void Redo() = 0;
  virtual void Undo() = 0;
  virtual void Clear() = 0;
};

IActionList *CreateActionList();

#endif