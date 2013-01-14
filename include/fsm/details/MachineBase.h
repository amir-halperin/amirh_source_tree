#pragma once

#include <iostream>

#include <fsm/details/EventsProcessor.h>

namespace fsm {

class StateBase;
class StateFactory;
class TransitionBase;
class EventBase;

// Provide State machine base logic.
// Designed to use abstract State and Transition objects to implement the heart of the machine.
// Implemented as a template class, so for starting using it the initial state must be defined as
// template parameter.
// Machine class use Event, State, and Transition abstract classes to implement main FSM logic.
//
// Limitation (or better to say, future enhancements):
// - missing support in multi-threading.
// - missing support in support communication between multiple machines executing in context of separate threads.
// - missing support in hierarchical states machines.
//
// To use this class:
// 1. create instance and provide the initial state.
// 2. Inject Event by calling ProcessEvent method.
//
// ProcessEvent uses Event, State, and Transition abstract classes to implement the state machine logic.
//
class MachineBase : public EventsProcessor
{
public:
  // default ctor is used for serial loading, or for DummyMachine
  MachineBase() : mFsmId(0), mCurrentState(0) {}
  // the fsmId set init state
  MachineBase(int fsmId, StateBase* initialState) : mFsmId(fsmId), mCurrentState(initialState) {}

  // implements EventsProcessor type
  // inject external event to trigger state machine transition
  virtual void ProcessEvent(const EventBase& e);

private:

  int mFsmId;
  StateBase* mCurrentState;

  friend class MachineSerializer;
};

}; // namespace fsm