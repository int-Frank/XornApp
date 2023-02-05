
#include <vector>
#include "ActionList.h"

struct ActionItem
{
  ActionID ID;
  Action *pAction;
};

class ActionList : public IActionList
{
public:

  ~ActionList();
  ActionList();

  ActionID PushBack(Action *, ActionData *) override;
  void Redo(ActionData *) override;
  void Undo(ActionData *) override;
  Action *GetAction(ActionID) const override;
  void Clear() override;

private:

  ActionID m_nextID;
  int m_index; // Index of the last completed action
  std::vector<ActionItem> m_actions;
};

ActionList::~ActionList()
{
  Clear();
}

ActionList::ActionList()
  : m_nextID(0)
  , m_index(-1)
{

}

void ActionList::Undo(ActionData *pData)
{
  if (m_index > 0)
  {
    m_actions[m_index].pAction->Undo(pData);
    m_index--;
  }
}

void ActionList::Redo(ActionData *pData)
{
  if (m_index + 1 < (int)m_actions.size())
  {
    m_index++;
    m_actions[m_index].pAction->Do(pData);
  }
}

ActionID ActionList::PushBack(Action *pAction, ActionData *pData)
{
  while (m_index + 1 < (int)m_actions.size())
  {
    delete m_actions.back().pAction;
    m_actions.pop_back();
  }

  ActionItem action = {};
  action.ID = m_nextID++;
  action.pAction = pAction;

  m_actions.push_back(action);
  m_actions.back().pAction->Do(pData);
  return action.ID;
}

void ActionList::Clear()
{
  for (auto action : m_actions)
    delete action.pAction;
  m_actions.clear();
  m_index = -1;
}

Action *ActionList::GetAction(ActionID id) const
{
  size_t low = 0;
  size_t high = m_actions.size();
  while (low <= high)
  {
    size_t mid = low + (high - low) / 2;

    if (m_actions[mid].ID == id)
      return m_actions[mid].pAction;

    if (m_actions[mid].ID < id)
      low = mid + 1;

    else
      high = mid - 1;
  }

  return nullptr;
}

IActionList *CreateActionList()
{
  return new ActionList();
}