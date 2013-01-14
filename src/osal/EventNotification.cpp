#include "osal/EventNotification.h"
#ifdef __VXWORKS__
# include "vxworks/EventNotificationVxWorks.hpp"
#elif defined(WIN32)  // note that for compilation on windows we have few choices here
# include "win32/EventNotification.hpp"
#else
# include "posix/EventNotificationPosix.hpp"
#endif  // __VXWORKS__ 
