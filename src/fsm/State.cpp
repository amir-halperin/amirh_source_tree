#include <fsm/State.h>
#include <fsm/Event.h>
#include <fsm/Transition.h>
#include <iostream>

namespace fsm {

StateBase::~StateBase()
{
  EventTransitionMapIteratorType i;

  for(i=mTransitionsTable.begin(); i!=mTransitionsTable.end(); ++i)
  {
    delete (TransitionBase*)i->second;
  }
}

TransitionBase* StateBase::GetSimpleTransition(const EventBase& e)
{
  EventTransitionMapIteratorType i = mTransitionsTable.find(e.Type());
  
  if( i != mTransitionsTable.end() )
  {
    return (TransitionBase*)i->second;
  }

  return 0;
}

void StateBase::CreateTransition( std::string s, TransitionBase* t )
{
  if(!t)
  {
    // TODO: throw exception
    std::cout << "unable to create transition. NULL transition object. Exception!!!\n";
    return;
  }

  EventTransitionPairType p(s,t);
  std::pair<EventTransitionMapIteratorType, bool> retVal = mTransitionsTable.insert(p);

  if (retVal.second==false)
  {
    // TODO: throw exception
    std::cout << "transition in state " << Type() 
              << " that accept event " << s 
              << " and provide Transition " << t->Type() 
              << " is already exist. Exception!!!\n";
  }
}

}; // namespace fsm