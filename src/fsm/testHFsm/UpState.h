#pragma once

#include <fsm/State.h>
#include <fsm/Transition.h>
#include "Events.h"
#include "TransitionActions.h"

class UpToDownTransition : public fsm::Transition<UpToDownTransition>
{
public:

  UpToDownTransition(TransitionActions& actions) : Transition("UpToDownTransition"), mActions(actions) {}
  virtual void ActivateAction(const fsm::EventBase& e) const {mActions.PortChangeStateToDown(e);}
  virtual fsm::StateBase* NextState() const;

private:

  TransitionActions& mActions;
};

class UpState : public fsm::State<UpState>
{
public:
  UpState(TransitionActions& actions) : State("UpState") 
  {
    CreateTransition(DownEvent().Type(), new UpToDownTransition(actions));
  }
};