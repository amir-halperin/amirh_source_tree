#include"StateA.h"
#include"StateAa.h"
#include"StateAc.h"
#include"StateB.h"

fsm::StateBase* trAToAa::NextState() const {return new stAa(mActions);}
fsm::StateBase* trAToAc::NextState() const {return new stAc(mActions);}
fsm::StateBase* trAToB::NextState() const {return new stB(mActions);}