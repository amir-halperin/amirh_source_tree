#pragma once

#include <fsm/State.h>
#include <fsm/Transition.h>
#include "Events.h"
#include "TransitionActions.h"

class AdminDownToUpTransition : public fsm::Transition<AdminDownToUpTransition>
{
public:

  AdminDownToUpTransition(TransitionActions& actions) : Transition("AdminDownToUpTransition"), mActions(actions) {}
  virtual void ActivateAction(const fsm::EventBase& e) const {mActions.Activated(e);}
  virtual fsm::StateBase* NextState() const;

private:

  TransitionActions& mActions;
};

class AdminDownState : public fsm::State<AdminDownState>
{
public:

  AdminDownState(TransitionActions& actions) : State("AdminDownState")
  {
    CreateTransition( AdminUpEvent().Type(), new AdminDownToUpTransition(actions));
  }
};