#include"StateAa.h"
#include"StateAab.h"

fsm::StateBase* trAaToAab::NextState() const {return new stAab(mActions);}