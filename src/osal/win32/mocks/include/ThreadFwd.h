#pragma once
// this file can be used for unit testing where we need access to some internal data
// so you can include this one and then use the internal ID structure for more elaborate unit testing
#include "osal/Thread.h"           // namespace RWMutex
#include "osal/mocks/ThreadMock.h" // the internal data type for the id
#include <gmock/gmock.h>            // for the mocking internal definition             

namespace osal
{
namespace Thread
{
 
struct Id
{
  ::testing::NiceMock<osal::mocks::ThreadMock> mData;
  
  Id(const Attributes& a, entry_func_t f);
  
  ~Id();
}; 

    
} // end of namespace Thread
  
} // end of namespace osal

