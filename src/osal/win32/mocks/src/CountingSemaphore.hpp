#ifndef COUNTING_SEMAPHORE_IMPLE_HPP
#define COUNTING_SEMAPHORE_IMPLE_HPP
// Note that in this implementation like in all the other implementation we would no operation that would be done
// so calling any function here would have no effect
// note that this is useful for unit tests where we would not like to test in any way the effect of calling this
#include "../details/CountingSemaphoreMock.hpp"
#include "../include/CountingSemaphoreFwd.h"   // definition for ID class
#include <assert.h> // assert macro

namespace osal
{

namespace CountingSemaphore
{
  
/** The type CountSemId is opaque to the application programmer */
  Id::Id(int count)
  {
    mData.Create(count);
  }
  
  Id::~Id()
  {
    mData.Delete();
  }

void RegisterAtExit(at_error_fun)
{  
}

Id* Create(int count)
{ 
 return new Id(count);
}


void Wait(Id* id)
{
  assert(id);
  id->mData.Wait();
}

bool TimedWait(Id* id, milliseconds_t)
{
  assert(id);
  return id->mData.TimeWait();
}

bool TryWait(Id* id)
{
  assert(id);
  return id->mData.TryWait();
}

void Post(Id* id)
{ 
  assert(id);
  id->mData.Post();
}

void Delete(Id*& id)
{  
  delete id;
  id = 0;
}

} // end of namespace CountingSemaphore
} // end of namespace osal
#else
# error "you must not include this file inside another header file"
#endif  //COUNTING_SEMAPHORE_IMPLE_HPP 
