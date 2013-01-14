#pragma once
/**
 * @brief This file contain mock class definition to be used inside unit testing that uses message queue
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
class MessageQueueMock
{
public:
  MessageQueueMock();
  
  MOCK_METHOD2(Create, void(unsigned int siz, unsigned int msiz));  // this can be used to verify that call method was called only!
  MOCK_METHOD3(Send, void(const char* msg, unsigned int siz, bool pri));    // this can be used to verify that call method was called only!
  MOCK_METHOD3(TimeSend, bool(const char* msg, unsigned int siz, bool pri));
  MOCK_METHOD3(TrySend, bool(const char* msg, unsigned int siz, bool pri));
  MOCK_METHOD2(Receive, int(char* msg, unsigned int siz));   
  MOCK_METHOD2(TimeReceive, int(char* msg, unsigned int siz));
  MOCK_METHOD2(TryReceive, int(char* msg, unsigned int siz));
  MOCK_METHOD0(Delete, void());       // this can be used to verify that call method was called only!
};

}  // end of namespace mocks

} // end of namespace osal

