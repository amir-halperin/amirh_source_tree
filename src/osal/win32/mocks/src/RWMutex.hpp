#ifndef RW_MUTEX_IMPLE_HPP
#define RW_MUTEX_IMPLE_HPP
// Note that in this implementation like in all the other implementation we would no operation that would be done
// so calling any function here would have no effect
// note that this is useful for unit tests where we would not like to test in any way the effect of calling this
#include "../include/RWMutexFwd.h"     // definition for id class
#include "../details/RWMutexMock.hpp"  // this is the mock class file
#include "osal/OsalGeneralDefines.h"  // milliseconds_t
#include "osal/RWMutex.h"   // header file for this
#include <assert.h>       // assert macro

namespace osal
{
namespace RWMutex
{
  
Id::Id()
{
  mData.Create();
}
  
Id::~Id()
{
  mData.Delete();
}


Id* Create()
{
  return new Id; 
}


void RegisterAtExit(at_error_fun)
{
}

void ReadLock(Id* id)
{
  assert(id);
  id->mData.ReadLock();
}

bool TryReadLock(Id* id)
{
  assert(id);
  return id->mData.TryReadLock();
}

bool TimedReadLock(Id* id, milliseconds_t)
{
  assert(id);
  return id->mData.TimeReadLock();
}
  
void WriteLock(Id* id)
{
  assert(id);
  id->mData.WriteLock();
}

bool TryWriteLock(Id* id)
{
  assert(id);
  return id->mData.TryWriteLock();
}

bool TimedWriteLock(Id* id, milliseconds_t)
{
  assert(id);
  return id->mData.TimeWriteLock();
}

void Release(Id* id)
{
  assert(id);
  id->mData.Release();
}

void Delete(Id*& id)
{  
  delete id;
  id = 0;
}

} // end of namespace Mutex
  
} // end of namespace osal
#else
# error "you must not include this file inside another header file"
#endif //RW_MUTEX_IMPLE_HPP
