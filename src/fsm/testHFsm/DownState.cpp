#include "DownState.h"
#include "UpState.h"

fsm::StateBase* DownToUpTransition::NextState() const
{
  return new UpState(mActions);
}