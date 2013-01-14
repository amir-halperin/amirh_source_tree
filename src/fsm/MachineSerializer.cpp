#include <fsm/MachineSerializer.h>
#include <fsm/Machine.h>
#include <fsm/StateFactory.h>
#include <fsm/State.h>

namespace fsm {

void MachineSerializer::Save(std::ostream& os, MachineBase& m) const
{
  os << m.mFsmId << " ";
  os << m.mCurrentState->Type() << " ";
  m.mCurrentState->Save(os);
}

bool MachineSerializer::Load(std::istream& io, MachineBase* m) const
{
  bool isLoadSucceeded = false;

  if(m)
  {
    io >> m->mFsmId;

    std::string stateType;
    io >> stateType;
    StateBase* s = mStatefactory.Create(stateType);
    // bad state type or state type is not found in the state factory due to factory bad initialization
    if(s)
    {
      // use the restored state as the current state
      m->mCurrentState = s;
      // recovery machine current state
      isLoadSucceeded =  m->mCurrentState->Load(io);
    }
    else
    {
      isLoadSucceeded = false;
    }
   
  }

  return isLoadSucceeded;
}

}; // namespace fsm