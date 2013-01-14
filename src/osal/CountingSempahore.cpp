#include "osal/CountingSemaphore.h"
#if defined(__VXWORKS__)
#	include "vxworks/CountingSempahoreVxWorks.hpp"
#elif defined(WIN32)  // note that for compilation on windows we have few choices here
# include "win32/CountingSemaphore.hpp"
#else
#	include "posix/CountingSempahorePosix.hpp"
#endif	// __VXWORKS__
