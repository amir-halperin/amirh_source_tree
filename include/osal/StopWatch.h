#ifndef STOPWATCH_DEF_H
#define STOPWATCH_DEF_H

#include "osal/OsalGeneralDefines.h"  // milliseconds_t
#ifndef __VXWORKS__
# include <boost/date_time/posix_time/posix_time_types.hpp>
#endif  // __VXWORKS__

namespace osal
{
  // use this class to measure that time pass between 2 points in the execusion of the code
class StopWatchOper
{
public:
  StopWatchOper();  // construct the first point in time
  
  osal::milliseconds_t Pause() const; // return the current time that was measured, not realy stop the clock
  
  osal::milliseconds_t Stop();  // this would ensure that the clock would reset! return current value
  
private:
#ifdef __VXWORKS__
  osal::milliseconds_t 
#else
  boost::posix_time::ptime
#endif  // __VXWORKS__
                        mInitialValue;
};

} // end of namespace osal
  
namespace UT
{
  typedef osal::StopWatchOper StopWatch;
} // end of namespace UT

#endif  // STOPWATCH_DEF_H
