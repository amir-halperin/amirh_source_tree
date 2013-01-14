#ifndef COUNTING_SEMAPHORE_WIN32_IMPLE_HPP
#define COUNTING_SEMAPHORE_WIN32_IMPLE_HPP

#include "../../details/CountingSemaphore.hpp" // actual imple is in here

namespace osal
{
namespace CountingSemaphore
{

struct Id : public details::Semaphore
{
  Id(int count) : Semaphore(count)
  {    
  }
  
};

Id* Create(int count)
{
  return new Id(count);
}

void Wait(Id* on)
{
  assert(on);
  on->Wait();
}


bool TimedWait(Id* on, milliseconds_t milliDuration)
{
  assert(on);
  return on->TimeWait(milliDuration);
}

bool TryWait(Id* on)
{
  assert(on);
  return on->TryWait();
}

void Post(Id* on)
{
  assert(on);
  on->Post();
}

void Delete(Id*& what)
{
  delete what;
  what = 0;
}

} // end of namespace CountingSemaphore
  
} // end of namespace osal

#else
# error "you must not include this file inside header file"
#endif  //COUNTING_SEMAPHORE_WIN32_IMPLE_HPP 
