#include <fsm/Transition.h>
#include <string.h>

namespace fsm {

bool TransitionBase::operator!=(const TransitionBase& other) const
{
  // transition objects are not equal in case their types (represented by strings) are not equal
  return (strcmp(Type(), other.Type()) != 0);
}

bool TransitionBase::operator==(const TransitionBase& other) const
{
  return !(*this!=other);
}

}; // namespace fsm_fw