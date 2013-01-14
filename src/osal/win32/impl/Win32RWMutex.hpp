#ifndef WIN32_RWMUTEX_IMPLE_HPP
#define WIN32_RWMUTEX_IMPLE_HPP

#include "../../details/RWMutex.hpp" // actual imple is there

namespace osal
{
namespace RWMutex
{

struct Id : public details::ReadWriteMutex
{
  Id()
  {
  }
};

Id* Create()
{
  return new Id;
}

void ReadLock(Id* id)
{
  assert(id);
  id->ReadLock();
}

bool TryReadLock(Id* id)
{
  assert(id);
  return id->TryReadLock(); 
}

bool TimedReadLock(Id* id, milliseconds_t milliTimeout)
{
  assert(id);
  return id->TimeReadLock(milliTimeout);
}

void WriteLock(Id* id)
{
  assert(id);
  id->WriteLock();
}

bool TryWriteLock(Id* id)
{
  assert(id);
  return id->TryWriteLock();
}

bool TimedWriteLock(Id* id, milliseconds_t milliTimeout)
{
  assert(id);
  return id->TimeWriteLock(milliTimeout);
}

void Release(Id* id)
{
  assert(id);
  id->Unlock();
}

void Delete(Id*& id)
{
  delete id;
  id = 0;
}

} // end of namespace RWMutex
  
} // end of namespace osal

#else
# error "you must not include this file inside header file"
#endif  // WIN32_RWMUTEX_IMPLE_HPP
