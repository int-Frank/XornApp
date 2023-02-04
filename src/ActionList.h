#ifndef ACTIONLIST_H
#define ACTIONLIST_H

#include <vector>
#include "Action.h"

typedef uint32_t ActionID;

class ActionList
{
public:

  ActionList();

  void PushBack(Action *, ActionData *);
  void PopBack();
  void Redo(ActionData *);
  void Undo(ActionData *);

  void Clear();

private:

  int m_index; // Index of the last completed action
  std::vector<Action *> m_actions;
};

#endif