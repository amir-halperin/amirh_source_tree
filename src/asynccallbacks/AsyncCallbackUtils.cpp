#include "asynccallbacks/details/AsyncCallbackUtils.h"  // headfile for this cpp file
#include "osal/Mutex.h" // the data type of mutex
#include "asynccallbacks/details/WorkingQueueEntry.h"  // the data we are working on here
#include <assert.h>     // assert macro

namespace asynccallbacks
{
namespace utils
{
  CriticalSection::CriticalSection(osal::Mutex::Id* m) : mGuard(m)
  {
    osal::Mutex::Lock(mGuard);
  }
  
  CriticalSection::~CriticalSection()
  {
    osal::Mutex::Release(mGuard);
  }



} // end of namespace utils
} // end of namespace  asynccallbacks
