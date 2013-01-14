#ifndef WIN32_EVENT_NOTIFICATION_IMPLE_HPP
#define WIN32_EVENT_NOTIFICATION_IMPLE_HPP

#include "../../details/EventNotification.hpp" // actual imple

namespace osal
{
  
namespace EventNotification
{
  
struct Id : public details::EventNotification
{
  Id()
  {
  }
};
  

Id* Create()
{
  return new Id;
}


void Wait(Id* on)
{
  assert(on);
  on->Wait();
}

bool TryWait(Id* on)
{
  assert(on);
  return on->TryWait();
}

bool TimedWait(Id* on, milliseconds_t milliDuration)
{
  assert(on);
  return on->TimeWait(milliDuration);
}

void Delete(Id*& what)
{
  delete what;
  what = 0;
}

void Signal(Id* on)
{
  assert(on);
  on->Release();
}

void SignalAll(Id* on)
{
  assert(on);
  on->ReleaseAll();
}

} // end of namespace EventNotification
} // end of namespace osal

#else
# error "you must not include this file inside header file"
#endif  // WIN32_EVENT_NOTIFICATION_IMPLE_HPP  
