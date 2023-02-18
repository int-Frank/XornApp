
#include <vector>
#include "ActionList.h"

struct ActionItem
{
  ActionItem(ActionID _id, Action *_pAction)
    : id(_id)
    , pAction(_pAction)
  {

  }

  ActionID id;
  Action *pAction;
};

class ActionList : public IActionList
{
public:

  ~ActionList();
  ActionList();

  ActionID AddAndExecute(Action *) override;
  ActionID TopID() const override;
  void Redo() override;
  void Undo() override;
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
  , m_actions()
{

}

void ActionList::Undo()
{
  if (m_index > 0)
  {
    m_actions[m_index].pAction->Undo();
    m_index--;
  }
}

void ActionList::Redo()
{
  if (m_index + 1 < (int)m_actions.size())
  {
    m_index++;
    m_actions[m_index].pAction->Do();
  }
}

ActionID ActionList::AddAndExecute(Action *pAction)
{
  while (m_index + 1 < (int)m_actions.size())
  {
    delete m_actions.back().pAction;
    m_actions.pop_back();
  }

  auto id = m_nextID++;
  m_actions.push_back(ActionItem(id, pAction));
  m_actions.back().pAction->Do();
  m_index++;
  return id;
}

ActionID ActionList::TopID() const
{
  if (m_index >= 0)
    return m_actions[m_index].id;
  return INVALID_ACTION_ID;
}

void ActionList::Clear()
{
  for (auto action : m_actions)
    delete action.pAction;
  m_actions.clear();
  m_index = -1;
}

IActionList *CreateActionList()
{
  return new ActionList();
}