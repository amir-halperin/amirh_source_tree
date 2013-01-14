#if defined(SUPPORT_FOR_WIN32_OSAL)
# include "impl/Win32EventNotification.hpp"  // this would have the real win32 event notification
#else
# include "mocks/src/EventNotification.hpp" // this one would have mock as the internals
//#elif defined(SUPPORT_FOR_OSAL_MOCKS)
//# include "noop/EventNotification.hpp"  // this would have a file without any action in it
#endif  // SUPPORT_FOR_WIN32_OSAL
