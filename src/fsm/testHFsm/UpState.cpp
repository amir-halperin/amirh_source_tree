#include "UpState.h"
#include "DownState.h"

fsm::StateBase* UpToDownTransition::NextState() const
{
  return new DownState(mActions);
}