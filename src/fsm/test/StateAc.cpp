#include"StateAc.h"
#include"StateStart.h"

fsm::StateBase* trAcToStart::NextState() const {return new stStart(mActions);}