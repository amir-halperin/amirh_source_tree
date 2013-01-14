#ifndef EVENT_NOTIFICATION_IMPLE_HPP
#define EVENT_NOTIFICATION_IMPLE_HPP
// Note that in this implementation like in all the other implementation we would no operation that would be done
// so calling any function here would have no effect
// note that this is useful for unit tests where we would not like to test in any way the effect of calling this

#include "../details/EventNotificationMock.hpp"  // mock class is defined here
#include "../include/EventNotificationFwd.h"     // Id class
#include <assert.h>                   // assert macro

namespace osal
{

namespace EventNotification
{

  Id::Id()
  {
    mData.Create();
  }
  
  Id::~Id()
  {
    mData.Delete();
  }

void RegisterAtExit(at_error_fun)
{
}

Id* Create()
{
  return new Id;
}

void Wait(Id* id)
{
  assert(id);
  id->mData.Wait();
}

bool TryWait(Id* id)
{
  assert(id);
  return id->mData.TryWait();
}

bool TimedWait(Id* id, milliseconds_t)
{
  assert(id);
  return id->mData.TimeWait();
}

void Signal(Id* id)
{ 
  assert(id);
  id->mData.Signal(); 
}

void SignalAll(Id* id)
{  
  assert(id);
  id->mData.SignalAll(); 
}

void Delete(Id*& id)
{
  delete id;
  id = 0;
}

} // end of namespace EventNotification
} // end of namespace osal

#else
# error "you must not include this file inside another header file"
#endif //EVENT_NOTIFICATION_IMPLE_HPP
