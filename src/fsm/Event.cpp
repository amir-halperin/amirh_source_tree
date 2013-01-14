#include <fsm/Event.h>
#include <string>
#include <iostream>

namespace fsm {

bool EventBase::operator==(const EventBase& other) const
{
  return (Type().compare(other.Type()) == 0);
}

// 
std::ostream& operator<<(std::ostream& os, const EventBase& e) 
{
  return os << e.Type().c_str();
}

}; // namespace fsm_fw
