#pragma once
// this file can be used for unit testing where we need access to some internal data
// so you can include this one and then use the internal ID structure for more elaborate unit testing
#include "osal/RWMutex.h"           // namespace RWMutex
#include "osal/mocks/RWMutexMock.h" // the internal data type for the id
#include <gmock/gmock.h>            // for the mocking internal definition             

namespace osal
{
namespace RWMutex
{
 
struct Id
{
  ::testing::NiceMock<osal::mocks::RWMutexMock> mData;
  
  Id();
  
  ~Id();
}; 

    
} // end of namespace RWMutex
  
} // end of namespace osal
