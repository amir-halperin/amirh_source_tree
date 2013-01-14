#pragma once
/**
 * @brief This file contain mock class definition to be used inside unit testing that uses read/write mutex
 * 
 * You can use this class to trigger return values using WillOnce() member function from the mock API
 * then the API for the OSAL will return whatever was registered using these objects 
 */
#include <gmock/gmock.h> // USES MOCK_METHOD1
#include "osal/Thread.h"  // some types defined here that we need them

namespace osal
{

namespace mocks
{
  
  // please note that this cannot be used for any functions in the count semaphore
  // API only functions that actually doing something are here
  // the mock is not used directly but though the API of the OSAL
class ThreadMock
{
public:
  ThreadMock();
  
  MOCK_METHOD2(Create, void(const osal::Thread::Attributes& att, osal::Thread::entry_func_t f));  // this can be used to verify that call method was called only!
  MOCK_METHOD2(EqualsId, bool(void* l, void* r));      
  MOCK_METHOD0(Priority, osal::Thread::PriorityType());
  MOCK_METHOD1(NewPriority, void(osal::Thread::PriorityType p));
  MOCK_METHOD0(Name, const char*());       // this can be used to verify that call method was called only!
  MOCK_METHOD0(Clean, void());     // this can be used to verify that call method was called only!
};

}  // end of namespace mocks

} // end of namespace osal


