#include "ExitFunctionHolder.h" // header file for this source file
#include "CriticalSection.h"    // to automate the access to mutex
#include <string.h>             // for error reporting
#include <errno.h>              // for error reports
#include <assert.h>             // assert function
#include <stdio.h>              // THIS ONE NEED TO BE REMOVED
#ifdef __VXWORKS__
# include <errnoLib.h>          // for error reporting
#endif  // __VXWORKS__

namespace osal
{
  
namespace Private
{

namespace
{
  bool WasTimedOut(int err);
  bool ResourceNotAvial(int err);
  
#ifdef __VXWORKS__
  bool WasTimedOut(int err)
  {
    return err == S_objLib_OBJ_TIMEOUT;
  }
  bool ResourceNotAvial(int err)
  {
    return err == S_objLib_OBJ_UNAVAILABLE;
  }
# define ENTER_CRITICAL_SECTION(lock) CriticalSection<semvx::SemHandle> __lock_(lock)
#elif defined(WIN32)
# define ENTER_CRITICAL_SECTION(lock) // need to define for POSIX as well
  bool WasTimedOut(int err)
  {
    return true;
  }
  bool ResourceNotAvial(int err)
  {
    return true;
  }
#else
# define ENTER_CRITICAL_SECTION(lock) // need to define for POSIX as well
#endif  // __VXWORKS__
  
  void DefaultExitFunction(int)
  {
    printf("assering on exit\r\n");
    assert(0); // fix this function if it need to so that we would exit more gracefully
  }
}   // end of local namespace

at_error_fun ExitFunctionHolder::DefaultFunction = DefaultExitFunction; // set this as the default function

ExitFunctionHolder::ExitFunctionHolder() : mCount(0), mLock(true)
{
  Push(DefaultFunction);  // put the default exit function in the list
}

void ExitFunctionHolder::Push(at_error_fun fn)
{
  ENTER_CRITICAL_SECTION(mLock);
  if (mCount < MAX_FAILURE_CALLBACK_FUNCTIONS)
  {
    mList[mCount++] = fn;
  }
}

void ExitFunctionHolder::CriticalError(int err, const char* func, int line) const
{
  LogError(err, func, line);
  ENTER_CRITICAL_SECTION(mLock);
  while (Pop()) // run all the functions that were registered from the user
  {
    
  }
}

bool ExitFunctionHolder::TimedOut(int err, const char* func, int line) const
{
  if (!WasTimedOut(err))
  {
    CriticalError(err, func, line);
  }
  return false;
}


bool ExitFunctionHolder::TryFail(int err, const char* func, int line) const
{
  if (!ResourceNotAvial(err))
  {
    CriticalError(err, func, line);
  }
  return false;
}
  
void ExitFunctionHolder::LogError(int err, const char* func, int line)
{
  // write now we don't have the real log here..
  printf("critical error %s@%d: %s\n", func, line, strerror(err));
}

at_error_fun ExitFunctionHolder::Pop() const
{
  if (mCount == 0)
  {
    return NULL;
  }
  else
  {
    return mList[mCount--];
  }
}

void ExitFunctionHolder::SetDefaultFunction(at_error_fun fn)
{
  DefaultFunction = fn;
  mList[0] = fn;    // replace this in the list of functions as well!
}

void ExitFunctionHolder::Assert(const char* msg, const char* func, int line) const
{
  printf("critical error %s@%d: %s\n", func, line, msg);
  ENTER_CRITICAL_SECTION(mLock);
  while (Pop()) // run all the functions that were registered from the user
  {
  }
}

} // end of namespace Private
  
} // end of namespace osal
