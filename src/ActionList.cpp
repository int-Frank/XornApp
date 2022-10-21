#include "ActionList.h"

ActionList::ActionList()
  : m_index(-1)
{

}

void ActionList::Undo(ActionData *pData)
{
  if (m_index > 0)
  {
    m_actions[m_index]->Undo(pData);
    m_index--;
  }
}

void ActionList::Redo(ActionData *pData)
{
  if (m_index + 1 < (int)m_actions.size())
  {
    m_index++;
    m_actions[m_index]->Do(pData);
  }
}

void ActionList::PushBack(Action *pAction, ActionData *pData)
{
  while (m_index + 1 < (int)m_actions.size())
  {
    delete m_actions.back();
    m_actions.pop_back();
  }

  m_actions.push_back(pAction);
  m_actions.back()->Do(pData);
}