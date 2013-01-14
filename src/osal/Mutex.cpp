#include "osal/Mutex.h"
#ifdef __VXWORKS__
# include "vxworks/MutexVxWorks.hpp"
#elif defined(WIN32)  // note that for compilation on windows we have few choices here
# include "win32/Mutex.hpp"
#else
# include "posix/MutexPosix.hpp"
#endif  // __VXWORKS__
