/*
 * Unit test for counting sempahore module. For more information about this
 * module you can read the header file comments and the documentation 
 * that were wrote for it as part of the review for this module
 */
#ifdef true
#undef true
#endif
#ifdef false
#undef false
#endif 

#include "osal/CountingSemaphore.h"  // unit under test
#include "osal/Thread.h"  // so we can test with more than one thread - assume that thread was tested
#include <osal/StopWatch.h>    // this for class StopWatch
#include "../OsalTimeUtils.h"  // for timeout value
#include <gtest/gtest.h>  // unit test framework

namespace   // all unit test functions and data are private to this file
{

  
unsigned int countNumberOfTimesAccess = 0;
unsigned int countNumberOfTimesFailAccess = 0;
bool highPriorityThreadTookSem = false;
bool lowPriorityThreadTookSem = false;

osal::CountingSemaphore::Id* countSemId = 0;
const unsigned int TEST_LOOP_COUNT = 5;
bool threadIsRunning = false;
const osal::CountingSemaphore::Id* INVALID_SEM = 0;
const unsigned int POST_SLEEP_TIME = 20;

void ConsumerThread()
{
  if (countSemId)
  {
    countNumberOfTimesAccess = 0;
    threadIsRunning = true;
    // this function would be used in the test to make sure that
    // we can block this thread when it is doing wait on empty
    // semaphore
    for (unsigned int i = 0; i < TEST_LOOP_COUNT; i++)
    {
      osal::CountingSemaphore::Wait(countSemId);
      ++countNumberOfTimesAccess;
    }
  }
  threadIsRunning = false;
}
  
void ConsumerThreadLow()
{
  if (countSemId)
  {
    countNumberOfTimesAccess = 0;
    threadIsRunning = true;
    lowPriorityThreadTookSem = false;
    // this function would be used in the test to make sure that
    // we can block this thread when it is doing wait on empty
    // semaphore
    osal::CountingSemaphore::Wait(countSemId);
    lowPriorityThreadTookSem = true;
    
  }
  threadIsRunning = false;
}
  
void ConsumerThreadHigh()
{
  if (countSemId)
  {
    countNumberOfTimesAccess = 0;
    threadIsRunning = true;
    highPriorityThreadTookSem = false;
    // this function would be used in the test to make sure that
    // we can block this thread when it is doing wait on empty
    // semaphore
    osal::CountingSemaphore::Wait(countSemId);
    highPriorityThreadTookSem = true;
    
  }
  threadIsRunning = false;
}
  
void ConsumerTryThread()
{
  if (countSemId)
  {
    countNumberOfTimesAccess = 0;
    countNumberOfTimesFailAccess  = 0;
    threadIsRunning = true;
    // this function would be used in the test to make sure that
    // that we would no block if the semaphore is not signaled
    // this is to ensure that the try case is really not
    // blocking this thread
    for (unsigned int i = 0; countNumberOfTimesAccess < TEST_LOOP_COUNT; i++)
    {
      if (osal::CountingSemaphore::TryWait(countSemId))
      {
        ++countNumberOfTimesAccess;
      }
      else
      {
        countNumberOfTimesFailAccess++; // count number of times that we failed to get the lock 
      }
    }
  }
  threadIsRunning = false;
}
  
void ConsumerTimeThread()
{
  if (countSemId)
  {
    countNumberOfTimesAccess = 0;
    countNumberOfTimesFailAccess  = 0;
    threadIsRunning = true;
    // we are waiting in this function with timeout, we expcting that
    // in some cases we would not get the lock and in other we will
    // this is done by given a timeout that is half of what the
    // main thread is sleeping
    const osal::milliseconds_t timeout = POST_SLEEP_TIME / 2;
    for (unsigned int i = 0; countNumberOfTimesAccess < TEST_LOOP_COUNT; i++)
    {
      if (osal::CountingSemaphore::TimedWait(countSemId, timeout))
      {
        ++countNumberOfTimesAccess;
      }
      else
      {
        countNumberOfTimesFailAccess++; // count number of times that we failed to get the lock 
      }
    }
  }
  threadIsRunning = false;
}
  
TEST(CountingSemaphoreUT, BasicTest)
{
  // basic test means that we would just create a semaphore and would ensure that
  // we can call the only function that really don't do much which is post (since this function
  // would never block) - we would not want this test to run with multithread
  osal::CountingSemaphore::Id* csid = osal::CountingSemaphore::Create(5);
  osal::CountingSemaphore::Id* csid2 = osal::CountingSemaphore::Create(5);  
  // we would like to make sure that we don't get the same pointer address - this mean that we don't 
  // have any issues with calling these function in multi threaded environment
  EXPECT_NE(csid, csid2);
  // we want to make sure that we don't get delay by doing 4 waits (since the initial count is 5)
  for (int i = 0; i < 5; i++)
  {
    EXPECT_EQ(true, osal::CountingSemaphore::TryWait(csid));
    EXPECT_EQ(true, osal::CountingSemaphore::TryWait(csid2));
  }
  
  for (int i = 0; i < 20; i++)
  {
    osal::CountingSemaphore::Post(csid);
    osal::CountingSemaphore::Post(csid2);
  }
  // after we've made 20 posts, we should be able to do 20 waits without getting locked out
  for (int i = 0; i < 20; i++)
  {
    EXPECT_EQ(true, osal::CountingSemaphore::TryWait(csid));
    EXPECT_EQ(true, osal::CountingSemaphore::TryWait(csid2));
  }
}

TEST(CountingSemaphoreUT, BasicSignaling)
{
  // this function would test the post/wait with 2 threads
  // the main thread would do the post and the second thread
  // would do the wait. We would like to make sure 
  // that the wait would really block the second thread
  threadIsRunning = false;
  countNumberOfTimesAccess = 0;
  countSemId =  osal::CountingSemaphore::Create(1);
  EXPECT_NE(countSemId, INVALID_SEM);
  osal::Thread::Id* tid = osal::Thread::Create(osal::Thread::CreateAttribute("BasicSemTest", 4048, 
                                                                               osal::Thread::Self::Priority()),
                                               ConsumerThread);
  while (!threadIsRunning)
  {   // make sure that the other thread as started
    osal::Thread::Self::Suspend();
  }
  for (unsigned int i = 0; i < TEST_LOOP_COUNT; i++)
  {
    EXPECT_EQ(countNumberOfTimesAccess, i+1u);  // that is it was taken one more than initial i value
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());              // sleep some time and ensure that this is really same value 
    EXPECT_EQ(countNumberOfTimesAccess, i+1u);  // that is it was taken one more than initial i value
    osal::CountingSemaphore::Post(countSemId);  // free the other thread
#if defined(__VXWORKS__)    // on RTOS this would be enough but on other OSs such as win it would not work
    osal::Thread::Self::Suspend();
#else
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());              // sleep some time and ensure that this is really same value 
#endif  // __VXWORKS__
  }
  osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());                // make sure that the other thread has finished
  EXPECT_EQ(threadIsRunning, false);
  osal::Thread::Clean(tid);
  osal::CountingSemaphore::Delete(countSemId);
  countNumberOfTimesAccess = 0;
}

TEST(CountingSemaphoreUT, TrySignaling)
{
  // this function would test the post/wait with 2 threads
  // the main thread would do the post and the second thread
  // would do the wait. We would like to make sure 
  // that the wait would really block the second thread
  threadIsRunning = false;
  countNumberOfTimesAccess = 0;
  countSemId =  osal::CountingSemaphore::Create(1);
  EXPECT_NE(countSemId, INVALID_SEM);
  
  osal::Thread::Id* tid = osal::Thread::Create(osal::Thread::CreateAttribute("BasicSemTest", 4048, 
                                                                               osal::Thread::Self::Priority()),
                                               ConsumerTryThread);
  unsigned int tryFailed = countNumberOfTimesFailAccess; 
  while (!threadIsRunning)
  {   // make sure that the other thread as started
    osal::Thread::Self::Suspend();
  }
  
  for (unsigned int i = 0; i < TEST_LOOP_COUNT; i++)
  {
    EXPECT_LT(tryFailed, countNumberOfTimesFailAccess);
    EXPECT_EQ(countNumberOfTimesAccess, i+1u);  // that is it was taken one more than initial i value
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());              // sleep some time and ensure that this is really same value 
    EXPECT_EQ(countNumberOfTimesAccess, i+1u);  // that is it was taken one more than initial i value
    EXPECT_LT(tryFailed, countNumberOfTimesFailAccess);
    osal::CountingSemaphore::Post(countSemId);  // free the other thread
#if defined(__VXWORKS__)    // on RTOS this would be enough but on other OSs such as win it would not work
    osal::Thread::Self::Suspend();
#else
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());              // sleep some time and ensure that this is really same value 
#endif  // __VXWORKS__
  }
  osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());                // make sure that the other thread has finished
  EXPECT_EQ(threadIsRunning, false);
  osal::Thread::Clean(tid);
  osal::CountingSemaphore::Delete(countSemId);
  countNumberOfTimesAccess = 0;
}

TEST(CountingSemaphoreUT, TimeSignaling)
{
  // this function would test the post/wait with 2 threads
  // the main thread would do the post and the second thread
  // would do the wait. We would like to make sure 
  // that the wait would really block the second thread
  threadIsRunning = false;
  countNumberOfTimesAccess = 0;
  countNumberOfTimesFailAccess = 0;
  countSemId =  osal::CountingSemaphore::Create(1);
  EXPECT_NE(countSemId, INVALID_SEM);
  osal::Thread::Id* tid = osal::Thread::Create(osal::Thread::CreateAttribute("BasicSemTest", 4048, 
                                                                               osal::Thread::Self::Priority()),
                                               ConsumerTimeThread);
  unsigned int timeFailed = countNumberOfTimesFailAccess; 
  while (!threadIsRunning)
  {   // make sure that the other thread as started
    osal::Thread::Self::Suspend();
  }
  
  for (unsigned int i = 0; i < TEST_LOOP_COUNT; i++)
  {
    EXPECT_LE(timeFailed, countNumberOfTimesFailAccess);
    EXPECT_EQ(countNumberOfTimesAccess, i+1u);  // that is it was taken one more than initial i value
    osal::Thread::Self::Sleep(POST_SLEEP_TIME); // sleep some time and ensure that this is really same value 
    EXPECT_EQ(countNumberOfTimesAccess, i+1u);  // that is it was taken one more than initial i value
    EXPECT_LE(timeFailed, countNumberOfTimesFailAccess);
    osal::CountingSemaphore::Post(countSemId);  // free the other thread
    timeFailed = countNumberOfTimesFailAccess;
#if defined(__VXWORKS__)    // on RTOS this would be enough but on other OSs such as win it would not work
    osal::Thread::Self::Suspend();
#else
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());              // sleep some time and ensure that this is really same value 
#endif  // __VXWORKS__
  }
  osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());                // make sure that the other thread has finished
  EXPECT_EQ(threadIsRunning, false);
  osal::Thread::Clean(tid);
  osal::CountingSemaphore::Delete(countSemId);
  countNumberOfTimesAccess = 0;
}

TEST(CountingSemaphoreUT, MultiThreadPriority)
{
#if defined(__VXWORKS__)  // this can only work with RTOS
  threadIsRunning = false;
  countNumberOfTimesAccess = 0;
  countSemId = osal::CountingSemaphore::Create(1);
  EXPECT_NE(countSemId, INVALID_SEM);
  EXPECT_EQ(osal::CountingSemaphore::TryWait(countSemId), true);
  // start the two threads
  // the first to start is the low priority thread
  // we don't expect it to take the semaphore as this semaphore is empty
  osal::Thread::Id* tid1 = osal::Thread::Create(osal::Thread::CreateAttribute("BasicSemTest", 4048, 
                                                                               osal::Thread::PRIORITY_24),
                                               ConsumerThreadLow);
  osal::Thread::Self::Sleep(10);  // make sure that the thread with lower priority started first
  osal::Thread::Id* tid = osal::Thread::Create(osal::Thread::CreateAttribute("BasicSemTest", 4048, 
                                                                               osal::Thread::PRIORITY_19),
                                               ConsumerThreadHigh);
  osal::Thread::Self::Sleep(10);    // let the other threads start
  // make sure that niether of them took the lock
  EXPECT_EQ(true, threadIsRunning);
  EXPECT_EQ(false, lowPriorityThreadTookSem);
  EXPECT_EQ(false, highPriorityThreadTookSem); 
  // sleep some make sure this is the same
  osal::Thread::Self::Sleep(10);    // let the other threads start
  EXPECT_EQ(true, threadIsRunning);
  EXPECT_EQ(false, lowPriorityThreadTookSem);
  EXPECT_EQ(false, highPriorityThreadTookSem); 
  
  // now free the sempahore - make sure that the higher priority thread
  // that started with second would still take the semaphore because semaphore is given to the
  // higher priority thread and not to the first one
  osal::CountingSemaphore::Post(countSemId);
  osal::Thread::Self::Suspend();
  EXPECT_EQ(false, lowPriorityThreadTookSem);
  EXPECT_EQ(true, highPriorityThreadTookSem); 
  // after that we would like to make sure that the other thread would get the
  // sempahore as well if we would post again
  osal::Thread::Self::Sleep(10);
  EXPECT_EQ(false, lowPriorityThreadTookSem);
  osal::CountingSemaphore::Post(countSemId);
  osal::Thread::Self::Suspend();
  
  EXPECT_EQ(true, lowPriorityThreadTookSem);
  osal::Thread::Clean(tid);
  osal::Thread::Clean(tid1);
  osal::CountingSemaphore::Delete(countSemId);
#endif  // __VXWORKS__
}

} // end of local namespace

