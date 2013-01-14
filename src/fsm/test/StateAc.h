#pragma once

#include <iostream>
#include <fsm/State.h>
#include <fsm/Transition.h>
#include "Events.h"
#include "TransitionActions.h"

class trAcToStart : public fsm::Transition<trAcToStart>
{
public:
  trAcToStart(TransitionActions& actions) : Transition("trAcToStart"), mActions(actions) {}
  
  virtual void ActivateAction(const fsm::EventBase& e) const {mActions.Sequence3(e);}
  virtual fsm::StateBase* NextState() const;

private:

  TransitionActions& mActions;
};

class stAc : public fsm::State<stAc>
{
public:

  stAc(TransitionActions& actions) : State("stAc")
  {
    CreateTransition(evA().Type(), new trAcToStart(actions));
    CreateTransition(evB().Type(), new trAcToStart(actions));
    CreateTransition(evC().Type(), new trAcToStart(actions));
  }
};