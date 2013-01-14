#if defined(SUPPORT_FOR_WIN32_OSAL)
# include "impl/Win32MessageQueue.hpp"  // this would have the real win32 message queue
//#elif defined(SUPPORT_FOR_OSAL_MOCKS)
#else
# include "mocks/src/MessageQueue.hpp" // this one would have mock as the internals
//# include "noop/MessageQueue.hpp"  // this would have a file without any action in it
#endif  // SUPPORT_FOR_WIN32_OSAL
