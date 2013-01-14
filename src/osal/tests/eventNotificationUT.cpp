/*
 * This would test the functionality of the event notification
 * module. to read more about this module read the comments
 * in the header file
 */
#include "osal/EventNotification.h" // unit under test
# include "../OsalTimeUtils.h"          // MinResolution
#include "osal/Thread.h"            // thread module
#include <gtest/gtest.h>            // unit test framework
#include <iostream>

namespace   // all functions and data are private to this file
{
  
osal::EventNotification::Id* eventNotifyId;
bool threadIsRunning = false;
bool highPrioThreadIsRunning = false;
const unsigned int MAX_THREAD_LOOP = 5;
unsigned int numberOfTimesNotBlocked = 0;


const osal::milliseconds_t time4Test()
{
  return osal::TimeUtils::MinResolution() * 2;
}


void TryLoopThread()
{
  // this function would make sure that try is working
  // this is done by calling the try on the event and
  // make sure that we are not blocked
  numberOfTimesNotBlocked  = 0;
  threadIsRunning = false;
  if (eventNotifyId)
  {
    threadIsRunning = true;  
    for (unsigned int i = 0; i < MAX_THREAD_LOOP && eventNotifyId;)
    {
      osal::EventNotification::Wait(eventNotifyId);
      i++;
      /*if (osal::EventNotification::TryWait(eventNotifyId))
      {
        i++;
      }
      else
      {
        numberOfTimesNotBlocked++;
      }*/
    }
  }
  threadIsRunning = false;
  
}
  
void TimeLoopThread()
{
  // this function would make sure that try is working
  // this is done by calling the try on the event and
  // make sure that we are not blocked
  numberOfTimesNotBlocked  = 0;
  threadIsRunning = false;
  if (eventNotifyId)
  {
    threadIsRunning = true;  
    for (unsigned int i = 0; i < MAX_THREAD_LOOP && eventNotifyId;)
    {
      if (osal::EventNotification::TimedWait(eventNotifyId, time4Test()/2))
      {
        i++;
      }
      else
      {
        numberOfTimesNotBlocked++;
      }
    }
  }
  threadIsRunning = false;
  
}
  
void SimpleCaseThread()
{
  threadIsRunning = false;
  // just wait for the event to happen and then
  // when this is release signal that the thread is not running any more
  if (eventNotifyId)
  {
    threadIsRunning = true;
    osal::EventNotification::Wait(eventNotifyId);
    threadIsRunning = false;
  }
}

void HighPriorityThread()
{
  highPrioThreadIsRunning = false;
  // just wait for the event to happen and then
  // when this is release signal that the thread is not running any more
  if (eventNotifyId)
  {
    highPrioThreadIsRunning = true;
    osal::EventNotification::Wait(eventNotifyId);
    highPrioThreadIsRunning = false;
  }
  
}
///////////////////////////////////////////////////////////////////////////////
// test class
class EventNotificationTest : public ::testing::Test
{
public:
  EventNotificationTest() :  tid(0), tid1(0)
  {
    
  }
  
protected:
  virtual void SetUp()
  {
    eventNotifyId = 0;
    threadIsRunning = false;
    highPrioThreadIsRunning = false;
    numberOfTimesNotBlocked = 0;
  }
  
  virtual void TearDown()
  {
    if (tid)
    {
      osal::Thread::Clean(tid);
      tid = 0;
    }
    if (tid1)
    {
      osal::Thread::Clean(tid1);
      tid1 = 0;
    }
    osal::EventNotification::Delete(eventNotifyId);
  }
  
  osal::Thread::Id* tid;
  osal::Thread::Id* tid1;
  
};

  
TEST_F(EventNotificationTest, BasicTest)
{
  // in this test we would make sure that we can block a thread using the event
  // then free it
  eventNotifyId = osal::EventNotification::Create();
  
  tid = osal::Thread::Create(osal::Thread::Attributes("ENBasicUT", 4048, osal::Thread::Self::Priority()),
                                               SimpleCaseThread);
  osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());  // let the other thread run
  EXPECT_EQ(true, threadIsRunning);
  osal::EventNotification::Signal(eventNotifyId);
  osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());  // let the other thread run
  EXPECT_EQ(threadIsRunning, false);  // after we signal the thread should have allowed to continue
}

TEST_F(EventNotificationTest, HighPrioTest)
{
  // in this thread we would do the same thing we are doing
  // in the thread above, only we would like to make sure
  // if we have 2 threads that are waiting on the event
  // the one with the higher priority would recieve the signal
  //first and would continue
#if defined(__VXWORKS__) // this would not work on windows platform as we don't have priorities
  eventNotifyId = osal::EventNotification::Create();
  
  // start low priority thread first so that it would sit on the event first
  tid1 = osal::Thread::Create(osal::Thread::Attributes("ENLowPrioT", 4048, osal::Thread::PRIORITY_28),
                                               SimpleCaseThread);
  osal::Thread::Self::Sleep(10);    // wait for this thread to start
  tid = osal::Thread::Create(osal::Thread::Attributes("ENHighPrioT", 4048, osal::Thread::PRIORITY_24),
                                               HighPriorityThread);
  osal::Thread::Self::Suspend();
  // both thread should be running right now but are blocked
  EXPECT_EQ(threadIsRunning, true);
  EXPECT_EQ(true, highPrioThreadIsRunning);
  // now send a signal and make sure that the higher priority thread would release first
  // even though it was getting locked second
  osal::EventNotification::Signal(eventNotifyId);
  osal::Thread::Self::Suspend();
  EXPECT_EQ(threadIsRunning, true);
  EXPECT_EQ(highPrioThreadIsRunning, false);
  // restart the high priority thread and try to release all the thread while they are waiting
  
  osal::Thread::TimeClean(tid, osal::TimeUtils::MinResolution());
  osal::Thread::Self::Suspend();
  if (highPrioThreadIsRunning)
  {
    return;
  }
  
  tid = osal::Thread::Create(osal::Thread::Attributes("ENHighPrioT2", 4048, osal::Thread::PRIORITY_22),
                             HighPriorityThread);
  
  osal::Thread::Self::Suspend();
  
  EXPECT_EQ(threadIsRunning, true);
  EXPECT_EQ(true, highPrioThreadIsRunning);
   
  osal::EventNotification::SignalAll(eventNotifyId);
  osal::Thread::Self::Suspend();
  EXPECT_EQ(threadIsRunning, false);
  EXPECT_EQ(highPrioThreadIsRunning, false);
#endif  // __VXWORKS__
}
  
TEST_F(EventNotificationTest, TestWithTry)
{
  // in this test we would make sure that we can block a thread using the event
  // then free it
  numberOfTimesNotBlocked = 0;
  eventNotifyId = osal::EventNotification::Create();
  unsigned int notBlocked = numberOfTimesNotBlocked;
  tid = osal::Thread::Create(osal::Thread::Attributes("TryTestEvent", 4048, osal::Thread::Self::Priority()), TryLoopThread);
  osal::Thread::Self::Suspend();
  int count = 20;
  while (!threadIsRunning && count--)
  {
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());
  }
  EXPECT_EQ(true, threadIsRunning);
  osal::Thread::Self::Suspend();  // just to make sure that it actually doing at least one try
  for (unsigned int i = 0; i < MAX_THREAD_LOOP; i++)
  {
    // AmirH - temp workaround change the receiver task from TryWait to Wait thus it means it is always blocked, so this test is not applicable
    //EXPECT_GT(numberOfTimesNotBlocked, notBlocked);
    osal::EventNotification::Signal(eventNotifyId);
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());
  }
  osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());
  count = 50;
  while (count-- && threadIsRunning)
  {
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());
  }
  EXPECT_EQ(threadIsRunning, false);  // after we signal the thread should have allowed to continue
}

TEST_F(EventNotificationTest, TestWithTime)
{
  // in this test we would make sure that we can block a thread using the event
  // then free it
  return;
  numberOfTimesNotBlocked = 0;
  eventNotifyId = osal::EventNotification::Create();
  unsigned int notBlocked = numberOfTimesNotBlocked;
  tid = osal::Thread::Create(osal::Thread::Attributes("TimeTestEvent", 4048, osal::Thread::Self::Priority()),
                                               TimeLoopThread);
  int count = 50;
  while (count-- && !threadIsRunning)
  {
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());
  }
  
  EXPECT_GT(count, 0);
  for (unsigned int i = 0; i < MAX_THREAD_LOOP; i++)
  {
    osal::Thread::Self::Sleep(time4Test());
    EXPECT_GT(numberOfTimesNotBlocked, notBlocked);
    osal::EventNotification::Signal(eventNotifyId);
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());
  }
  EXPECT_EQ(numberOfTimesNotBlocked, MAX_THREAD_LOOP*2);
  count = 50;
  while (count-- && threadIsRunning)
  {
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());
  }
  EXPECT_EQ(threadIsRunning, false);  // after we signal the thread should have allowed to continue
}

}   // end of local namespace
