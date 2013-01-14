#ifndef EVENT_NOTOFOCATION_IMPLE_HPP
#define EVENT_NOTOFOCATION_IMPLE_HPP 
// This file would implement event notifications based on boost (which would soon be part of 
// C++ standard implementation) for event notifications 
// since this is portable by itself we can use this for both win32 and posix plaftfroms
#include "osal/OsalGeneralDefines.h"            // timeout typedef
#include <boost/thread/condition_variable.hpp>  // actual imple
#include <boost/thread/mutex.hpp>               // we also need a lock for this
#include <boost/date_time/posix_time/posix_time_types.hpp>  // duration type

namespace osal
{
namespace EventNotification
{
namespace details
{
  
class EventNotification
{
public:
  EventNotification()
  {
  }
  
  void Wait()
  {
    boost::unique_lock<boost::mutex> lock(mLock); // we need to get the lock before using this
    mNotifier.wait(lock);
  }
  
  bool TryWait()
  {
    return TimeWait(1); // make this with very short duration
  }
  
  bool TimeWait(milliseconds_t timeout)
  {
    boost::unique_lock<boost::mutex> lock(mLock); // we need to get the lock before using this
    return mNotifier.timed_wait(lock, boost::posix_time::milliseconds(timeout));
  }
  
  void Release()
  {
    mNotifier.notify_one();
  }
  
  void ReleaseAll()
  {
    mNotifier.notify_all();
  }
  
private:
  boost::condition_variable mNotifier;
  boost::mutex              mLock;
};
  
}
  
} // end of namespace EventNotificatin
  
} // end of namespace osal

#else
# error "you must not include this file inside header file"
#endif  //EVENT_NOTOFOCATION_IMPLE_HPP 
