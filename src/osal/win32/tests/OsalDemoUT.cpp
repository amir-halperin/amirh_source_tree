#include "osal/Mutex.h"
#include "osal/MessageQueue.h"
#include "osal/Thread.h"
#include "osal/CountingSemaphore.h"
#include "osal/EventNotification.h"
#include "../mocks/include/EventNotificationFwd.h" // definition for the id
#include "../mocks/include/ThreadFwd.h" // definition for the id
#include "../mocks/include/MutexFwd.h" // definition for the id
#include "../mocks/include/CountingSemaphoreFwd.h" // definition for the id
#include "../mocks/include/RWMutexFwd.h" // definition for the id
#include "osal/RWMutex.h"
#include <gtest/gtest.h>
#include <memory>
#include <string>

namespace
{

class TestClass;

void StartThread()
{ 
}

// this class is a production code class
// as such it does not aware of the fact that
// it is under test - there is nothing in this class that 
// uses the testing facilities - the only exption here
// is that we add a friend class that can access its internals
// what this class does is nothing that this class aware of 
// but this class can be use to gain access to internal part and test them
class DemoClass
{
public:
  friend class TestClass;
  
  DemoClass()
  {
    mSem = osal::CountingSemaphore::Create(1);
    mThread = osal::Thread::Create(osal::Thread::CreateAttribute("threadName", 256, osal::Thread::PRIORITY_12), StartThread);
    mMutex = osal::Mutex::Create();
    mRWMutex = osal::RWMutex::Create();
    mEvent = osal::EventNotification::Create();
    mQueue = osal::MessageQueue::Create(1, 2);
  }
  
  ~DemoClass()
  {
    osal::CountingSemaphore::Delete(mSem);
    osal::Thread::Clean(mThread);
    osal::MessageQueue::Delete(mQueue);
    osal::Mutex::Delete(mMutex);
    osal::RWMutex::Delete(mRWMutex);
    osal::EventNotification::Delete(mEvent);
  }
  
  bool Add2Queue(const char* msg, unsigned int size)
  {
    if (osal::MessageQueue::TrySend(mQueue, msg, size, true)) 
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  
  bool SetValue(int val)
  {
    if (osal::Mutex::TryLock(mMutex))
    {
      mValue = val;
      return true;
      osal::Mutex::Release(mMutex);
    }
    else
    {
      return false;
    }
  }
  
  int GetValue()
  {
    if (osal::RWMutex::TimedReadLock(mRWMutex, 10))
    {
      
      int val = mValue;
      osal::RWMutex::Release(mRWMutex);
      return val;
    }
    else
    {
      return -1;
    }
  }
  
  bool GetMessage(char* buff, unsigned int buffSize)
  {
    if (osal::MessageQueue::TimedReceive(mQueue, buff, buffSize, 23) > 0)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  
  int Read()
  {
    if (osal::EventNotification::TryWait(mEvent))
    {
      return mValue;
    }
    else
    {
      return -1;
    }
  }
  
  int SetNewValue(int val)
  {
    if (osal::CountingSemaphore::TimedWait(mSem, 20))
    {
      int ret = mValue;
      mValue = val;
      return ret;
    }
    else
    {
      return -1;
    }
  }
  
protected:
  osal::CountingSemaphore::Id* GetSemId()
  {
    return mSem;
  }
  osal::Thread::Id* GetTID()
  {
    return mThread;
  }
  osal::Mutex::Id* GetMID()
  {
    return mMutex;
  }
  osal::RWMutex::Id* GetRWMutex()
  {
    return mRWMutex;
  }
  osal::EventNotification::Id* GetEvent()
  {
    return mEvent;
  }
  osal::MessageQueue::Id* GetMQ()
  {
    return mQueue;
  }
private:
  osal::CountingSemaphore::Id* mSem;
  osal::Thread::Id* mThread;
  osal::Mutex::Id* mMutex;
  osal::RWMutex::Id* mRWMutex;
  osal::EventNotification::Id* mEvent;
  osal::MessageQueue::Id* mQueue;
  int mValue;
};
//////////////////////////////////////// end of production code /////////////////////////////
//
//  start of test code - note that we expecting that this code would not 
// be placed in the same file as the production class code is located
// but this class does have access to the internals of the class that it is testing
// note that for that reason this class gain access to the internals of the above class
// and is using it as member to us it from the testing POV
class TestClass
{
public:
  TestClass()
  {
    
  }
  
  ~TestClass()
  {
    EXPECT_CALL(mMember->GetSemId()->mData, Delete());
    EXPECT_CALL(mMember->GetTID()->mData, Clean());
    EXPECT_CALL(mMember->GetEvent()->mData, Delete());
    EXPECT_CALL(mMember->GetRWMutex()->mData, Delete());
    EXPECT_CALL(mMember->GetMID()->mData, Delete());
    mMember.reset(0); // so that detur is called
  }
  
  void Init()
  {
    using ::testing::_;
    // we are expecting for the create function to be called when ctor for DemoClass is called
    /*EXPECT_CALL(mMember->GetSemId()->mData, Create(_));
    EXPECT_CALL(mMember->GetTID()->mData, Create(_, _));
    EXPECT_CALL(mMember->GetEvent()->mData, Create());
    EXPECT_CALL(mMember->GetRWMutex()->mData, Create());
    EXPECT_CALL(mMember->GetMID()->mData, Create());*/
    mMember.reset(new DemoClass);
  }
  
  void TestQueue(const char* msg, unsigned int size)
  {
    std::string data(msg);
    char buffer[128];
    EXPECT_EQ(mMember->Add2Queue(msg, size), true);
    EXPECT_EQ(mMember->GetMessage(buffer, 128), true);
    EXPECT_EQ(data, std::string(buffer, size));
  }
  
  void SetValue(int val)
  {
    using ::testing::Mock;
    using ::testing::Return;
    bool expected = val > 0;
      
    EXPECT_CALL(mMember->GetMID()->mData, TryLock()).WillOnce(Return(expected));
    EXPECT_EQ(mMember->SetValue(val), expected);
      
  }
  // this function can set the call to fail or not with the flag pass to it
  void GetValue(bool willNotFail)
  {
    using ::testing::Mock;
    using ::testing::Return;
    int val = mMember->mValue;
    
    EXPECT_CALL(mMember->GetRWMutex()->mData, TimeReadLock()).WillOnce(Return(willNotFail));
    EXPECT_EQ(mMember->GetValue() == val, willNotFail);
  }
  
  void Read(bool success)
  {
    using ::testing::Mock;
    using ::testing::Return;
    int val = mMember->mValue;
    
    EXPECT_CALL(mMember->GetEvent()->mData, TryWait()).WillOnce(Return(success));
    EXPECT_EQ(mMember->Read() == val, success);
    
  }
  
  void SetNewValue(int val)
  {
    
    using ::testing::Mock;
    using ::testing::Return;
    int oldVal = mMember->mValue;
    bool success = val > 0;
    
    EXPECT_CALL(mMember->GetSemId()->mData, TimeWait()).WillOnce(Return(success));
    EXPECT_EQ(mMember->SetNewValue(val) == oldVal, success);
  }
  
private:
  std::auto_ptr<DemoClass> mMember;
};

/////////////////////////////////////////// START OF TEST CASES ///////////////////////////////

TEST(OsalDemoUT, FirstCase)
{
  {TestClass tc;
  tc.Init();
  tc.GetValue(true);  // make sure that it will not fail
  tc.Read(true);  // make sure that it will not fail
  tc.SetNewValue(10);  // make sure that it will not fail
  const char* msg = "This is a test message";
  unsigned int len = strlen(msg);
  tc.TestQueue(msg, len);
  tc.SetValue(16);  // make sure that it would succeed
  }
}

TEST(OsalDemoUT, SecondCase)
{
  {TestClass tc;
  tc.Init();
  tc.GetValue(false);  // make sure that it will fail
  tc.Read(false);  // make sure that it will fail
  tc.SetNewValue(-1);  // make sure that it will not fail
  const char* msg = "This is a test message";
  unsigned int len = strlen(msg);
  tc.TestQueue(msg, len);
  tc.SetValue(-1);  // make sure that it would fail
  }
}

} // end of local namespace
