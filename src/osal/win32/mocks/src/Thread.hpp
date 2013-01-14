#ifndef THREAD_IMPLE_HPP
#define THREAD_IMPLE_HPP
// Note that in this implementation like in all the other implementation we would no operation that would be done
// so calling any function here would have no effect
// note that this is useful for unit tests where we would not like to test in any way the effect of calling this
#include "../details/ThreadMock.hpp" // mock class
#include "../include/ThreadFwd.h"    // Id class
#include <assert.h>       // assert macro

namespace osal
{
  
namespace Thread
{
  
  
  Id::Id(const Attributes& a, entry_func_t f)
  {
    mData.Create(a, f);
  }
  
  Id::~Id()
  {
    mData.Clean();
  }
  
Attributes::Attributes() :  Name("osalWin32noOp"), StackSize(0), Priority(LOWSET_PRIORITY)
{
}

Attributes::Attributes(const char* n, unsigned int ss, PriorityType p):  Name(n), StackSize(ss), Priority(p)
{
}

Attributes CreateAttribute(const char* name, unsigned int sSize, PriorityType p)
{
  return Attributes(name, sSize, p);
}


void RegisterAtExit(at_error_fun)
{ 
}

bool EqualsId(Id* left, Id* right)
{
 return left == right;
 
}


Id* Create(const Attributes& attr, entry_func_t entryFunc)
{
  return new Id(attr, entryFunc);
}

PriorityType Priority(Id* id)
{
  assert(id);
  return id->mData.Priority();
}

void NewPriority(Id* id, PriorityType p)
{
  assert(id);
  return id->mData.NewPriority(p);
}

const char* Name(Id* id)
{
  assert(id);
  return id->mData.Name();
}

bool Clean(Id*& id)
{
  delete id;
  id = 0;
  return true;
}

bool TimeClean(Id*& id, milliseconds_t )
{
  delete id;
  id = 0;
  return true;
}

namespace Self
{
  
Thread::Id* Id()
{
  return 0;
}

milliseconds_t Sleep(milliseconds_t)
{
  return 0;
}

void Suspend()
{ 
}

Thread::PriorityType Priority()
{
  return LOWSET_PRIORITY;
}

void NewPriority(Thread::PriorityType )
{
}

const char* Name()
{
  return "osalWin32noOp";
}

bool Equel(const Thread::Id* other)
{
  return true;
}

bool EqualName(const char* )
{
  return true;
}

} // end of namespace Self

} // end of namespace Thread
  
} // end of namespace osal

#else
# error "you must not include this file inside another header file"
#endif //THREAD_IMPLE_HPP
