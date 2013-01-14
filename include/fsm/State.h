#pragma once

#include <map>
#include <string>
#include <fsm/details/Serializable.h>
#include <fsm/details/EventsProcessor.h>
#include <fsm/Machine.h>

namespace fsm {

class EventBase;
class TransitionBase;

// Abstract base state class. Provide abstract interface to be used by Machine class.
// Responsible to triggers actions on state entry and on state exist.
// Maintains internal transitions table directed from this state.
// Provides methods to get next transition based on given event.
// Must be derived by a specific state, where the specific is responsible for:
// - unique state type definition.
// - transition table initialization during state construction.
// - re implement template methods, OnEnter, and OnExist as necessary.
// - re implement template method GetCustomTransition in case of multiple 
//   transition triggered via same event and internal guard evaluation.
//
// State is a Serializable object, which means its data can be saved and load from a stream.
// Derived state is required to re-implement Serializable Load and Save methods only in cases
// where additional data members exist, and required to be restored.
//
class StateBase : public Serializable, public EventsProcessor
{
public:

  virtual ~StateBase();

  // Optional operation that should be activated each time this state is entered.
  // Called both when a transition from other state into this state is occur, and 
  // an internal transition from this state to this state is occur.
  virtual void OnEnter(){}
  // Optional operation that should be activated each time this state is exited.
  // Called both on transition to an external state, and in case of internal transition.
  virtual void OnExit(){}
  
  // Return simple transition i.e. transition that is triggered only by received event
  // Return NULL is no transition match to received event is found
  TransitionBase* GetSimpleTransition(const EventBase& e);
  // Optional custom transition to be reimplemented by derived state in case there are more than one transition for a specific event.
  // The custom transition reevaluate a received event, and in addition to an internal guard state is determined on a new internal event.
  // The new internal event is one to one correlated to a specific transition that is stored in the transitions table.
  virtual const TransitionBase* GetCustomTransition(const EventBase& e) {return 0;}
  // Defined by derived state to provide a state type.
  virtual const char* Type() const =0;
  
  // Polymorphic ctor. Used by StateFactory
  virtual StateBase* Create() const =0;

  // Every state have the potential to be a composite state and therefore need to provide some sort of ProcessEvent method.
  // Default implementation is empty, while in case the derive state is also a composite, it should reimplement to forward the event
  // to a real machine for further processing.
  virtual void ProcessEvent(const EventBase& e) {}

protected:

  // Insert new transition into the state's transitions table.
  // Must be used by derived state during state creation.
  // Transition table init must be complete before any event is sent to the state.
  void CreateTransition(std::string s, TransitionBase* t);

private:

  typedef std::map<std::string, TransitionBase*> EventTransitionMapType;
  typedef std::pair<std::string, TransitionBase*> EventTransitionPairType;
  typedef std::map<std::string, TransitionBase*>::iterator EventTransitionMapIteratorType;

  // hold transition objects originating from this state.
  // Each transition is mapped to a specific event.
  EventTransitionMapType mTransitionsTable;
};

template<typename DERIVED_TYPE>
class State : public StateBase
{
public:

  State(std::string type) : mType(type) {}
  virtual const char* Type() const {return mType.c_str();}
  virtual StateBase* Create() const {return new DERIVED_TYPE(static_cast<DERIVED_TYPE const&>(*this));}

private:

  std::string mType;
};

template<typename DERIVED_TYPE, typename INNER_STATE_TYPE, typename ACTIONS_TYPE>
class CompositeState : public State<DERIVED_TYPE>
{
public:

  CompositeState(std::string type, ACTIONS_TYPE& actions) : State(type), mStateMachine(actions) {}

  // This is a composite state, and threfore requires to re-implements EventsProcessor type
  // for injecting events to a composite state machine.
  virtual void ProcessEvent(const EventBase& e) {mStateMachine.ProcessEvent(e);}

private:

  Machine<INNER_STATE_TYPE, ACTIONS_TYPE> mStateMachine;
};

}; //namespace fsm