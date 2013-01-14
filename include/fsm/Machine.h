#pragma once

#include <fsm/details/MachineBase.h>

namespace fsm {

// Template class that wrap the MachineBase by providing it the initial State object.
// This class eliminate the need from the user to derive MachineBase for different machine types.
//
// Usage example:
//
// // stStart is the initial state of TestMachine
// typedef Machine<stStart> TestMachine; 
//
//void main()
//{
//  TestMachine m;
//
//  evA a;
//  m.ProcessEvent(a);
//}
template<typename INIT_STATE, typename TRANSITION_ACTIONS>
class Machine : public MachineBase
{
public:
  
  // used during loading machine from stream
  Machine(TRANSITION_ACTIONS& actions) : MachineBase(0, new INIT_STATE(actions) ) {}
  // set init state based on template parameter
  Machine(TRANSITION_ACTIONS& actions, int fsmId) : MachineBase(fsmId, new INIT_STATE(actions) ) {}
};

}; // namespace fsm