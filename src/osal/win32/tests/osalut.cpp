#include "osal/Mutex.h"
#include "osal/MessageQueue.h"
#include "osal/Thread.h"
#include "osal/CountingSemaphore.h"
#include "osal/EventNotification.h"
#include "../mocks/include/EventNotificationFwd.h" // definition for the id
#include "../mocks/include/ThreadFwd.h" // definition for the id
#include "../mocks/include/MutexFwd.h" // definition for the id
#include "../mocks/include/CountingSemaphoreFwd.h" // definition for the id
#include "osal/RWMutex.h"
#include <gtest/gtest.h>
#include <string>

namespace
{

unsigned int dummyFunctionCalls = 0;

void dummyFunction()
{
  dummyFunctionCalls++;
}
  
TEST(OsalTest, ThreadTest)
{
  osal::Thread::Id* id = osal::Thread::Create(osal::Thread::CreateAttribute("Thread", 1024, osal::Thread::PRIORITY_100), dummyFunction);
  // make sure that we didn't call this function at all
  EXPECT_EQ(0u, dummyFunctionCalls);
  osal::Thread::Id* id2 = osal::Thread::Create(osal::Thread::CreateAttribute("Thread2", 1024, osal::Thread::PRIORITY_100), dummyFunction);
  EXPECT_EQ(0u, dummyFunctionCalls);
  
  EXPECT_CALL(id->mData, Name());
  EXPECT_CALL(id2->mData, Name());
  EXPECT_CALL(id->mData, Priority());
  EXPECT_CALL(id2->mData, Priority());
  EXPECT_NE(osal::Thread::EqualsId(id, id2), true);
  EXPECT_EQ(osal::Thread::Self::EqualName("someName"), true); // function would always return true
  EXPECT_EQ(osal::Thread::Self::Equel(id2), true); // function would always return true
  EXPECT_EQ(osal::Thread::Self::Equel(id), true); // function would always return true
  EXPECT_EQ(osal::Thread::Self::Sleep(100), 0u); // function would always return 0
  EXPECT_EQ(osal::Thread::Self::Priority(), osal::Thread::LOWSET_PRIORITY); // function would always return LOWSET_PRIORITY
  EXPECT_EQ(osal::Thread::Priority(id), osal::Thread::LOWSET_PRIORITY); // function would always return LOWSET_PRIORITY
  EXPECT_EQ(osal::Thread::Priority(id2), osal::Thread::LOWSET_PRIORITY); // function would always return LOWSET_PRIORITY
  osal::Thread::Self::NewPriority(osal::Thread::PRIORITY_104); // try to set new priority, make sure that it 
  EXPECT_EQ(osal::Thread::Self::Priority(), osal::Thread::LOWSET_PRIORITY); // doesn't have any effect
  
  EXPECT_EQ(std::string(osal::Thread::Name(id)), std::string("TaskMock"));
  EXPECT_EQ(std::string(osal::Thread::Name(id2)), std::string("TaskMock"));
  
  EXPECT_EQ(osal::Thread::Clean(id), true);
  EXPECT_EQ(osal::Thread::Clean(id2), true);
}

TEST(OsalTest, MessageQueueTest)
{
  osal::MessageQueue::Id* mqId = osal::MessageQueue::Create(10, 20);  // note that we don't really using these values
  const char* msg = "this is a test message";
  osal::MessageQueue::Send(mqId, msg, strlen(msg), true);
  EXPECT_EQ(osal::MessageQueue::CurrentCount(mqId), 1);
  const char* msg2 = "this is a second test message";
  osal::MessageQueue::Send(mqId, msg2, strlen(msg2), true);
  EXPECT_EQ(osal::MessageQueue::CurrentCount(mqId), 2);
  const char* msg3 = "this is a 3rd test message";
  EXPECT_EQ(osal::MessageQueue::TrySend(mqId, msg3, strlen(msg3), true), true);
  const char* msg4 = "this is a 4th test message";
  EXPECT_EQ(osal::MessageQueue::TimedSend(mqId, msg4, strlen(msg4), 1, true), true);
  char buffer[128];
  EXPECT_EQ(osal::MessageQueue::Receive(mqId, buffer, 128), (int)strlen(msg));
  buffer[strlen(msg)] = '\0';
  EXPECT_EQ(std::string(buffer), std::string(msg));
  EXPECT_EQ(osal::MessageQueue::TryReceive(mqId, buffer, 128), (int)strlen(msg2));
  buffer[strlen(msg2)] = '\0';
  EXPECT_EQ(std::string(buffer), std::string(msg2));
  EXPECT_EQ(osal::MessageQueue::TimedReceive(mqId, buffer, 128, 0), (int)strlen(msg3));
  buffer[strlen(msg3)] = '\0';
  EXPECT_EQ(std::string(buffer), std::string(msg3));
  EXPECT_EQ(osal::MessageQueue::CurrentCount(mqId), 1);
  osal::MessageQueue::Delete(mqId);
}

TEST(OsalTest, MutexTest)
{
  osal::Mutex::Id* midSimple = osal::Mutex::Create();
  osal::Mutex::Id* midUnsafe = osal::Mutex::CreateRecursive(osal::Mutex::PRIORITY_SAFE);
  osal::Mutex::Id* midSafe = osal::Mutex::CreateRecursive(osal::Mutex::PRIORITY_UNSAFE);
  EXPECT_NE(midSimple, midUnsafe);
  EXPECT_NE(midSafe, midUnsafe);
  EXPECT_NE(midSafe, midSimple);
  
  // make sure that we have not effect on calling the mutex lock here - since we are running
  // in single thread mode it should have no effect
  EXPECT_CALL(midSafe->mData, Lock()).Times(6);
  EXPECT_CALL(midSimple->mData, Lock()).Times(6);
  EXPECT_CALL(midUnsafe->mData, Lock()).Times(6);
  
  for (unsigned int i = 0; i < 6; i++)
  {
    unsigned int counter = 0;
    osal::Mutex::Lock(midSimple);
    counter++;
    osal::Mutex::Lock(midSafe);
    counter++;
    osal::Mutex::Lock(midUnsafe);
    counter++;
    EXPECT_EQ(counter, 3u);
  }
  // do the same with the release command
  for (unsigned int i = 0; i < 6; i++)
  {
      unsigned int counter = 0;
      osal::Mutex::Release(midSimple);
      counter++;
      osal::Mutex::Release(midSafe);
      counter++;
      osal::Mutex::Release(midUnsafe);
      counter++;
      EXPECT_EQ(counter, 3u);
  }
  for (unsigned int i = 0; i < 6; i++)
  {
      unsigned int counter = 0;
      EXPECT_EQ(osal::Mutex::TryLock(midSimple), true);
      counter++;
      EXPECT_EQ(osal::Mutex::TryLock(midSafe), true);
      counter++;
      EXPECT_EQ(osal::Mutex::TryLock(midUnsafe), true);
      counter++;
      EXPECT_EQ(counter, 3u);
  }
  for (unsigned int i = 0; i < 6; i++)
  {
      unsigned int counter = 0;
      EXPECT_EQ(osal::Mutex::TimedLock(midSimple, i), true);
      counter++;
      EXPECT_EQ(osal::Mutex::TimedLock(midSafe, i), true);
      counter++;
      EXPECT_EQ(osal::Mutex::TimedLock(midUnsafe, i), true);
      counter++;
      EXPECT_EQ(counter, 3u);
  }
  
  EXPECT_CALL(midSafe->mData, Delete());
  EXPECT_CALL(midSimple->mData, Delete());
  EXPECT_CALL(midUnsafe->mData, Delete());
  osal::Mutex::Delete(midSafe);
  osal::Mutex::Delete(midUnsafe);
  osal::Mutex::Delete(midSimple);
}
  
TEST(OsalTest, RWMutexTest)
{
  osal::RWMutex::Id* mid= osal::RWMutex::Create();
  // make sure that we have not effect on calling the mutex lock here - since we are running
  // in single thread mode it should have no effect
  for (unsigned int i = 0; i < 6; i++)
  {
    unsigned int counter = 0;
    osal::RWMutex::ReadLock(mid);
    counter++;
    osal::RWMutex::WriteLock(mid);
    counter++;
    osal::RWMutex::ReadLock(mid);
    counter++;
    EXPECT_EQ(counter, 3u);
  }
  // do the same with the release command
  for (unsigned int i = 0; i < 6; i++)
  {
      unsigned int counter = 0;
      osal::RWMutex::Release(mid);
      counter++;
      EXPECT_EQ(counter, 1u);
  }
  for (unsigned int i = 0; i < 6; i++)
  {
      unsigned int counter = 0;
      EXPECT_EQ(osal::RWMutex::TryReadLock(mid), true);
      counter++;
      EXPECT_EQ(osal::RWMutex::TryWriteLock(mid), true);
      counter++;
      EXPECT_EQ(osal::RWMutex::TryReadLock(mid), true);
      counter++;
      EXPECT_EQ(counter, 3u);
  }
  for (unsigned int i = 0; i < 6; i++)
  {
      unsigned int counter = 0;
      EXPECT_EQ(osal::RWMutex::TimedReadLock(mid, i), true);
      counter++;
      EXPECT_EQ(osal::RWMutex::TimedWriteLock(mid, i), true);
      counter++;
      EXPECT_EQ(osal::RWMutex::TimedReadLock(mid, i), true);
      counter++;
      EXPECT_EQ(counter, 3u);
  }
  
  osal::RWMutex::Delete(mid);
}
  
TEST(OsalTest, Events)
{
  using ::testing::Mock;
  
  osal::EventNotification::Id* event = osal::EventNotification::Create();
  // we want to make sure that we don't get hang up because of the use here
  EXPECT_CALL(event->mData, Wait()).Times(4);
  EXPECT_CALL(event->mData, TryWait()).Times(4);
  EXPECT_CALL(event->mData, TimeWait()).Times(4);
  for (unsigned int i = 0; i < 4; i++)
  {
    unsigned int counter = 0;
    osal::EventNotification::Wait(event);
    counter++;
    EXPECT_EQ(osal::EventNotification::TryWait(event), true);
    counter++;
    EXPECT_EQ(osal::EventNotification::TimedWait(event, i), true);
    counter++;
    EXPECT_EQ(counter, 3u);
  }
  
  EXPECT_CALL(event->mData, Signal());
  osal::EventNotification::Signal(event);
  EXPECT_CALL(event->mData, Delete());
  osal::EventNotification::Delete(event);
}

TEST(OsalTest, CountingSem)
{
  using ::testing::Mock;
  osal::CountingSemaphore::Id* sem1 = osal::CountingSemaphore::Create(2);
  osal::CountingSemaphore::Id* sem2 = osal::CountingSemaphore::Create(3);
  
  EXPECT_NE(sem1, sem2);
  
  // run a loop and ensure that we don't block
  EXPECT_CALL(sem1->mData, Wait()).Times(6);
  EXPECT_CALL(sem2->mData, Wait()).Times(6);
  for (unsigned int i = 0; i < 6; i++)
  {
    unsigned int count = 0;
    osal::CountingSemaphore::Wait(sem1);
    count++;
    osal::CountingSemaphore::Wait(sem2);
    count++;
    EXPECT_EQ(2u, count);
  }

  EXPECT_CALL(sem1->mData, TryWait()).Times(6);
  EXPECT_CALL(sem2->mData, TryWait()).Times(6);
  for (unsigned int i = 0; i < 6; i++)
  {
    unsigned int count = 0;
    EXPECT_EQ(osal::CountingSemaphore::TryWait(sem1), true);
    count++;
    EXPECT_EQ(osal::CountingSemaphore::TryWait(sem2), true);
    count++;
    EXPECT_EQ(2u, count);
  }
  
  EXPECT_CALL(sem1->mData, TimeWait()).Times(6);
  EXPECT_CALL(sem2->mData, TimeWait()).Times(6);
  for (unsigned int i = 0; i < 6; i++)
  {
    unsigned int count = 0;
    EXPECT_EQ(osal::CountingSemaphore::TimedWait(sem1, i), true);
    count++;
    EXPECT_EQ(osal::CountingSemaphore::TimedWait(sem2, i), true);
    count++;
    EXPECT_EQ(2u, count);
  }
  
  EXPECT_CALL(sem1->mData, Post());
  EXPECT_CALL(sem2->mData, Post());
  osal::CountingSemaphore::Post(sem1);
  osal::CountingSemaphore::Post(sem2);
  EXPECT_CALL(sem1->mData, Delete());
  EXPECT_CALL(sem2->mData, Delete());
  osal::CountingSemaphore::Delete(sem1);
  osal::CountingSemaphore::Delete(sem2);
}

} // end of local namespace
