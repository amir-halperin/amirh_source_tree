/*
 * This is a collection of unit tests for mutex module
 * for more details about the mutex module you can read
 * in the header file hf_src/hf_interf/osal/Mutex.h and
 * any design document
 */
#include "osal/Mutex.h"   // module under test
#include "osal/Thread.h"  // so we can test with more than one thread - assume that thread was tested
#include <osal/StopWatch.h>    // this for class StopWatch
#include "../OsalTimeUtils.h" // for timeout value
#include <gtest/gtest.h>  // unit test framework
#include <iostream>

namespace   // all tests are private to other parts of the code base 
{
  
osal::Mutex::Id* mutex4Test = 0;
unsigned int numberOfTimesLocked = 0;
bool finishRunning = false;
bool threadIsRunning = false;
const unsigned int timeoutToBlock =
#ifdef __VXWORKS__
        140;
#else
    osal::TimeUtils::MinResolution();
#endif // __VXWORKS__
bool lockWasTaken = false;
const unsigned int maxLock4RecursiveTest = 12;

void RecursiveTestFunction()
{
  numberOfTimesLocked = 0;
  if (!mutex4Test)
  {
    return;
  }
  threadIsRunning = true;
  while (!finishRunning && numberOfTimesLocked < maxLock4RecursiveTest)
  {
    osal::Mutex::Lock(mutex4Test);
    ++numberOfTimesLocked;
    osal::Thread::Self::Suspend();  // so that we would not burn the CPU;
  }
  // release all the locks we made
  unsigned int toRelease = numberOfTimesLocked;
  while (toRelease > 0)
  {
    osal::Mutex::Release(mutex4Test);
    --toRelease;
  }
  threadIsRunning = false;
}

void SyncTestFunction()
{
  if (!mutex4Test)
  {
    return;
  }
  lockWasTaken = false;
  osal::Mutex::Lock(mutex4Test);  // take the lock and 
  threadIsRunning = true;
  // then release it after you sleep this would allow
  // to ensure that the lock actually doing something
  lockWasTaken = true;
 
  osal::Thread::Self::Sleep(timeoutToBlock);
  threadIsRunning = false;
  osal::Mutex::Release(mutex4Test);
}

///////////////////////////////////////////////////////////////////////////////

void EnsureNotEqual(osal::Mutex::Id* left, osal::Mutex::Id* right)
{
  EXPECT_NE(left, right);
}
  
void TestWithTry(const char* threadName)
{
  
  osal::Thread::Id* tid = osal::Thread::Create(osal::Thread::CreateAttribute(threadName, 4048, 
                                                                             osal::Thread::Self::Priority()), 
                                               SyncTestFunction);
  while (!threadIsRunning)
  {
    osal::Thread::Self::Suspend();  // let the other thread run..
  }
  
  bool taken = false;
  int count = 20;
  while (threadIsRunning && count--)
  {
    taken = osal::Mutex::TryLock(mutex4Test);
    bool running = threadIsRunning; // we must catch this value!
    if (running)
    {   // we can only test this if the other thread is still running
      EXPECT_NE(running, taken);  // otherwise we would have a rase condition..
    }
    //osal::Thread::Self::Suspend();
    osal::Thread::Self::Sleep(100);
  }
  
  if (taken)
  {
    osal::Mutex::Release(mutex4Test);
  }
  EXPECT_EQ(osal::Thread::Clean(tid), true);
  osal::Mutex::Delete(mutex4Test);
  
}

void TestBlockingMutex(const char* threadName)
{
  threadIsRunning  = false;
  osal::Thread::Id* tid = osal::Thread::Create(osal::Thread::CreateAttribute(threadName, 4048, 
                                                                             osal::Thread::Self::Priority()), 
                                               SyncTestFunction);
  while (!threadIsRunning)
  {
    osal::Thread::Self::Suspend();  // let the other thread run..
  }
  UT::StopWatch sw;
  EXPECT_EQ(true, lockWasTaken);
  //unsigned long ticks = tickGet();
  osal::Mutex::Lock(mutex4Test);  // try to take it from this thread, this would block the main thread
  osal::milliseconds_t sleepTime = sw.Stop();
#if !defined(__VXWORKS__) // this is because on non RTOS the time is not good enough
  sleepTime += osal::TimeUtils::MinResolution()/2;
#endif  // __VXWORKS__
  //unsigned long ticks2 = tickGet();
  //printf("was locked out for %lu milliseconds (%lu ticks)", sleepTime, ticks2 - ticks);
  EXPECT_GE(sleepTime, timeoutToBlock);
  osal::Mutex::Release(mutex4Test);
  osal::Mutex::Delete(mutex4Test);
  osal::Thread::Clean(tid);
  
}

void TestRecursiveMutex(const char* threadName)
{
  osal::Thread::Id* tid = osal::Thread::Create(osal::Thread::CreateAttribute(threadName, 4048, 
                                                                             osal::Thread::Self::Priority()), 
                                                                             RecursiveTestFunction);
  EXPECT_NE(tid, (osal::Thread::Id*)0);
  osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution()); // let the other one run for a while
  finishRunning = true;           // tell other thread to stop
  int tries = 5;
  while (threadIsRunning && tries > 0)
  {
    --tries;
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution()); // let the other one run for a while
  }
  if (threadIsRunning)
  {
    EXPECT_NE(threadIsRunning, true);
    osal::Thread::TimeClean(tid, osal::TimeUtils::MinResolution()); // force kill this thread!
    osal::Mutex::Delete(mutex4Test);   
    return;
  }
  EXPECT_LE(numberOfTimesLocked, maxLock4RecursiveTest);
  osal::Mutex::Delete(mutex4Test);  // we would create a new mutex here so do a cleanup
  osal::Thread::Clean(tid);
  
}

void TestWithTimeout(const char* threadName)
{
  osal::Thread::Id* tid = osal::Thread::Create(osal::Thread::CreateAttribute(threadName, 4048, 
                                                                             osal::Thread::Self::Priority()), 
                                               SyncTestFunction);
  
  while (!threadIsRunning)
  {
    osal::Thread::Self::Suspend();  // let the other thread run..
  }
  EXPECT_EQ(true, lockWasTaken);
  // first test that we cannot get the lock in time..
  osal::milliseconds_t timeout = timeoutToBlock / 2;
  bool gotLock = osal::Mutex::TimedLock(mutex4Test, timeout);
  EXPECT_NE(gotLock, (lockWasTaken && threadIsRunning));
  gotLock = osal::Mutex::TimedLock(mutex4Test, timeoutToBlock); // this time we should get the lock
  EXPECT_EQ(threadIsRunning, false);
  EXPECT_NE(threadIsRunning, gotLock);
  if (gotLock)
  {
    osal::Mutex::Release(mutex4Test);
  }
  osal::Mutex::Delete(mutex4Test);
  osal::Thread::Clean(tid);
  
}

///////////////////////////////////////////////////////////////////////////////
// Unit test starts here
TEST(MutexUT, BasicTest)
{
  // create some mutex make sure that they are not the same..
  osal::Mutex::Id* m1 = osal::Mutex::Create();
  osal::Mutex::Id* m2 = osal::Mutex::Create();
  EnsureNotEqual(m1, m2);
  osal::Mutex::Delete(m1);
  osal::Mutex::Delete(m2);
  m1 = osal::Mutex::CreateRecursive(osal::Mutex::PRIORITY_SAFE);
  m2 = osal::Mutex::CreateRecursive(osal::Mutex::PRIORITY_SAFE);
  EnsureNotEqual(m1, m2);
  osal::Mutex::Id* m3 = osal::Mutex::Create();
  EnsureNotEqual(m3, m2);
  EnsureNotEqual(m3, m1);
  osal::Mutex::Id* m4 = osal::Mutex::CreateRecursive(osal::Mutex::PRIORITY_UNSAFE);
  osal::Mutex::Id* m5 = osal::Mutex::CreateRecursive(osal::Mutex::PRIORITY_UNSAFE);
  EnsureNotEqual(m3, m4);
  EnsureNotEqual(m3, m5);
  EnsureNotEqual(m1, m4);
  EnsureNotEqual(m2, m5);
  osal::Mutex::Delete(m1);
  osal::Mutex::Delete(m2);
  osal::Mutex::Delete(m3);
  osal::Mutex::Delete(m4);
  osal::Mutex::Delete(m5);
}
  
TEST(MutexUT, Recursive)
{
  // create a recursive protected mutex, run it in it own thread (so that the main
  // thread that run all tests would not be deadlock if something is not working
  // as it should) and ensure that calling this would not block the new thread
  // create a new mutex this one with the other option
  mutex4Test = osal::Mutex::CreateRecursive(osal::Mutex::PRIORITY_UNSAFE);
  TestRecursiveMutex("TestRecursive");
  numberOfTimesLocked = 0;
  
  mutex4Test = osal::Mutex::CreateRecursive(osal::Mutex::PRIORITY_SAFE);
  TestRecursiveMutex("TestRecursive2");
  numberOfTimesLocked = 0;
  // now we would do the same with a mutex that is not recursive safe, this would block the
  // thread that we are creating for testing, so we would have to kill it
  threadIsRunning = false;
  finishRunning = false;
  numberOfTimesLocked = 0;
  mutex4Test = osal::Mutex::Create();
  osal::Thread::Id* tid = osal::Thread::Create(
      osal::Thread::CreateAttribute("TestThread3", 4048, 
                                    osal::Thread::PRIORITY_36), 
                                    RecursiveTestFunction);
  osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution()*5); // let the other one run for a while
  finishRunning = true;           // tell other thread to stop
  osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution()); // ensure that the other thread is getting CPU time
  int tries = 5;
  while (tries--)
  {
    EXPECT_EQ(threadIsRunning, true);
    EXPECT_EQ(numberOfTimesLocked, 1u);
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution()); // ensure that the other thread is getting CPU time
  }
  osal::Thread::TimeClean(tid, osal::TimeUtils::MinResolution()); // force kill this thread!
  osal::Mutex::Delete(mutex4Test);  // we would create a new mutex here so do a cleanup
}

TEST(MutexUT, BasicLock)
{
  // this would test the basic usage for mutex - that is we have one thread
  // that lock and use some resource and we would like to make sure
  // that we really protecting this resource while another thread is trying to do something
  // similar
  // run this with different type of thread
  mutex4Test = osal::Mutex::Create();
  TestBlockingMutex("BasicBlocking");
  
  threadIsRunning  = false;
  mutex4Test = osal::Mutex::CreateRecursive(osal::Mutex::PRIORITY_SAFE);
  TestBlockingMutex("BasicBlocking2");
  // and last run this with the third type of mutex
  threadIsRunning  = false;
  mutex4Test = osal::Mutex::CreateRecursive(osal::Mutex::PRIORITY_UNSAFE);
  TestBlockingMutex("BasicBlocking3");
}

TEST(MutexUT, TryTest)
{
  // in this test we would test whether using a try call would work
  // this means that if another thread is locking the mutex we cannot
  // access it, and once this lock is not locked any more, we would have
  // access to it
  threadIsRunning  = false;
  mutex4Test = osal::Mutex::Create();
  TestWithTry("TryTest");
  // run this with different type of thread
  threadIsRunning  = false;
  mutex4Test = osal::Mutex::CreateRecursive(osal::Mutex::PRIORITY_SAFE);
  
  TestWithTry("TryTest2");
  // and last run this with the third type of mutex
  threadIsRunning  = false;
  mutex4Test = osal::Mutex::CreateRecursive(osal::Mutex::PRIORITY_UNSAFE);
  TestWithTry("TryTest3");
}

TEST(MutexUT, TimeoutTest)
{
  // In this test we would test that we are not blocked after timeout value
  // than that we are getting the mutex within the timeout value
  // test for every type of mutex we have
  threadIsRunning = false;
  mutex4Test = osal::Mutex::Create();
  TestWithTimeout("TimeoutTest");
  
  threadIsRunning = false;
  mutex4Test = osal::Mutex::CreateRecursive(osal::Mutex::PRIORITY_SAFE);
  TestWithTimeout("TimeoutTest2");
  
  threadIsRunning = false;
  mutex4Test = osal::Mutex::CreateRecursive(osal::Mutex::PRIORITY_UNSAFE);
  TestWithTimeout("TimeoutTest3");
}

} // end of local namespace

