#ifndef RW_MUTEX_DETAILS_IMPLE_HPP
#define RW_MUTEX_DETAILS_IMPLE_HP 

// This file would implement read/write mutex based on boost (which would soon be part of 
// C++ standard implementation) for mutex
// since this is portable by itself we can use this for both win32 and posix plaftfroms

#include "osal/OsalGeneralDefines.h"  // milliseconds_t
#include <boost/thread/shared_mutex.hpp>    // actual impl is taken from here
#include <boost/date_time/posix_time/posix_time_types.hpp> // duration type
#include <boost/thread/thread_time.hpp> // get_system_time
#if defined(BOOST_THREAD_PLATFORM_WIN32)  // for windows
# include <boost/thread/thread.hpp>       // sleep
#endif

namespace osal
{
  
namespace RWMutex
{
  
namespace details
{
  
bool ExclusiveTimedLock(boost::shared_mutex& lock, boost::posix_time::time_duration dur)
{
#if defined(BOOST_THREAD_PLATFORM_WIN32)  // for windows
  // windows don't support this option out of the box - we would do it with sleep
  // this is not an optimal option but we can live with it
  static const long minSleepTime(100);
  
  long millis = dur.total_milliseconds();
  long sleeptime = millis / minSleepTime > 1 ? 
                   millis / minSleepTime : 
                   1;
  while (millis > 0)
  {
    if (lock.try_lock_upgrade())
    {
      return true;
    }
    else
    {
      boost::this_thread::sleep(boost::posix_time::milliseconds(sleeptime*minSleepTime));
      millis -= minSleepTime;
      sleeptime = millis / minSleepTime;
    }
  }
  return false;
#else
  return lock.timed_lock_upgrade(dur);
#endif  // BOOST_THREAD_PLATFORM_WIN32
}
  
class ReadWriteMutex
{
public:
  ReadWriteMutex()
  {
  }
  
  void ReadLock()
  {
    mLock.lock_shared();
    release_func = ReadWriteMutex::SharedRelease;
  }
  
  void WriteLock()
  {
    mLock.lock();
    release_func = ReadWriteMutex::UniqueRelease;
  }
  
  bool TryReadLock()
  {
    if (mLock.try_lock_shared())
    {
      release_func = ReadWriteMutex::SharedRelease;
      return true;
    }
    else
    {
      return false;
    }
  }
  
  bool TryWriteLock()
  {
    if (mLock.try_lock())
    {
      release_func = ReadWriteMutex::UniqueRelease;
      return true;
    }
    else
    {
      return false;
    }
  }
  
  bool TimeReadLock(milliseconds_t timeout)
  {
    boost::posix_time::time_duration d = boost::posix_time::milliseconds(timeout);
    if (mLock.timed_lock_shared(boost::get_system_time() + d))
    {
      release_func = ReadWriteMutex::SharedRelease;
      return true;
    }
    else
    {
      return false;
    }
  }
  
  bool TimeWriteLock(milliseconds_t timeout)
  {
    boost::posix_time::time_duration d = boost::posix_time::milliseconds(timeout);
    //if (ExclusiveTimedLock(mLock, d))
    if (mLock.timed_lock(d))
    {
      release_func = ReadWriteMutex::UniqueRelease;
      return true;
    }
    else
    {
      return false;
    }    
  }
  
  void Unlock()
  {
    release_func(mLock);  // note that the write function would be called based on the lock we got
  }
  
private:
  
  static void UniqueRelease(boost::shared_mutex& what)
  {
    what.unlock();//_upgrade();
  }
  
  static void SharedRelease(boost::shared_mutex& what)
  {
    what.unlock_shared();
  }
  
private:
  boost::shared_mutex mLock;
  // we need to know what is the action to do when releasing the lock so we would have a pointer that would 
  // point to the function to be called based on the lock we took
  typedef void(*unlock_fun)(boost::shared_mutex& lock);
  unlock_fun  release_func;
};
  
} // end of namespace details
  
} // end of namespace RWMutex
  
} // end of namespace osal

#else
# error "you must not include this file inside header file"
#endif // RW_MUTEX_DETAILS_IMPLE_HP 
