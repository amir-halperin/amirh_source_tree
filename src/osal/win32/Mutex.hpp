#if defined(SUPPORT_FOR_WIN32_OSAL)
# include "impl/Win32Mutex.hpp"  // this would have the real win32 mutex
//#elif defined(SUPPORT_FOR_OSAL_MOCKS)
#else
# include "mocks/src/Mutex.hpp" // this one would have mock as the internals
//# include "noop/Mutex.hpp"  // this would have a file without any action in it
#endif  // SUPPORT_FOR_WIN32_OSAL
