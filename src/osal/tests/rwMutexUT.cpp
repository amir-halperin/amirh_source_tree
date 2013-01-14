/*
 * This would test read write mutex. you can read more
 * about this mutex in the header file for this module
 */
#include "osal/RWMutex.h" // module under test
#include "osal/Thread.h"  // so we can test with more than one thread - assume that thread was tested
#include <osal/StopWatch.h>    // this for class StopWatch
#include "../OsalTimeUtils.h" // for timeout value
#include <gtest/gtest.h>  // unit test framework
#include <string.h>       // strlen
#include <string>         // class string
#include <iostream>

#if !defined(__VXWORKS__) || defined(INCLUDE_SEM_READ_WRITE)
namespace   // all unit tests are private to this file
{
 
 
osal::RWMutex::Id* mutex4Test = 0;
unsigned int numberOfTimesLocked = 0;
bool finishRunning = false;
bool threadIsRunning = false;
const unsigned int timeoutToBlock =
#ifdef __VXWORKS__
        140;
#else
    osal::TimeUtils::MinResolution()*4;
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
    osal::RWMutex::WriteLock(mutex4Test);
    ++numberOfTimesLocked;
    osal::Thread::Self::Suspend();  // so that we would not burn the CPU;
  }
  // release all the locks we made
  unsigned int toRelease = numberOfTimesLocked;
  while (toRelease > 0)
  {
    osal::RWMutex::Release(mutex4Test);
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
  osal::RWMutex::WriteLock(mutex4Test);  // take the lock and 
  UT::StopWatch sw;
  threadIsRunning = true;
  // then release it after you sleep this would allow
  // to ensure that the lock actually doing something
  lockWasTaken = true;
 
  osal::Thread::Self::Sleep(timeoutToBlock);
  threadIsRunning = false;
  lockWasTaken = false;
  osal::RWMutex::Release(mutex4Test);
}

void SyncReadOnlyFunction()
{
  if (!mutex4Test)
  {
    return;
  }
  threadIsRunning = true;
  lockWasTaken = false;
  osal::RWMutex::ReadLock(mutex4Test);  // take the lock and 
  // then release it after you sleep this would allow
  // to ensure that the lock actually doing something
  lockWasTaken = true;
 
  osal::Thread::Self::Sleep(timeoutToBlock);
  osal::RWMutex::Release(mutex4Test);
  lockWasTaken = false;
  threadIsRunning = false;
  
}

///////////////////////////////////////////////////////////////////////////////

void EnsureNotEqual(osal::RWMutex::Id* left, osal::RWMutex::Id* right)
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
    taken = osal::RWMutex::TryReadLock(mutex4Test);
    bool running = threadIsRunning; // we must catch this value!
    if (running)
    {   // we can only test this if the other thread is still running
      EXPECT_NE(running, taken);  // otherwise we would have a rase condition..
    }
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());
  }
  if (threadIsRunning)
  {
    EXPECT_NE(threadIsRunning, true);
    exit(1);
  }
  
  if (taken)
  {
    osal::RWMutex::Release(mutex4Test);
  }
  osal::Thread::Clean(tid);
  osal::RWMutex::Delete(mutex4Test);
  mutex4Test = 0;
  
}

void TestBlockingMutex(const char* threadName)
{
  threadIsRunning  = false;
  lockWasTaken = false;
  osal::Thread::Id* tid = osal::Thread::Create(osal::Thread::CreateAttribute(threadName, 4048, 
                                                                             osal::Thread::Self::Priority()), 
                                               SyncTestFunction);
  while (!threadIsRunning)
  {
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());  // let the other thread run..
  }
  UT::StopWatch sw;
  EXPECT_EQ(true, lockWasTaken);
  osal::RWMutex::ReadLock(mutex4Test);  // try to take it from this thread, this would block the main thread
  osal::milliseconds_t sleepTime = sw.Stop();
#if !defined(__VXWORKS__)
  sleepTime += osal::TimeUtils::MinResolution();  // we cannot know for sure on non RTOS what the excact time would be
#endif  // __VXWORKS__
  EXPECT_GE(sleepTime, timeoutToBlock);
  osal::RWMutex::Release(mutex4Test);
  osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());  // let the other thread run..
  osal::Thread::Clean(tid);
  osal::RWMutex::Delete(mutex4Test);
  mutex4Test = 0;
  
}

void TestRecursiveMutex(const char* threadName)
{
#if defined(__VXWORKS__)  // this would not work on any other platform 
  osal::Thread::Id* tid = osal::Thread::Create(osal::Thread::CreateAttribute(threadName, 4048, 
                                                                             osal::Thread::Self::Priority()), 
                                                                             RecursiveTestFunction);
  EXPECT_NE(tid, (osal::Thread::Id*)0);
  osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution()*2); // let the other one run for a while
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
    osal::RWMutex::Delete(mutex4Test);   
    mutex4Test = 0;
    return;
  }
  EXPECT_LE(numberOfTimesLocked, maxLock4RecursiveTest);
  osal::RWMutex::Delete(mutex4Test);  // we would create a new mutex here so do a cleanup
  mutex4Test = 0;
  osal::Thread::Clean(tid);
#endif  // __VXWORKS__
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
  bool gotLock = osal::RWMutex::TimedReadLock(mutex4Test, timeout);
  EXPECT_NE(gotLock, (lockWasTaken && threadIsRunning));
  gotLock = osal::RWMutex::TimedReadLock(mutex4Test, timeoutToBlock); // this time we should get the lock
  EXPECT_EQ(threadIsRunning, false);
  EXPECT_NE(threadIsRunning, gotLock);
  if (gotLock)
  {
    osal::RWMutex::Release(mutex4Test);
  }
  osal::Thread::Clean(tid);
  osal::RWMutex::Delete(mutex4Test);
  
}

///////////////////////////////////////////////////////////////////////////////
// Unit test starts here
TEST(MutexUT, BasicTest)
{
  // create some mutex make sure that they are not the same..
  osal::RWMutex::Id* m1 = osal::RWMutex::Create();
  osal::RWMutex::Id* m2 = osal::RWMutex::Create();
  EnsureNotEqual(m1, m2);
  osal::RWMutex::Delete(m1);
  osal::RWMutex::Delete(m2);
}
  
TEST(RWMutexUT, Recursive)
{
  // create a recursive protected mutex, run it in it own thread (so that the main
  // thread that run all tests would not be deadlock if something is not working
  // as it should) and ensure that calling this would not block the new thread
  // create a new mutex this one with the other option
  mutex4Test = osal::RWMutex::Create();
  TestRecursiveMutex("TestRecursive");
  numberOfTimesLocked = 0;
}

TEST(RWMutexUT, BasicLock)
{
  // this would test the basic usage for mutex - that is we have one thread
  // that lock and use some resource and we would like to make sure
  // that we really protecting this resource while another thread is trying to do something
  // similar
  // run this with different type of thread
  mutex4Test = osal::RWMutex::Create();
  TestBlockingMutex("BasicBlocking");
}

TEST(RWMutexUT, TryTest)
{
  // in this test we would test whether using a try call would work
  // this means that if another thread is locking the mutex we cannot
  // access it, and once this lock is not locked any more, we would have
  // access to it
  threadIsRunning  = false;
  mutex4Test = osal::RWMutex::Create();
  TestWithTry("TryTest");
}

TEST(RWMutexUT, TimeoutTest)
{
  // In this test we would test that we are not blocked after timeout value
  // than that we are getting the mutex within the timeout value
  // test for every type of mutex we have
  threadIsRunning = false;
  mutex4Test = osal::RWMutex::Create();
  TestWithTimeout("TimeoutTest");
}

TEST(RWMutexUT, ReadAccess)
{
  // in this test we would like to make sure that two threads
  // can access the same mutex if they lock it with read only access
  mutex4Test = osal::RWMutex::Create();
  osal::Thread::Id* tid = osal::Thread::Create(osal::Thread::CreateAttribute("TestReadAccessT", 4048, 
                                                                             osal::Thread::Self::Priority()),
                                               SyncReadOnlyFunction);
  osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());  // let the other thread start running
  int count = 10;
  while (!threadIsRunning && count--)
  {
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());
  }
  EXPECT_EQ(threadIsRunning, true);
  // at this point the lock was made
  EXPECT_EQ(threadIsRunning, lockWasTaken);
  int locked = 0;
  while (lockWasTaken)
  { // ensure that we can take this as well
    osal::RWMutex::ReadLock(mutex4Test);
    ++locked;
    osal::RWMutex::Release(mutex4Test);
#if defined(__VXWORKS__) // with vxworks we would have context switch but with other OSs this would not be enough
    osal::Thread::Self::Suspend();
#else
    osal::Thread::Self::Sleep(osal::TimeUtils::MinResolution());
#endif  // __VXWORKS__
  }
  EXPECT_GT(locked, 1);
  osal::Thread::Clean(tid);
  osal::RWMutex::Delete(mutex4Test);
  mutex4Test = 0;
}

TEST(RWMutexUT, ReadLockNoWriteAccess)
{
  // in this test we would like to make sure that if
  // the lock was taken by some read lock, then a thread
  // that tries to access with write access cannot get the lock
  mutex4Test = osal::RWMutex::Create();
  osal::Thread::Id* tid = osal::Thread::Create(osal::Thread::CreateAttribute("ReadWriteBlockT", 4048, 
                                                                               osal::Thread::Self::Priority()),
                                               SyncReadOnlyFunction);
  while (!threadIsRunning)
  {
    osal::Thread::Self::Suspend();
  }
  // at this point the lock was made
  EXPECT_EQ(threadIsRunning, lockWasTaken);
  int locked = 0;
  if (lockWasTaken)
  { // ensure that we cannot access the lock
    UT::StopWatch sw;
    osal::RWMutex::WriteLock(mutex4Test);
    ++locked;
    osal::RWMutex::Release(mutex4Test);
    osal::milliseconds_t timeoutLockedout = sw.Stop();
#if !defined(__VXWORKS__)
    timeoutLockedout += osal::TimeUtils::MinResolution();  // we cannot know for sure on non RTOS what the excact time would be
#endif  // __VXWORKS__
    EXPECT_GE(timeoutLockedout, timeoutLockedout);
  }
  else
  {
    EXPECT_EQ(true, lockWasTaken);  // something is wrong
  }
  osal::Thread::Clean(tid);
  osal::RWMutex::Delete(mutex4Test);
  mutex4Test = 0;
}

} // end of local namespace

#endif  // !defined(__VXWORKS__) || defined(INCLUDE_SEM_READ_WRITE)
