#pragma once

#include <fsm/Event.h>

class TransitionActions
{
public:

  virtual void Activated(const fsm::EventBase& e) {}
  virtual void DeActivated(const fsm::EventBase& e) {}
  virtual void PortChangeStateToUp(const fsm::EventBase& e) {}
  virtual void PortChangeStateToDown(const fsm::EventBase& e) {}
};

class TransitionActionsImpl : public TransitionActions
{
public:

};