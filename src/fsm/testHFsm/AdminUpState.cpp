#include "AdminUpState.h"
#include "AdminDownState.h"

fsm::StateBase* AdminUpToDownTransition::NextState() const {return new AdminDownState(mActions);} 