#ifndef RW_MUTEX_VXWORKS_MISSING_IMPLE__HPP
#define RW_MUTEX_VXWORKS_MISSING_IMPLE__HPP

// this would use regualr mutex in case we have no support in
// the BSP for the read/write semaphore
#include "osal/Mutex.h" // the actual imple would be from here

namespace osal
{
  
namespace RWMutex
{
  
void RegisterAtExit(at_error_fun func)
{
  Mutex::RegisterAtExit(func);
}

struct Id
{
  Id(Mutex::Id* from) : mData(from)
  {
    
  }
  
  ~Id()
  {
    Mutex::Delete(mData);
  }
  
  Mutex::Id* mData;
};

Id* Create()
{
  return new Id(Mutex::CreateRecursive(Mutex::PRIORITY_SAFE));  // this is the same protection
}


void ReadLock(Id* id)
{
  assert(id);
  Mutex::Lock(id->mData);
}

bool TryReadLock(Id* id)
{
  assert(id);
  return Mutex::TryLock(id->mData);
}

bool TimedReadLock(Id* id, milliseconds_t milliTimeout)
{
  assert(id);
  return Mutex::TimedLock(id->mData, milliTimeout);
}

void WriteLock(Id* id)
{
  assert(id);
  Mutex::Lock(id->mData);
}

bool TryWriteLock(Id* id)
{
  assert(id);
  return Mutex::TryLock(id->mData);
}

bool TimedWriteLock(Id* id, milliseconds_t milliTimeout)
{
  assert(id);
  return Mutex::TimedLock(id->mData, milliTimeout);
}

void Release(Id* id)
{
  assert(id);
  Mutex::Release(id->mData);
}

void Delete(Id*& id)
{
  if (id)
  {
    delete id;
    id = 0;
  }
}
}  // end of namespace RWMutex
  
} // end of namespace osal

#endif  // RW_MUTEX_VXWORKS_MISSING_IMPLE__HPP
