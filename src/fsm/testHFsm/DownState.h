#pragma once

#include <fsm/State.h>
#include <fsm/Transition.h>
#include "Events.h"
#include "TransitionActions.h"

class TransitionActions;

class DownToUpTransition : public fsm::Transition<DownToUpTransition>
{
public:

  DownToUpTransition(TransitionActions& actions) : Transition("DownToUpTransition"), mActions(actions) {}
  virtual void ActivateAction(const fsm::EventBase& e) const {mActions.PortChangeStateToUp(e);}
  virtual fsm::StateBase* NextState() const;

private:

  TransitionActions& mActions;
};

class DownState : public fsm::State<DownState>
{
public:
  DownState(TransitionActions& actions) : State("DownState") 
  {
    CreateTransition(UpEvent().Type(), new DownToUpTransition(actions));
  }
};