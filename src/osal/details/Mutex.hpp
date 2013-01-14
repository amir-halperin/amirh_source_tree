#ifndef MUTEX_DETAIL_IMPLE_HPP
#define MUTEX_DETAIL_IMPLE_HPP 
// This file would implement mutex based on boost (which would soon be part of 
// C++ standard implementation) for mutex
// since this is portable by itself we can use this for both win32 and posix plaftfroms

#include "osal/OsalGeneralDefines.h"  // milliseconds_t
#include <boost/thread/mutex.hpp>     // class boost::thread
#include <boost/thread/recursive_mutex.hpp>     // class boost::recursive_mutex
#include <boost/date_time/posix_time/posix_time_types.hpp> // duration type

namespace osal
{
  
namespace Mutex
{
  
namespace details
{

class MutexImpl
{
public:
  virtual void Lock() = 0;
  
  virtual void Unlock() = 0;
  
  virtual bool TimeLock(milliseconds_t timeout) = 0;
  
  virtual bool TryLock() = 0;
  
protected:
  template<typename Lock>
  void LockImple(Lock& with)
  {
    with.lock();
  }
  
  template<typename Lock>
  bool TryLockImple(Lock& with)
  {
    return with.try_lock();
  }
  
  template<typename Lock>
  bool TimeLockImple(Lock& with, milliseconds_t to)
  {
    boost::posix_time::time_duration d = boost::posix_time::milliseconds(to);
    return with.timed_lock(d);
  }
  
  template<typename Lock>
  void UnlockImple(Lock& what)
  {
    what.unlock();
  }
};

// first case is to support for mutex that is not safe in the sense that 
// if you would lock twice you would deadlock the calling thread
class SimpleMutexImpl : public MutexImpl
{
public:
  SimpleMutexImpl()
  {
    
  }
  
  void Lock()
  {
    LockImple(mLock);
  }
  
  void Unlock()
  {
    UnlockImple(mLock);
  }
  
  bool TimeLock(milliseconds_t timeout)
  {
    return TimeLockImple(mLock, timeout);
  }
  
  bool TryLock()
  {
    return TryLockImple(mLock);
  }
  
private:
  boost::timed_mutex mLock; // note that only this mutex type support the actions we need to support as well
};

// second case is when we need to support a safe mutex one that 
// protect the caller from deadlocking because lock was called
// twice from the same thread
class SafeMutexImpl : public MutexImpl
{
public:
  SafeMutexImpl()
  {
    
  }
  
  void Lock()
  {
    LockImple(mLock);
  }
  
  void Unlock()
  {
    UnlockImple(mLock);
  }
  
  bool TimeLock(milliseconds_t timeout)
  {
    return TimeLockImple(mLock, timeout);
  }
  
  bool TryLock()
  {
    return TryLockImple(mLock);
  }
  
private:
  boost::recursive_timed_mutex mLock;
};

///////////////////////////////////////////////////////////////////////////////

} // end of namespace details
 
} // end of namespace Mutex
} // namespace osal

#else
# error "you must not include this file inside header file"
#endif  //MUTEX_DETAIL_IMPLE_HPP 
