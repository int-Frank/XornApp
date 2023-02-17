#ifndef ACTION_H
#define ACTION_H

class Project;

struct ActionData
{
  ActionData(Project *pProj)
    : pProject(pProj)
  {

  }

  Project *pProject;
};

class Action
{
public:

  Action(ActionData const &data)
    : m_actionData(data)
  {

  }

  virtual ~Action() {}
  virtual bool Do() = 0;
  virtual bool Undo() = 0;

protected:
  ActionData m_actionData;
};

#endif