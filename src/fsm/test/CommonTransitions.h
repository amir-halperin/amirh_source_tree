#pragma once

#include <fsm/Transition.h>
#include "StateStart.h"

class TransitionActions;

// common transition used all states.
// in cases where event breaks a sequence we need a transition to stStart to initiate a new sequence.
class trError : public fsm::Transition<trError>
{
public:
  trError(TransitionActions& actions) : Transition("trError"), mActions(actions) {}
  virtual fsm::StateBase* NextState() const { return new stStart(mActions);}
  virtual fsm::TransitionBase* Clone() const {return new trError(*this);}

private:

  TransitionActions& mActions;
};