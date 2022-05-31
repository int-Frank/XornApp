#ifndef APPEVENTS_H
#define APPEVENTS_H

class App;

class Event
{
public:

  virtual void operator()(App *) = 0;
};

class EventScroll : Event
{
public:

  void operator()(App *) override;

private:
  float m_scroll;
};

class EventEditMode : Event
{
public:

  void operator()(App *) override;

private:
  bool m_on;
};

#endif