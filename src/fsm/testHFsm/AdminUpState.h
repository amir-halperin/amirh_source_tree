#pragma once

#include <fsm/State.h>
#include <fsm/Transition.h>
#include <fsm/Machine.h>

#include "Events.h"
#include "DownState.h"
#include "TransitionActions.h"

class AdminUpToDownTransition : public fsm::Transition<AdminUpToDownTransition>
{
public:

  AdminUpToDownTransition(TransitionActions& actions) : Transition("AdminUpToDownTransition"), mActions(actions) {}
  virtual void ActivateAction(const fsm::EventBase& e) const {mActions.DeActivated(e);}
  virtual fsm::StateBase* NextState() const;

private:

  TransitionActions& mActions;
};

// A composite state i.e. a state that is also a state machine.
class AdminUpState : public fsm::CompositeState<AdminUpState, DownState, TransitionActions>
{
public:

  AdminUpState(TransitionActions& actions) : CompositeState("AdminUpState", actions)
  {
    CreateTransition(AdminDownEvent().Type(), new AdminUpToDownTransition(actions));
  }
};