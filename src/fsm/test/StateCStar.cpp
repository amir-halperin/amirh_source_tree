#include"StateCStar.h"
#include"StateStart.h"

// re-implement Transition defined in base State due-to we need 
// to decide between two transitions based on a guard i.e. mCounter < 1000
const fsm::TransitionBase* stCStar::GetCustomTransition(const fsm::EventBase& e)
{
fsm::TransitionBase* t=0;

  // in case of event evC we have two transition options to decide from
  if( e == evC() )
  {
    ++mNumOfReceived_C_Events;
    // retrieve the right transition based on received event and guard value
    if(mNumOfReceived_C_Events < 1000)
    {
      // retrieve the internal transition
      t = GetSimpleTransition(evC_GuardLess1000());
      // runtime down cast check verification
      if(*t == trCStarToCStar(mActions))
      {
        // Even this is an internal transition, the state is deleted and re-created.
        // Thus we need to preserve the current guard value and forward it via 
        // the transition object to the new state. 
        // The new state is going to be created by the transition. 
        static_cast<trCStarToCStar*>(t)->Guard(mNumOfReceived_C_Events);  
      }
      else // if down cast fail it means that the FSM is defined incorrect
      {
        // TODO: throw exception
        std::cout << "Exception: failed to static_cast from Transition* to trCStarToCStar*\n";
      }
    }
    else
    {
      // retrieve the external transition to move on the next state
      t = GetSimpleTransition(evC_GuardEqual1000());
    }
  }

  return t;
}

// Internal transition i.e. same class but different objects!
// Need to preserve the guard state that is being stored in the Transition object.
fsm::StateBase* trCStarToCStar::NextState() const 
{
  return new stCStar(mActions, mGuard);
}