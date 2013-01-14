#ifndef COUNTING_SEMAPHORE_IMPLE_HPP
#define COUNTING_SEMAPHORE_IMPLE_HPP 
// This file would implement semaphore based on boost (which would soon be part of 
// C++ standard implementation) for semaphores 
// since this is portable by itself we can use this for both win32 and posix plaftfroms

#include "osal/OsalGeneralDefines.h"            // timeout typedef
#include <boost/interprocess/sync/interprocess_semaphore.hpp> // actual imple is here
#include <boost/thread/thread_time.hpp>         // system_time

namespace osal
{
namespace CountingSemaphore
{
  
namespace details
{
  
class Semaphore
{
public:
  Semaphore(int count) : mSem(count)
  {
  }
  
  void Wait()
  {
    mSem.wait();
  }
  
  bool TryWait()
  {
    return mSem.try_wait();
  }
  
  bool TimeWait(milliseconds_t timeout)
  {
    return mSem.timed_wait(boost::get_system_time() + boost::posix_time::milliseconds(timeout)); 
  }
  
  void Post()
  {
    mSem.post();
  }
  
private:
    boost::interprocess::interprocess_semaphore mSem;
};
  
} // end of namespace details
    
} // end of namespace CountingSemaphore
  
} // end of namespace osal

#else
# error "you must not include this file inside header file"
#endif  // COUNTING_SEMAPHORE_IMPLE_HPP 
