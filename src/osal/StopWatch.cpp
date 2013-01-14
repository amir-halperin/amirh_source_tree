#include <osal/StopWatch.h> // header file for the class
#ifdef __VXWORKS__
# include "OsalTimeUtils.h"  // function to translate from ticks 2 milli and back
# include <vxWorks.h>  // vxworks types
# include <tickLib.h>  // tickGet
#else
#include <boost/date_time/posix_time/posix_time_types.hpp>
#endif  // __VXWORKS__

namespace osal
{
  
namespace
{
#ifdef __VXWORKS__
  typedef osal::milliseconds_t  current_time_t;
#else
  typedef boost::posix_time::ptime  current_time_t;
#endif  // __VXWORKS__
  
  
  current_time_t GetCurrentTime()
  {
#ifdef __VXWORKS__
    return osal::TimeUtils::Ticks2Milli(tickGet());
#else
    return boost::posix_time::microsec_clock::local_time();
#endif  // __VXWORKS__    
  }
  
  osal::milliseconds_t GetTimeDiff(current_time_t oldTime)
  {
#ifdef __VXWORKS__
    return GetCurrentTime() - oldTime;
#else
    return boost::posix_time::time_duration(GetCurrentTime() - oldTime).total_milliseconds();
#endif  // __VXWORKS__
  }
  
  
} // end of local namespace
 
StopWatchOper::StopWatchOper() : mInitialValue(GetCurrentTime())
{  
}

osal::milliseconds_t StopWatchOper::Pause() const
{
  return GetTimeDiff(mInitialValue);
}

osal::milliseconds_t StopWatchOper::Stop()
{
  osal::milliseconds_t ret = GetTimeDiff(mInitialValue);
  mInitialValue = GetCurrentTime();
  return ret;
}
  
} // end of namespace osal
