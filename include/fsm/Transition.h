#pragma once

#include <string>

namespace fsm {

class StateBase;
class EventBase;

// Abstract class that define a state machine transition. Designed to be used by Machine class.
// Provide interface to create next state object, and to activate optional transition's action.
// Must be derived by a specific transition class that is closely related to a specific state.
class TransitionBase
{
public:

  virtual ~TransitionBase() {}
  // mandatory to be implemented by derived to define unique transition type.
  virtual const char* Type() const =0;
  // mandatory to be implemented by derived to create the next state based on specific transition.
  // A transition object must be unique according to the combination of action and next state.
  virtual StateBase* NextState() const =0;
  // Optional action that could be triggered during the transition.
  virtual void ActivateAction(const EventBase& e) const {}
  // mandatory to be implemented by derived
  // purpose: generally Transition is owned by the State class. However during state machine transition,
  // it is required to delete source i.e. current; state, to trigger the action defined in the transition, and
  // to move on to the next state. Due to the fact Transition objects are deleted by the source State before the 
  // Transition completed his job (i.e. trigger the ActivationAction, and provide the NextState), it is required 
  // to duplicate it, so the source State can be safely deleted. The duplication is required by State base class
  // which doesn't know specific type of the Transition, so therefore the Transition object should do duplication job.
  virtual TransitionBase* Clone() const =0;

  bool operator!=(const TransitionBase& other) const;
  bool operator==(const TransitionBase& other) const;
};

template<typename DERIVED_TYPE>
class Transition : public fsm::TransitionBase
{
public:
  Transition(std::string type) : mType(type) {}

  virtual const char* Type() const { return mType.c_str(); }

  // must be re-implemented to allow generic ProcessEvent method in machine to copy transition object during transition implementation.
  virtual fsm::TransitionBase* Clone() const { return new DERIVED_TYPE( static_cast<DERIVED_TYPE const&>(*this) ); }

private:

  std::string mType;
};

}; // namespace fsm_fw