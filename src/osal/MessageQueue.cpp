#include "osal/MessageQueue.h"
#ifdef __VXWORKS__
# include "vxworks/MessageQueueVxWorks.hpp"
#elif defined(WIN32)  // note that for compilation on windows we have few choices here
# include "win32/MessageQueue.hpp"
#else
# include "posix/MessageQueuePosix.hpp"
#endif  // __VXWORKS__
