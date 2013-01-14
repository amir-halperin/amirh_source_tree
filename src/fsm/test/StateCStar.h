#pragma once

#include <iostream>

#include <fsm/State.h>
#include <fsm/Transition.h>
#include "Events.h"
#include "CommonTransitions.h"
#include "StateStart.h"

// an internal event that is used as a mix between external evC event and guard < 1000 value 
class evC_GuardLess1000: public fsm::Event<evC_GuardLess1000>
{
public:

  evC_GuardLess1000() : Event("evC_GuardLess1000") {}
};

// an internal event that is used as a mix between external evC event and guard == 1000 value 
class evC_GuardEqual1000: public fsm::Event<evC_GuardEqual1000>
{
public:

  evC_GuardEqual1000() : Event("evC_GuardEqual1000") {}
};

class trCStarToCStar : public fsm::Transition<trCStarToCStar>
{
public:

  trCStarToCStar(TransitionActions& actions) : Transition("trCStarToCStar"), mActions(actions), mGuard(1) {}
  
  virtual fsm::StateBase* NextState() const;
  
  // Replace default implementation that implements Clone by using default constructor.
  // Due to the fact this Transition have data, copy ctor must by used to satisfy the clone operation!
  virtual fsm::TransitionBase* Clone() const {return new trCStarToCStar(*this);}

  void Guard(int g) {mGuard = g;}

private:

  TransitionActions& mActions;
  int mGuard; // store the guard value of the prev state to be forward to the next state
};

class trCStarToStart : public fsm::Transition<trCStarToStart>
{
public:
  trCStarToStart(TransitionActions& actions) : Transition("trCStarToStart"), mActions(actions) {}
  
  virtual void ActivateAction(const fsm::EventBase& e) const {mActions.Sequence2(e);}
  virtual fsm::StateBase* NextState() const {return new stStart(mActions);}

private:

  TransitionActions& mActions;
};

// Example for more complex state.
// This state defines two transitions that are triggered on the same event.
// To decide which transition is required to fire, an additional data member is maintained
// in this state, and it used as a guard.
// To implement this special transition, the state re-implements GetCustomTransition method.
// 
// In addition, this state is required to re-implement Serializable methods, hence the guard
// is additional data that must be recovered during persistent initialization.
class stCStar : public fsm::State<stCStar>
{
public:

  // used when this state is entered due to transition from another state
  stCStar(TransitionActions& actions) : State("stCStar"), mNumOfReceived_C_Events(1), mActions(actions) {Init();}
  // used when in case of internal transition i.e. from stCStar to stCStar
  stCStar(TransitionActions& actions, int c) : State("stCStar"), mActions(actions), mNumOfReceived_C_Events(c) {Init();}
  // Init is used to prevent code duplication in ctors
  void Init()
  {
    CreateTransition( evA().Type(), new trError(mActions) );
    CreateTransition( evB().Type(), new trError(mActions) );
    CreateTransition( evC_GuardLess1000().Type(), new trCStarToCStar(mActions) );
    CreateTransition( evC_GuardEqual1000().Type(), new trCStarToStart(mActions) );
  }

  // Custom transition definition. 
  // We need to decide between two transitions based on a guard value i.e. value of mNumOfReceived_C_Events.
  // Retrieve one of two transitions in case the received event is evC.
  // Return NULL in case received event is not evC.
  virtual const fsm::TransitionBase* GetCustomTransition(const fsm::EventBase& e);

  // Re-implement Serializable methods to serializing internal data
  virtual void Save(std::ostream& os) {StateBase::Save(os); os<<mNumOfReceived_C_Events;}
  virtual bool Load(std::istream& is) {bool ret = StateBase::Load(is); is>>mNumOfReceived_C_Events; return ret;}

private:

  TransitionActions& mActions;
  // counter that counts how many time evC is received in this state
  // used as a guard to decide between internal and trCStarToStart transitions
  int mNumOfReceived_C_Events; 
};