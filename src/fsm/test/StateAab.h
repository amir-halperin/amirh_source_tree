#pragma once

#include <fsm/State.h>
#include <fsm/Transition.h>
#include "Events.h"
#include "CommonTransitions.h"
#include "StateC.h"

class TransitionActions;

class trAabToC : public fsm::Transition<trAabToC>
{
public:
  trAabToC(TransitionActions& actions) : Transition("trAabToC"), mActions(actions) {}
  virtual fsm::StateBase* NextState() const { return new stC(mActions);}

private:

  TransitionActions& mActions;
};

class stAab : public fsm::State<stAab>
{
public:

  stAab(TransitionActions& actions) : State("stAab")
  {
    CreateTransition(evC().Type(), new trAabToC(actions));
    CreateTransition(evA().Type(), new trError(actions));
    CreateTransition(evB().Type(), new trError(actions));
  }
};