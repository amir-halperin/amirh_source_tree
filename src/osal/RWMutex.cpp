#include "osal/RWMutex.h" // header for for this file
#ifdef __VXWORKS__
# include "vxworks/RWMutexVxWorks.hpp" 
#elif defined(WIN32)  // note that for compilation on windows we have few choices here
# include "win32/RWMutex.hpp"
#else
# include "posix/RWMutexPosix.hpp"
#endif  // __VXWORKS__
