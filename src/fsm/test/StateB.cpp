#include"StateB.h"
#include"StateCStar.h"

fsm::StateBase* trBToCStar::NextState() const {return new stCStar(mActions);}