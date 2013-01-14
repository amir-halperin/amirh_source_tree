#pragma once

#include <fsm/State.h>
#include <fsm/Transition.h>
#include "Events.h"
#include "CommonTransitions.h"

class trAaToAab : public fsm::Transition<trAaToAab>
{
public:
  trAaToAab(TransitionActions& actions) : Transition("trAaToAab"), mActions(actions) {}
  virtual const char* Type() const { return "trAaToAab"; }
  virtual fsm::StateBase* NextState() const;

private:

  TransitionActions& mActions;
};

class stAa : public fsm::State<stAa>
{
public:

  stAa(TransitionActions& actions) : State("stAa")
  {
    CreateTransition(evC().Type(), new trError(actions));
    CreateTransition(evA().Type(), new trError(actions));
    CreateTransition(evB().Type(), new trAaToAab(actions));
  }
};