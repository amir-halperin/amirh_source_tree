#ifndef THREAD_DETAIL_IMPLE_HPP
#define THREAD_DETAIL_IMPLE_HPP 
// This file would implement thread based on boost (which would soon be part of 
// C++ standard implementation) for thread
// since this is portable by itself we can use this for both win32 and posix plaftfroms

#include "osal/OsalGeneralDefines.h"  // milliseconds_t
#include "osal/Thread.h"              // PriorityType
#include <boost/thread/thread.hpp>

namespace osal
{
namespace Thread
{
namespace details
{
 
unsigned int Set2PosixPriority(osal::Thread::PriorityType prio)
{
#if !defined(BOOST_THREAD_WIN32)
  static const unsigned int MAX_VALUE = 99; // this is the highst value for priority under Linux
  unsigned int ret = prio % MAX_VALUE;
  return ret;
#else
  return 0; // we don't care for it
#endif
}
  
  // note that the runable object is some object that can be passed
  // to the constructor of the boost::thread object which can be 
  // either global function or some object with operator () in it
  // also note that we do not supprting any parameter passing to the function
  // right now since osal do not support this either
boost::thread Start(osal::Thread::entry_func_t someEntryFunc)
{
  return boost::thread(someEntryFunc);
}

boost::thread Start(const Attributes& att, osal::Thread::entry_func_t someEntryFunc)
{
#if !defined(BOOST_THREAD_WIN32)  // no support for attribute setting under win32
  return boost::thread(someEntryFunc, booost::thread_attributes(att.Name, att.StackSize, Set2PosixPriority(att.Priority)));
#else
  return Start(someEntryFunc);
#endif  // BOOST_THREAD_WIN32
}

void Sleep(milliseconds_t timeout)
{
  boost::this_thread::sleep(boost::posix_time::milliseconds(timeout));
}

void Suspend()
{
  Sleep(1); // this is to ensure that we are actualy waiting on something
}

} // namespace details

namespace 
{
  const PriorityType DEFAULT_PRIORITY = INVALID_PRIORITY;
} // end of local namespace

Attributes::Attributes(const char* name, unsigned int stackSize, PriorityType prio) : 
    Name(name), StackSize(stackSize), Priority(prio)
{ 
}

Attributes::Attributes() : Name(DefaultName()), StackSize(DefaultStackSize()), Priority(DEFAULT_PRIORITY)
{
}

Attributes CreateAttribute(const char* name, unsigned int sSize, PriorityType pt)
{
  return Attributes(name, sSize, pt); 
}

  
} // namespace Thread
} // namespace osal
#else
# error "you must not include this file inside header file"
#endif  // THREAD_DETAIL_IMPLE_HPP
