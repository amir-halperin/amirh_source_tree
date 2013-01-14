#pragma once

#include <iostream>
#include <fsm/State.h>
#include <fsm/Transition.h>

#include "Events.h"
#include "CommonTransitions.h"
#include "TransitionActions.h"

class trCToStart : public fsm::Transition<trCToStart>
{
public:

  trCToStart(TransitionActions& actions) : Transition("trCToStart"), mActions(actions) {}
  
  virtual void ActivateAction(const fsm::EventBase& e) const {mActions.Sequence1(e);}
  virtual fsm::StateBase* NextState() const;

private:

  TransitionActions& mActions;
};

class stC : public fsm::State<stC>
{
public:

  stC(TransitionActions& actions) : State("trCToStart")
  {
    CreateTransition(evA().Type(), new trCToStart(actions));
    CreateTransition(evB().Type(), new trError(actions));
    CreateTransition(evC().Type(), new trError(actions));
  }
};