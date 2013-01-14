
#include "asynccallbacks/Executer.h"
#include "gtest/gtest.h"
#include "osal/Thread.h"
#include <string>

using namespace asynccallbacks;

namespace
{ // all test code is local to this file
  
  const double EXPECTED_D = 44.3;
  std::string EXPECTED_S = "this is the expected string";
  const int EXPECTED_A = 11;
  const int EXPECTED_B = 41;
  // this would demo the use of class base execution
  class TestClass
  {
  public:
    
    
    TestClass(unsigned int maxMsg) : mExecuter(this, 4), mTimes1(0), mTimes2(0), mTimes3(0), mTimes4(0)
    {
    }
    
    void Start()
    {
      mExecuter.Start("testThread");
    }
    
    void Stop()
    {
      mExecuter.Stop();
    }
    
    bool F1()
    {
      return mExecuter.Call(&TestClass::f1);
    }
    
    bool F2()
    {
      return mExecuter.Call(&TestClass::f2, EXPECTED_A, EXPECTED_B, EXPECTED_D);
    }
    
    bool F3()
    {
      return mExecuter.Call(&TestClass::f3, EXPECTED_S);
    }
    
    bool F4()
    {
      return mExecuter.Call(&TestClass::f4, EXPECTED_D, EXPECTED_B, &EXPECTED_S, EXPECTED_A);
    }
    
  private:
    
    void f1()
    {
      mTimes1++; 
    }
    
    void f2(int a, int b, double d)
    {
      EXPECT_EQ(a, EXPECTED_A);
      EXPECT_EQ(b, EXPECTED_B );
      EXPECT_EQ(d, EXPECTED_D);
      mTimes2++; 
    }
    
    int f3(const std::string& s) const
    {
      EXPECT_EQ(s, EXPECTED_S);
      mTimes3++; 
      return s.size();
      
    }
    
    int f4(double d, int b, std::string* s, int a)
    {
      EXPECT_EQ(a, EXPECTED_A);
      EXPECT_EQ(b, EXPECTED_B );
      EXPECT_EQ(d, EXPECTED_D);
      EXPECT_EQ(*s, EXPECTED_S);
      mTimes4++; 
      return a;
    }
    
    Executer<TestClass> mExecuter;  // this would run the member functions here
  public:
    mutable unsigned int mTimes1, mTimes2, mTimes3, mTimes4; // counters for test
  };
  
bool threadEnded[10] = {false};

template<int I>
void TestWithClass()
{
  TestClass tc(10);
  // make sure that nothing was used yet
  EXPECT_EQ(tc.mTimes1, 0u);
  EXPECT_EQ(tc.mTimes2, 0u);
  EXPECT_EQ(tc.mTimes3, 0u);
  EXPECT_EQ(tc.mTimes4, 0u);
  // verify that we cannot call any function as start was not called
  EXPECT_NE(tc.F1(), true);
  EXPECT_NE(tc.F2(), true);
  EXPECT_NE(tc.F3(), true);
  EXPECT_NE(tc.F4(), true);
  EXPECT_EQ(tc.mTimes1, 0u);
  EXPECT_EQ(tc.mTimes2, 0u);
  EXPECT_EQ(tc.mTimes3, 0u);
  EXPECT_EQ(tc.mTimes4, 0u);
  tc.Start();
  // register funcation calls more than once
  EXPECT_EQ(tc.F1(), true);
  EXPECT_EQ(tc.F2(), true);
  EXPECT_EQ(tc.F3(), true);
  EXPECT_EQ(tc.F4(), true);
  EXPECT_EQ(tc.F2(), true);
  EXPECT_EQ(tc.F1(), true);
  EXPECT_EQ(tc.F4(), true);
  EXPECT_EQ(tc.F4(), true);
  EXPECT_EQ(tc.F3(), true);
  // verify that we successfully use these functions
  osal::Thread::Self::Sleep(10);
  EXPECT_NE(tc.mTimes1, 0u);
  EXPECT_NE(tc.mTimes2, 0u);
  EXPECT_NE(tc.mTimes3, 0u);
  EXPECT_NE(tc.mTimes4, 0u);
  tc.Stop();    // tell the thread to stop
  //osal::Thread::Self::Sleep(10);
  // make sure that we cannot register any more!
  EXPECT_NE(tc.F1(), true);
  EXPECT_NE(tc.F2(), true);
  EXPECT_NE(tc.F3(), true);
  EXPECT_NE(tc.F4(), true);
  // now start again the thread and make sure that we can register!
  tc.Start();
  EXPECT_EQ(tc.F4(), true);
  EXPECT_EQ(tc.F2(), true);
  EXPECT_EQ(tc.F1(), true);
  EXPECT_EQ(tc.F4(), true);
  tc.Stop();    // tell the thread to stop
  //osal::Thread::Self::Sleep(10);
  
  threadEnded[I] = true;
}


class ExecuterUT : public ::testing::Test
{
public:
  ExecuterUT()
  {
    
  }
  
protected:
  virtual void SetUp()
  {
    for (int i = 0; i < 10; i++)
    {
      threadEnded[i] = false;
    }
  }
  
};

TEST_F(ExecuterUT, TestWithClass)
{
  TestWithClass<0>();
}

TEST_F(ExecuterUT, TestWith2Class)
{
  // this test would be the same as above, only in this case we would start two instances of the same class
  // to make sure that there is no problem using this with more than one caller
  // we would start 2 threads and in each thread we would pass a function
  // that would do the test, this is done so that we would have concurrent access to the 
  // infrastructure to ensure that using it with the same type is thread safe
  osal::Thread::Id* tid1 = osal::Thread::Create(osal::Thread::CreateAttribute("firstThread", 1024*1024, osal::Thread::Self::Priority()), TestWithClass<1>);
  osal::Thread::Id* tid2 = osal::Thread::Create(osal::Thread::CreateAttribute("secondThread", 1024*1024, osal::Thread::Self::Priority()), TestWithClass<2>);
  osal::Thread::Self::Sleep(100); // let the thread with the test run
  for (unsigned int i = 0; i < 10; i++)
  {
    if (threadEnded[0] && threadEnded[1])
    {
      break;
    }
    else
    {
      osal::Thread::Self::Sleep(10);  // let other thread a chance to run
    }
  }
  osal::Thread::Clean(tid1);
  osal::Thread::Clean(tid2);
}

}
