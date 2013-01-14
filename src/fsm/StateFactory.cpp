#include <fsm/StateFactory.h>
#include <fsm/State.h>

namespace fsm {

void StateFactory::Insert(StateBase* prototype)
{
  mStatePrototypes.insert(StringStatePairType(prototype->Type(), prototype));
}

StateBase* StateFactory::Create(std::string type) const
{
  StringToStateMapConstInteratorType i = mStatePrototypes.find(type);

  if(i != mStatePrototypes.end())
  {
    // the prototype
    StateBase* s = i->second; 
    // create new instance which is the same as the prototype
    return (StateBase*)s->Create(); 
  }

  return 0;
}

}; // namespace fsm