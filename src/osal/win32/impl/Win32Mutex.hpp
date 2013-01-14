#ifndef WIN32_MUTEX_IMPLE_HPP
#define  WIN32_MUTEX_IMPLE_HPP

#include "../../details/Mutex.hpp" // actual imple
#include <boost/shared_ptr.hpp> // shared_ptr --> safe pointer

namespace osal
{ 
namespace Mutex
{
 
struct Id
{
  typedef details::MutexImpl  mutex_type; // this is a base for the mutex we are using
public:
  Id() : mMutex(new details::SimpleMutexImpl)
  { 
  }
  
  Id(Protection p) : mMutex(new details::SafeMutexImpl)
  {    
  }
  
  mutex_type& get()
  {
    return *mMutex;
  }
  
private:
  
  boost::shared_ptr<mutex_type> mMutex;
};


Id* Create()
{
  return new Id;
}

Id* CreateRecursive(Protection prioritySafe)
{
  return new Id(prioritySafe);
}

void Lock(Id* id)
{
  assert(id);
  id->get().Lock();
}

bool TryLock(Id* id)
{
  assert(id);
  return id->get().TryLock();
}

bool TimedLock(Id* id, milliseconds_t milliDuration)
{
  assert(id);
  return id->get().TimeLock(milliDuration);
}

void Release(Id* id)
{
  assert(id);
  id->get().Unlock();
}

void Delete(Id*& id)
{
  delete id;
  id = 0;
}

}  // end of Mutex 
} // end of namespace osal

#else
# error "you must not include this file inside header file"
#endif  // WIN32_MUTEX_IMPLE_HPP
