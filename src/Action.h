#ifndef ACTION_H
#define ACTION_H

class Project;

struct ActionData
{
  Project *pProj;
};

class Action
{
public:

  virtual ~Action() {}
  virtual bool Do(ActionData *) = 0;
  virtual bool Undo(ActionData *) = 0;
};

#endif