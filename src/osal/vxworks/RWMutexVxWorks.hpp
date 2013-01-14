#ifndef RW_MUTEX_VXWORKS__HPP
#define RW_MUTEX_VXWORKS__HPP

#include "ExitFunctionHolder.h"   // for the functions that handle the error cases
#include "OsalTimeUtils.h"        // Milli2Ticks and Ticks2Milli 
#include <semLib.h>               // for the type of semaphore we have here
#include <errnoLib.h>             // errnoGet
#include <vxWorks.h>              // vxworks types
#include <assert.h>               // assert function
#include <memory>                 // auto_ptr

#else
# error "You cannot include this file inside other header file"
#endif  // RW_MUTEX_VXWORKS__HPP

#ifdef INCLUDE_SEM_READ_WRITE
namespace osal
{
  
namespace RWMutex
{
  
namespace 
{
  Private::ExitFunctionHolder& ErrorHandler()
  {
    static Private::ExitFunctionHolder theList;
    return theList;
  }
  
  const unsigned int MAX_READER_4_READ_WRITE_SEM = 20;
} // end of local namespace

  
struct Id
{
  SEM_ID  data;
  
  Id() : data(semRWCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE, MAX_READER_4_READ_WRITE_SEM))
  {
  }
  
  ~Id()
  {
    if (semWTake(data, WAIT_FOREVER) == OK)
    {
      semDelete(data);
    }
  }
  
  SEM_ID get() const
  {
    return data;
  }
  
  bool valid() const
  {
    return data != NULL;
  }
};

void RegisterAtExit(at_error_fun func)
{
  ErrorHandler().Push(func);
}

Id* Create()
{
  std::auto_ptr<Id> id(new Id);
  if (!id->valid())
  {
    ErrorHandler().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
  }
  return id.release();
}

void ReadLock(Id* id)
{
  assert(id);
  if (semRTake(id->get(), WAIT_FOREVER) != OK)
  {
    ErrorHandler().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
  }
}

bool TryReadLock(Id* id)
{
  assert(id);
  if (semRTake(id->get(), NO_WAIT) != OK)
  {
    return ErrorHandler().TryFail(errnoGet(), __FUNCTION__, __LINE__);
  }
  else
  {
    return true;
  }
}

bool TimedReadLock(Id* id, milliseconds_t milliTimeout)
{
  assert(id);
  if (semRTake(id->get(), TimeUtils::Milli2Ticks(milliTimeout)) != OK)
  {
    return ErrorHandler().TimedOut(errnoGet(), __FUNCTION__, __LINE__);
  }
  else
  {
    return true;
  }
}

void WriteLock(Id* id)
{
  assert(id);
  if (semWTake(id->get(), WAIT_FOREVER) != OK)
  {
    ErrorHandler().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
  }
}

bool TryWriteLock(Id* id)
{
  assert(id);
  if (semWTake(id->get(), NO_WAIT) != OK)
  {
    return ErrorHandler().TryFail(errnoGet(), __FUNCTION__, __LINE__);
  }
  else
  {
    return true;
  }
}

bool TimedWriteLock(Id* id, milliseconds_t milliTimeout)
{
  assert(id);
  if (semWTake(id->get(), TimeUtils::Milli2Ticks(milliTimeout)) != OK)
  {
    return ErrorHandler().TimedOut(errnoGet(), __FUNCTION__, __LINE__);
  }
  else
  {
    return true;
  }
}

void Release(Id* id)
{
  assert(id);
  if (semGive(id->get()) != OK)
  {
    ErrorHandler().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
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

} // end of namespace RWMutex
  
} // end of namespace osal
#else
# include "RWMutexVxWorksMissingImple.hpp"  // it may be that vxworks don't have
              // read/write semaphore support in the BSP, in this case this
              // would be only degenerated to regualr mutex
#endif
