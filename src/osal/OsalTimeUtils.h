#ifndef OSALSHAREDRESOURCES__H
#define OSALSHAREDRESOURCES__H

#include "osal/OsalGeneralDefines.h"  // shared resources and types for osal modules

namespace osal
{

namespace TimeUtils
{
// This function would convert the value of the milli seconds
// in to the value of ticks - since all VxWorks native functions
// are using ticks
///@return the number of ticks from a given millisecond value based on clock rate
int Milli2Ticks(milliseconds_t milli);

/// @return the number of ticks based on host configuration to clock rate
milliseconds_t Ticks2Milli(int ticks);

// @return the minimum resolution that can be set for any function that accept duration value
milliseconds_t MinResolution();

}  // TimeUtils

}	// end of osal namespace

#endif	// OSALSHAREDRESOURCES__H

