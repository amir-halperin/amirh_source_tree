#include"StateStart.h"
#include"StateA.h"

fsm::StateBase* trStartToA::NextState() const {return new stA(mActions);}