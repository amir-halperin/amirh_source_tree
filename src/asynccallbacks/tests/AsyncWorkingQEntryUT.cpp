#include "WorkingQUTHelper.h"
#include "gtest/gtest.h"
#include "asynccallbacks/details/WorkingQueueEntry.h"
#include "osal/MessageQueue.h"
#include "asynccallbacks/details/WorkingQueue.h"
#include "osal/Thread.h"
#include "osal/Mutex.h"
#include "osal/EventNotification.h"
#include <string>
#include <boost/ptr_container/ptr_vector.hpp>




namespace
{ // all the code is private to this file

  using namespace wqut;
  using namespace asynccallbacks;
  
  
  /////////////////////////////////////////////////////////////////////////////
  
    
  typedef WorkingQueue mq_type;
//osal::MessageQueue::Id* messageQueue = 0;
  mq_type& GetMessageQueue()
  { 
    static mq_type messageQueue(10);
    return messageQueue;
  }

unsigned int EXPECTED_MESSAGES = 4;
const unsigned int SIZE_OF_MESSAGE = 1024;
osal::EventNotification::Id* dispatcherStart = 0;
osal::EventNotification::Id* dispatcherEnd = 0;
  
void OperateFunction(const WorkingQueueEntry& newRequest)
{
  newRequest();
}

bool SendMessage(WorkingQueueEntry* newMsg)
{
  //static const unsigned int actaul_size =sizeof(WorkingQueueEntry*); 
  return GetMessageQueue().Push(newMsg);
}

// test the registration for later calls between 2 threads
void Dispatcher()
{
  //assert(messageQueue); // make sure that we are calling this with valid value!
  //unsigned int bufferSize = SIZE_OF_MESSAGE;
  osal::EventNotification::Signal(dispatcherStart);
  for (unsigned int i = 0; i < EXPECTED_MESSAGES;)
  {
    WorkingQueueEntry entry;
    if (GetMessageQueue().Pop(entry))
    {
      OperateFunction(entry);
      i++;
    }
    else
    {
      osal::Thread::Self::Sleep(100);
    }
    // block for the next request
    /*WorkingQueueEntry* entry = 0;
    int size = osal::MessageQueue::Receive(messageQueue, (char*)&entry, sizeof(WorkingQueueEntry*));
    EXPECT_GT(size, 0);
    if (entry)
    {
      OperateFunction(*entry);
    }
    else
    {
      EXPECT_NE((void*)0, (void*)entry);
    }*/
  }
  osal::EventNotification::Signal(dispatcherEnd);
}

class TestAsyncWorkingQ : public ::testing::Test
{
public:
  TestAsyncWorkingQ()
  {
    dispatcherStart = osal::EventNotification::Create();
    dispatcherEnd = osal::EventNotification::Create();
  }
  
  ~TestAsyncWorkingQ()
  {
    osal::EventNotification::Delete(dispatcherStart);
    osal::EventNotification::Delete(dispatcherEnd);
  }
  
protected:
  virtual void SetUp()
  {
    //messageQueue = osal::MessageQueue::Create(10, SIZE_OF_MESSAGE);
    //assert(messageQueue); // make sure that we were able to create the message queue
    TestFunc1Counter = 0;
    TestFunc2Counter = 0;
    Foo::ctor_calls = 0;
    Foo::dtor_calls = 0;
    Foo::one_args_call = 0;
    Foo::two_args_call = 0;
    Foo::three_args_call = 0;
    Foo::four_args_call = 0;
    Bar::ctor_calls = 0;
    Bar::dtor_calls = 0;
    Bar::one_args_call = 0;
    Bar::two_args_call = 0;
    Bar::three_args_call = 0;
    Bar::four_args_call = 0;
  }
  
  virtual void TearDown()
  {
    //osal::MessageQueue::Delete(messageQueue);
  }
protected:
  Foo mFoo;
};

TEST_F(TestAsyncWorkingQ, SimpleAsyncTest)
{
  // in this test we would have another thread that would read from a queue to which
  // the messages are placed at
  EXPECT_EQ(0, Foo::ctor_calls);
  EXPECT_EQ(0, Foo::dtor_calls );
  EXPECT_EQ(0, Foo::one_args_call);
  EXPECT_EQ(0, Foo::two_args_call);
  EXPECT_EQ(0, Foo::three_args_call);
  EXPECT_EQ(0, Foo::four_args_call);
  
  osal::Thread::PriorityType prio = osal::Thread::Self::Priority(); // same priority as this one
  
  WorkingQueueEntry* pointers[4];
  osal::Thread::Id* workerT = osal::Thread::Create(osal::Thread::CreateAttribute("SimpleAsyncTest", 1024*1024, prio), Dispatcher);
  osal::Thread::Self::Sleep(100);
  osal::EventNotification::Wait(dispatcherStart);
  std::auto_ptr<WorkingQueueEntry> entry = MakeWorkingQueueEntry(&Foo::OneArg, &mFoo, INT_ARGUMENT);
  pointers[0] = entry.release();
  SendMessage(pointers[0]);
  entry = MakeWorkingQueueEntry(&Foo::ThreeArg, &mFoo, DOUBLE_ARGUMENT, STRING_ARG, INT_ARGUMENT);
  pointers[1] = entry.release();
  SendMessage(pointers[1]);
  entry = MakeWorkingQueueEntry(&Foo::TwoArg, &mFoo, DOUBLE_ARGUMENT, INT_ARGUMENT);
  pointers[2] = entry.release();
  SendMessage(pointers[2]);
  entry = MakeWorkingQueueEntry(&Foo::FourArg, &mFoo, INT_ARGUMENT, &STRING_ARG, DOUBLE_ARGUMENT, INT2_ARGUMENT);
  pointers[3] = entry.release();
  SendMessage(pointers[3]);
  osal::EventNotification::Wait(dispatcherEnd);
  EXPECT_EQ(0, Foo::ctor_calls);
  EXPECT_EQ(0, Foo::dtor_calls );
  EXPECT_EQ(1, Foo::one_args_call);
  EXPECT_EQ(1, Foo::two_args_call);
  EXPECT_EQ(1, Foo::three_args_call);
  EXPECT_EQ(1, Foo::four_args_call);
  osal::Thread::Clean(workerT);
}

}
