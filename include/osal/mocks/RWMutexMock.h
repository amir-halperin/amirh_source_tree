#pragma once
/**
 * @brief This file contain mock class definition to be used inside unit testing that uses read/write mutex
 * 
 * You can use this class to trigger return values using WillOnce() member function from the mock API
 * then the API for the OSAL will return whatever was registered using these objects 
 */
#include <gmock/gmock.h> // USES MOCK_METHOD1

namespace osal
{

namespace mocks
{
  
  // please note that this cannot be used for any functions in the count semaphore
  // API only functions that actually doing something are here
  // the mock is not used directly but though the API of the OSAL
class RWMutexMock
{
public:
  RWMutexMock();
  
  MOCK_METHOD0(Create, void());  // this can be used to verify that call method was called only!
  MOCK_METHOD0(ReadLock, void());       // this can be used to verify that call method was called only!
  MOCK_METHOD0(TimeReadLock, bool());
  MOCK_METHOD0(TryReadLock, bool());
  MOCK_METHOD0(WriteLock, void());       // this can be used to verify that call method was called only!
  MOCK_METHOD0(TimeWriteLock, bool());
  MOCK_METHOD0(TryWriteLock, bool());
  MOCK_METHOD0(Release, void());    // allow to test whether this was called
  MOCK_METHOD0(Delete, void());     // this can be used to verify that call method was called only!
};

}  // end of namespace mocks

} // end of namespace osal


