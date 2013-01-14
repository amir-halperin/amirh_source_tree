#ifndef MUTEX_IMPLE_HPP
#define MUTEX_IMPLE_HPP
// Note that in this implementation like in all the other implementation we would no operation that would be done
// so calling any function here would have no effect
// note that this is useful for unit tests where we would not like to test in any way the effect of calling this
#include "../details/MutexMock.hpp"  // this is the mock class file
#include "../include/MutexFwd.h"     // the Id class
#include <assert.h>       // assert macro

namespace osal
{
  
namespace Mutex
{
  
  Id::Id()
  {
    mData.Create();
  }
  
  Id::Id(int p)
  {
    mData.CreateRecursive(p);
  }
  
  Id::~Id()
  {
    mData.Delete();
  }


Id* Create()
{
  return new Id;
}

Id* CreateRecursive(Protection p)
{
  return new Id((int)p);
}

void RegisterAtExit(at_error_fun)
{
}

void Lock(Id* id)
{
  assert(id);
  id->mData.Lock();
}

bool TryLock(Id* id)
{
  assert(id);
  return id->mData.TryLock();
}

bool TimedLock(Id* id, milliseconds_t)
{
  assert(id);
  return id->mData.TimeLock();
}
  
void Release(Id* id)
{
  assert(id);
  id->mData.Release();
}

void Delete(Id*& id)
{
  delete id;
  id = 0;
}

} // end of namespace Mutex
  
} // end of namespace osal
#else
# error "you must not include this file inside another header file"
#endif //EVENT_NOTIFICATION_IMPLE_HPP
