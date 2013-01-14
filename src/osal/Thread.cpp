#include "osal/Thread.h"
#if defined(__VXWORKS__)
#	include "vxworks/ThreadVxWorks.hpp" 
#elif defined(WIN32)  // note that for compilation on windows we have few choices here
# include "win32/Thread.hpp"
#else
#	include "posix/ThreadPosix.hpp"
#endif	// __VXWORKS__
