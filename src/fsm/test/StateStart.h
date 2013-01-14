#pragma once

#include <fsm/State.h>
#include <fsm/Transition.h>

#include "Events.h"
#include "TransitionActions.h"

class trStartToA : public fsm::Transition<trStartToA>
{
public:
  trStartToA(TransitionActions& actions) : Transition("trStartToA"), mActions(actions) {}
  virtual fsm::StateBase* NextState() const;

private:

  TransitionActions& mActions;
};

class stStart : public fsm::State<stStart>
{
public:

  // state ctor responsible to build its local transition table
  stStart(TransitionActions& actions) : State("stStart")
  {
    // each applicable event is mapped to a specific transition object
    CreateTransition(evA().Type(), new trStartToA(actions));
  }
};