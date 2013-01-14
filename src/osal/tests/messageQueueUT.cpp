/*
 *  This would test functionality of the message queue module from the OSAL
 *  from more details about this you can look in the header file 
 */
#include "osal/MessageQueue.h"  // the module under test
#include "osal/Thread.h"  // so we can test with more than one thread - assume that thread was tested
#include <osal/StopWatch.h>    // this for class StopWatch
#include <gtest/gtest.h>  // unit test framework
#include <string.h>       // strlen
#include <string>         // class string

namespace // all unit test would hide their internal data
{
  
osal::MessageQueue::Id* testMessageQueue = 0;
const unsigned int MAX_MESSAGE_SIZE = 256;
const char* MESSAGES[] = 
{
    "one", "two", "three", "four", "five"
};
const unsigned int MESSAGE_COUNT = sizeof(MESSAGES)/sizeof(MESSAGES[0]);

unsigned int timeoutValue4Test = 0;


void WriteQueueThread()
{
  // this thread would write messages to the queue
  if (testMessageQueue)
  {
    for (unsigned int i = 0; i < MESSAGE_COUNT; i++)
    {
      osal::MessageQueue::Send(testMessageQueue, (CONST_MESSAGE char*)MESSAGES[i], strlen(MESSAGES[i]), false);
      if (timeoutValue4Test)
      {
        osal::Thread::Self::Sleep(timeoutValue4Test);
      }
    }
  }
}
  
void ReadQueueThread()
{
  // this thread would write messages to the queue
  if (testMessageQueue)
  {
    for (unsigned int i = 0; i < MESSAGE_COUNT; i++)
    {
      char buffer[MAX_MESSAGE_SIZE];
      osal::MessageQueue::Receive(testMessageQueue, buffer, MAX_MESSAGE_SIZE);
      if (timeoutValue4Test)
      {
        osal::Thread::Self::Sleep(timeoutValue4Test);
      }
    }
  }
}
  
TEST(MessageQueueUT, BasicTest)
{
  // this test would just try to make a message queue and to verify that create funcion actually work
  
  osal::MessageQueue::Id* mq = osal::MessageQueue::Create(10, 20);  // if this function would fail a report outside this would be called
  osal::MessageQueue::Id* mq2 = osal::MessageQueue::Create(10, 20); // ensure that the second one do not affect by the first
  EXPECT_NE(mq, mq2);
  EXPECT_EQ(osal::MessageQueue::CurrentCount(mq), osal::MessageQueue::CurrentCount(mq2));
  EXPECT_EQ(osal::MessageQueue::CurrentCount(mq), 0);
  char buff[10];
  // we should not be able to read anything since this queue is empty
  EXPECT_EQ(-1, osal::MessageQueue::TryReceive(mq, buff, 10));
  EXPECT_EQ(-1, osal::MessageQueue::TryReceive(mq2, buff, 10));
  osal::MessageQueue::Delete(mq);
  osal::MessageQueue::Delete(mq2);
}

TEST(MessageQueueUT, BasicSendRecive)
{
  // in this function we would have a thread that would write messages to the queue
  // the other thread would read them and verify that the messages we recieve are correct
  testMessageQueue = osal::MessageQueue::Create(1, MAX_MESSAGE_SIZE);
  osal::Thread::Id* tid = osal::Thread::Create(osal::Thread::Attributes("MQTestT", 1024*1024,
                                                                        osal::Thread::Self::Priority()),
                                                                        WriteQueueThread);
  unsigned int messages = 0;
  char buffer[MAX_MESSAGE_SIZE];
  for (; messages < MESSAGE_COUNT; messages++)
  {    
    int count = osal::MessageQueue::Receive(testMessageQueue, buffer, MAX_MESSAGE_SIZE);
    EXPECT_NE(count, -1);   // expect that this message was read successfully
    EXPECT_EQ(std::string(buffer, count), std::string(MESSAGES[messages]));
  }
  EXPECT_EQ(-1, osal::MessageQueue::TryReceive(testMessageQueue, buffer, 10));
  EXPECT_EQ(osal::MessageQueue::CurrentCount(testMessageQueue), 0);
  osal::Thread::Clean(tid);
  osal::MessageQueue::Delete(testMessageQueue);
  testMessageQueue = 0;
                                                                        
}

TEST(MessageQueueUT, TrySendRecieve)
{
  // in this test we would like to make sure that we can use the queue without being blocked
  // both for send messages and to receive them
  // in order to test that, we would like to set a timeout value for the thread that would do
  // the reading when we would like to test the try send and timeout to the thread that would
  // do the would the send when testing try receive
  testMessageQueue = osal::MessageQueue::Create(1, MAX_MESSAGE_SIZE);
  timeoutValue4Test = 10;
  osal::Thread::Id* tid = osal::Thread::Create(osal::Thread::Attributes("MQTestTryT", 1024*1024,
                                                                        osal::Thread::Self::Priority()),
                                                                        WriteQueueThread);
  unsigned int messages = 0;
  char buffer[MAX_MESSAGE_SIZE];
  unsigned int failedTry = 0;
  while (messages < MESSAGE_COUNT)
  {    
    int count = osal::MessageQueue::TryReceive(testMessageQueue, buffer, MAX_MESSAGE_SIZE);
    if (count != -1)
    {
      EXPECT_EQ(std::string(buffer, count), std::string(MESSAGES[messages]));
      ++messages;
    }
    else
    {
      ++failedTry;
    }
  }
  EXPECT_EQ(-1, osal::MessageQueue::TryReceive(testMessageQueue, buffer, 10));
  EXPECT_EQ(osal::MessageQueue::CurrentCount(testMessageQueue), 0);
  EXPECT_GT(failedTry, 0u);
  osal::Thread::Clean(tid);
  osal::MessageQueue::Delete(testMessageQueue);
  testMessageQueue = 0;
  // now do the same with trySend
  testMessageQueue = osal::MessageQueue::Create(1, MAX_MESSAGE_SIZE);
  timeoutValue4Test = 10;
  tid = osal::Thread::Create(osal::Thread::Attributes("MQTestTryT2", 1024*1024,
                                                       osal::Thread::Self::Priority()),
                               ReadQueueThread);
  // now try to send to the queue, it should fail sometimes
  for (unsigned int i = 0, failedTry = 0; i < MESSAGE_COUNT;)
  {
    if (!osal::MessageQueue::TrySend(testMessageQueue, (CONST_MESSAGE char*)MESSAGES[i], strlen(MESSAGES[i]), false))
    {
      ++failedTry;
    }
    else
    {
      ++i;
    }
  }
  EXPECT_GT(failedTry, 0u);  // we expect that we sometimes failed to send a message
  osal::Thread::Clean(tid);
  osal::MessageQueue::Delete(testMessageQueue);
  testMessageQueue = 0;
  
}

TEST(MessageQueueUT, TimeSendRecieve)
{
  // in this test we would like to make sure that we can use the queue without being blocked
  // both for send messages and to receive them
  // in order to test that, we would like to set a timeout value for the thread that would do
  // the reading when we would like to test the try send and timeout to the thread that would
  // do the would the send when testing try receive
  testMessageQueue = osal::MessageQueue::Create(1, MAX_MESSAGE_SIZE);
  timeoutValue4Test = 30;
  osal::Thread::Id* tid = osal::Thread::Create(osal::Thread::Attributes("MQTestTryT", 1024*1024,
                                                                        osal::Thread::Self::Priority()),
                                                                        WriteQueueThread);
  unsigned int messages = 0;
  char buffer[MAX_MESSAGE_SIZE];
  unsigned int failedTry = 0;
  while (messages < MESSAGE_COUNT)
  {    
    int count = osal::MessageQueue::TimedReceive(testMessageQueue, buffer, MAX_MESSAGE_SIZE, 10);
    if (count != -1)
    {
      EXPECT_EQ(std::string(buffer, count), std::string(MESSAGES[messages]));
      ++messages;
    }
    else
    {
      ++failedTry;
    }
  }
  EXPECT_EQ(-1, osal::MessageQueue::TryReceive(testMessageQueue, buffer, 10));
  EXPECT_EQ(osal::MessageQueue::CurrentCount(testMessageQueue), 0);
  EXPECT_GT(failedTry, 0u);
  osal::Thread::Clean(tid);
  osal::MessageQueue::Delete(testMessageQueue);
  testMessageQueue = 0;
  // now do the same with trySend
  testMessageQueue = osal::MessageQueue::Create(1, MAX_MESSAGE_SIZE);
  timeoutValue4Test = 10;
  tid = osal::Thread::Create(osal::Thread::Attributes("MQTestTryT2", 1024*1024,
                                                       osal::Thread::Self::Priority()),
                             ReadQueueThread);
  // now try to send to the queue, it should fail sometimes
  for (unsigned int i = 0, failedTry = 0; i < MESSAGE_COUNT;)
  {
    if (!osal::MessageQueue::TimedSend(testMessageQueue, (CONST_MESSAGE char*)MESSAGES[i], 
                                       strlen(MESSAGES[i]), false, 10))
    {
      ++failedTry;
    }
    else
    {
      ++i;
    }
  }
  EXPECT_GT(failedTry, 0u);  // we expect that we sometimes failed to send a message
  osal::Thread::Clean(tid);
  osal::MessageQueue::Delete(testMessageQueue);
  testMessageQueue = 0;
}

} // end of local namespace
