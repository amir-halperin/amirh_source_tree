#ifndef WIN32_THREAD_IMPLE_HPP
#define WIN32_THREAD_IMPLE_HPP

#include "../../details/Thread.hpp"    // actual imple is here
#include <string>                   // to save thread name

namespace osal 
{
namespace Thread
{

struct Id
{
  
  Id(const Attributes& att, entry_func_t startFunc) : mHandle(details::Start(startFunc)), mAttr(att), mName(att.Name)
  {    
  }
  
  boost::thread& get()
  {
    return mHandle;
  }
  
  const boost::thread& get() const
  {
    return mHandle;
  }
  
  PriorityType GetPrio() const
  {
    return mAttr.Priority;
  }
  
  void SetPrio(PriorityType p)
  {
    mAttr.Priority = p;
  }
  
  const char* Name() const
  {
    return mName.c_str();
  }
  
private:
  boost::thread mHandle;
  Attributes    mAttr;
  std::string   mName;
};

Id* Create(const Attributes& attr, entry_func_t entryFunc)
{
  return new Id(attr, entryFunc);
}

PriorityType Priority(Id* to)
{
  assert(to);
  return to->GetPrio();
}

bool EqualsId(Id* left, Id* right)
{
  return left && right && (left->get() == right->get());
}

void NewPriority(Id* to, PriorityType newPrio)
{
  assert(to);
  to->SetPrio(newPrio);
  // not supported under this paltform
}

const char* Name(Id* id)
{
  assert(id);
  return id->Name();
}


bool Clean(Id*& id)
{
  //static const milliseconds_t time2wait = 2000; // wait for 2 seconds for thread to exit
  if (id)
  {
    id->get().join();
    delete id;
    id = 0;
#if 0
    if (id->get().timed_join(boost::posix_time::milliseconds(time2wait)))
    {
      delete id;
      id = 0;
      return true;
    }
    else
    {
      return false;
    }
#endif
  }
  return true;
}

bool TimeClean(Id*& id, milliseconds_t milliDuration)
{
  // note that we cannot kill a thread here!
  if (id)
  {
    if (id->get().timed_join(boost::posix_time::milliseconds(milliDuration)))
    {
      delete id;
      id = 0;
      return true;
    }
    else
    {
      id->get().detach(); // we fail to stop it
      delete id;
      id = 0;             // there is nothing we can do about this anymore
      return false;
    }
    
  }  
  else
  {
    return true;
  }
}

namespace Self
{

Thread::Id* Id()
{
  return 0; // we are not supporting this option
}
  
milliseconds_t Sleep(milliseconds_t milliDuration)
{
  details::Sleep(milliDuration);
  return milliDuration;
}

void Suspend()
{
  details::Suspend();
}

Thread::PriorityType Priority()
{
  return INVALID_PRIORITY;  // not supported
}

void NewPriority(Thread::PriorityType newPrio)
{
  // not supported
}

const char* Name()
{
  return "unknown"; // not supported
}

bool Equel(const Thread::Id* other)
{
  return other && boost::this_thread::get_id() == other->get().get_id();
}

bool EqualName(const char* expectName)
{
  return false; // we are not supporting this option
}

} // end of namespace Self

} // end of namespace Thread
} // end of namespace osal
#else
# error "you must not include this file inside header file"
#endif  // WIN32_THREAD_IMPLE_HPP
