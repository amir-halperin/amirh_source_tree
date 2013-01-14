#include "AdminDownState.h"
#include "AdminUpState.h"
#include "iostream"

fsm::StateBase* AdminDownToUpTransition::NextState() const {return new AdminUpState(mActions);}