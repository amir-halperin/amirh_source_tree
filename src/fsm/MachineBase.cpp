#include <fsm/State.h>
#include <fsm/Transition.h>
#include <fsm/Event.h>
#include <fsm/StateFactory.h>
#include <fsm/details/MachineBase.h>


namespace fsm {

// trace for internal testing
//#define DEBUG_LOGS

void MachineBase::ProcessEvent(const EventBase& e)
{
  // check first if the state have a customized transition
  const TransitionBase* t = mCurrentState->GetCustomTransition(e);
  // if no customized transition was found look for simple transition
  if(!t) t = mCurrentState->GetSimpleTransition(e);

  if(t) // if transition was found
  {
#ifdef DEBUG_LOGS
    std::cout << " current:" << mCurrentState->Type() << " event:" << e.Type() << "\n";
#endif
    // Transition objects are deleted during source State deletion 
    // thus we duplicates the relevant Transition before deleting the source State.
    TransitionBase* copied_t = t->Clone();

    // activate exit logic on current state
    mCurrentState->OnExit();
    // we finished with source state
    delete mCurrentState;

    // use the Transition
    copied_t->ActivateAction(e); // activate actions defined on the transition
    // activate entry logic on new state
    mCurrentState = copied_t->NextState();
    // we finished with the transition
    delete copied_t;

#ifdef DEBUG_LOGS
    std::cout << " next:" << mCurrentState->Type() << "\n";
#endif
    // activate new state default operations
    mCurrentState->OnEnter();
  }// if transition was found
  else // if no transition found forward event into state for ineer processesing
  {
    mCurrentState->ProcessEvent(e); // each state might be also a composite state
  }
}

}; // namespace fsm