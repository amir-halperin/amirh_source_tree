#ifndef EVENT_NOTIFICATION_VXWORKS__HPP
#define EVENT_NOTIFICATION_VXWORKS__HPP

#include "CriticalSection.h"      // to gaurd error function registrations
#include "SemHandle.h" // to data type that are use to implement this
#include "ExitFunctionHolder.h"   // hold the exit functions
#include "OsalTimeUtils.h"        // support for milli2ticks
#include <vxWorks.h>              // vxworks data types
#include <errnoLib.h>             // library to support errno
#include <assert.h>               // assert function
#include <stdio.h>                // perror function
#include <memory>                 // for auto_ptr

namespace osal
{
  
namespace EventNotification
{
  
namespace
{
  Private::ExitFunctionHolder& ErrorFunctionsHandler()
  {
    static Private::ExitFunctionHolder value;
    return value;
  }
} // end of local namespace
  
struct Id : semvx::SemHandle 
{
  Id()
  {
  }
  
};

void RegisterAtExit(at_error_fun func)
{
  ErrorFunctionsHandler().Push(func);
}

Id* Create()
{
  std::auto_ptr<Id> id(new Id);
  if (!id->Open())
  {
    ErrorFunctionsHandler().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
  }
  return id.release();  // give the pointer back to the user
}

void Wait(Id* on)
{
  assert(on);
  if (!on->Take())
  {
    ErrorFunctionsHandler().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
  }
}

bool TryWait(Id* on)
{
  assert(on);
  if (!on->TryTake())
  {
    return ErrorFunctionsHandler().TryFail(errnoGet(), __FUNCTION__, __LINE__);
  }
  else
  {
    return true;
  }
}

bool TimedWait(Id* on, milliseconds_t milliDuration)
{
  assert(on);
  if (!on->Take(TimeUtils::Milli2Ticks(milliDuration)))
  {
    return ErrorFunctionsHandler().TimedOut(errnoGet(), __FUNCTION__, __LINE__);
  }
  else
  {
    return true;
  }
  
}

void Signal(Id* on)
{
  assert(on);
  if (!on->Give())
  {
    ErrorFunctionsHandler().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
  }
}

void SignalAll(Id* on)
{
  assert(on);
  if (!on->ReleaseAll())
  {
    ErrorFunctionsHandler().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
  }
}

void Delete(Id*& id)
{
  if (id)
  {
    delete id;
    id = 0;
  }
}

}       // end of namespace EventNotification
  
}       // end of namespace osal

#endif  // EVENT_NOTIFICATION_VXWORKS__HPP
