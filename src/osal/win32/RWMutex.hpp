#if defined(SUPPORT_FOR_WIN32_OSAL)
# include "impl/Win32RWMutex.hpp"  // this would have the real win32 RW mutex
//#elif defined(SUPPORT_FOR_OSAL_MOCKS)
#else
# include "mocks/src/RWMutex.hpp" // this one would have mock as the internals
//# include "noop/RWMutex.hpp"  // this would have a file without any action in it
#endif  // SUPPORT_FOR_WIN32_OSAL
