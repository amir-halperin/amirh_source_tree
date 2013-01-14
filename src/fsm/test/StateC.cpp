#include"StateC.h"
#include"StateStart.h"

fsm::StateBase* trCToStart::NextState() const {return new stStart(mActions);}