#include "OsalTimeUtils.h"  // header file for this file

#ifdef __VXWORKS__
#include <sysLib.h>   // sysClkRateGet
#endif  // __vxworks__

namespace osal
{
  
namespace TimeUtils
{
    
  
  namespace
  {
    const milliseconds_t milli2seconds = 1000;
    const milliseconds_t round_factor = milli2seconds / 2;
  } // end of local namespace
  
  int Milli2Ticks(milliseconds_t milli)
  {
#ifdef __VXWORKS__  
    milli *= (milliseconds_t)sysClkRateGet();
    if (milli < milli2seconds)
    {
      return 1;
    }
    else
    {
      // note that we don't want to return here for 1900 the number 2 
      // so we are doing floor(seil(ticks)) to make sure that we are
      // doing correct rouning
      // the reason no to return double (the current type)
      // is that in vxworks no all tasks support floating point
      //operations!
      return (int)((milli + round_factor)/milli2seconds);
    }    
#else
    return milli;
#endif  // __VXWORKS__
    
  }
  
  milliseconds_t Ticks2Milli(int ticks)
  {
#ifdef __VXWORKS__
    return (milliseconds_t)(ticks * milli2seconds/(milliseconds_t)sysClkRateGet());
#else
    return ticks;
#endif // __VXWORKS__
  }
  
  milliseconds_t MinResolution()
  {
#ifdef __VXWORKS__
    return milli2seconds / (milliseconds_t)sysClkRateGet();
#else
    static const milliseconds_t MIN_RES_NRT_OS = 100;
    return MIN_RES_NRT_OS;
#endif // __VXWORKS__
  }
} // end of TimeUtils namesapce
  
} // end of namespace osal
