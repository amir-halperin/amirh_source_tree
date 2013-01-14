/*
 * This is a unit test file for the OSAL implementation for threads
 */
#ifdef true
#undef true
#endif
#ifdef false
#undef false
#endif 

#include "osal/Thread.h"  // for the code under test
#include "../OsalTimeUtils.h"  // timeout value
#include <osal/StopWatch.h>    // this for class StopWatch
#include <gtest/gtest.h>  // unit test framework
#include <string>         // class string

namespace   // al tests would be hide from the outside of this compilation unit
{
 

  
bool threadIsRunning = false;
bool stopRunning = true;
const osal::Thread::Id* INVALID_TID = 0;
  
void ThreadFunction();

void ThreadFunction()
{
  threadIsRunning = true;
  while (!stopRunning)
  {
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());
  }
  threadIsRunning = false;
}

  
TEST(ThreadUnitTest, BasicUsage)
{
  const char* name = "BasicTName";
  unsigned int stackSize = 1024;
  osal::Thread::PriorityType priority = osal::Thread::PRIORITY_29;
  osal::Thread::Attributes threadTestAtt(name, stackSize, priority);
  
  EXPECT_EQ(std::string(name), std::string(threadTestAtt.Name));
  EXPECT_EQ(stackSize, threadTestAtt.StackSize);
  EXPECT_EQ(priority, threadTestAtt.Priority);
  // create default set of attributes 
  osal::Thread::Attributes threadTestAtt2;  // check the defaullt settings
  EXPECT_NE(std::string(name), std::string(threadTestAtt2.Name));
  EXPECT_NE(stackSize, threadTestAtt2.StackSize);
  EXPECT_NE(priority, threadTestAtt2.Priority);
  
  EXPECT_EQ(std::string(osal::Thread::DefaultName()), std::string(threadTestAtt2.Name));
  EXPECT_EQ(osal::Thread::DefaultStackSize(), threadTestAtt2.StackSize);
  EXPECT_EQ(osal::Thread::INVALID_PRIORITY, threadTestAtt2.Priority);
  // this is a create function make sure that this is the same as threadTestAtt
  osal::Thread::Attributes ta = osal::Thread::CreateAttribute(name, stackSize, priority);
  EXPECT_EQ(std::string(ta.Name), std::string(threadTestAtt.Name));
  EXPECT_EQ(ta.StackSize, threadTestAtt.StackSize);
  EXPECT_EQ(ta.Priority, threadTestAtt.Priority);
  EXPECT_EQ(std::string(name), std::string(ta.Name));
  EXPECT_EQ(stackSize, ta.StackSize);
  EXPECT_EQ(priority, ta.Priority);
  
  // now run the thread and make sure that it does something here
  stopRunning = false;
  osal::Thread::Id* tid1 = osal::Thread::Create(threadTestAtt, ThreadFunction);
  osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());  // let other thread start running
  if (tid1 == NULL)
  {
    EXPECT_NE(INVALID_TID, tid1);  // this can never happen
    return;                 // there is no point to continue
  }
  EXPECT_EQ(std::string(name), std::string(osal::Thread::Name(tid1)));
  EXPECT_EQ(priority, osal::Thread::Priority(tid1));
  EXPECT_EQ(threadIsRunning, true);
  // ensure that calling the priority function didn't change the priority!
#if defined(__VXWORKS__) // this would not work under windows
  EXPECT_EQ(priority, osal::Thread::Priority(tid1));
  osal::Thread::Id* selfId = osal::Thread::Self::Id();
  EXPECT_NE(selfId, INVALID_TID);
  EXPECT_EQ(osal::Thread::EqualsId(selfId, tid1), false);
  osal::Thread::Clean(selfId);
#endif  // __VXWORKS__
  // set new priority to the task
  osal::Thread::PriorityType oldPrio = osal::Thread::Priority(tid1);
  osal::Thread::NewPriority(tid1, osal::Thread::PRIORITY_48);
  EXPECT_EQ(oldPrio, priority);
  EXPECT_NE(priority, osal::Thread::Priority(tid1));
  EXPECT_EQ(osal::Thread::PRIORITY_48, osal::Thread::Priority(tid1));
  stopRunning = true;
  osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());  // make sure that the other thread would have the chance to get the message
  osal::Thread::Clean(tid1);
}

TEST(ThreadUnitTest, SelfThreadTest)
{
  // this test would not work under windows
#if defined(__VXWORKS__)
  // this one is just to make sure that some functions that work on the current thread are
  // really doing what we expect them to do
  osal::milliseconds_t timeoutValue = 400;  // this is milli value
  UT::StopWatch sw;
  osal::Thread::Self::Sleep(timeoutValue);
  osal::milliseconds_t sleepTime = sw.Pause();
  EXPECT_GE(sleepTime, timeoutValue);       // it may be larger because of rounding and other tasks that are running
  // now lets play with priority
  osal::Thread::PriorityType currentPriority = osal::Thread::Self::Priority();  // get priority
  osal::Thread::PriorityType newPriority = osal::Thread::PRIORITY_18; 
  if (newPriority == currentPriority)
  {
    newPriority = osal::Thread::PRIORITY_19;
  }
  osal::Thread::PriorityType oldPriority = osal::Thread::Self::Priority();
  osal::Thread::Self::NewPriority(newPriority);
  EXPECT_EQ(oldPriority, currentPriority);
  currentPriority = osal::Thread::Self::Priority();
  EXPECT_EQ(currentPriority, newPriority);
#endif  // __VXWORKS__
}
  
} // end of local namespace
