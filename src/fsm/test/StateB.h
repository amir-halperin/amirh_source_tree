#pragma once

#include <fsm/State.h>
#include <fsm/Transition.h>
#include "Events.h"
#include "CommonTransitions.h"

class trBToCStar : public fsm::Transition<trBToCStar>
{
public:
  trBToCStar(TransitionActions& actions) : Transition("trBToCStar"), mActions(actions) {}
  virtual fsm::StateBase* NextState() const;

private:

  TransitionActions& mActions;
};

class stB : public fsm::State<stB>
{
public:

  stB(TransitionActions& actions) : State("stB")
  {
    CreateTransition(evC().Type(), new trBToCStar(actions));
    CreateTransition(evB().Type(), new trError(actions));
    CreateTransition(evA().Type(), new trError(actions));
  }
};