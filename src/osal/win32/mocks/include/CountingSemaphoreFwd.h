#pragma once
// this file can be used for unit testing where we need access to some internal data
// so you can include this one and then use the internal ID structure for more elaborate unit testing
#include "osal/RWMutex.h"           // namespace RWMutex
#include "osal/mocks/CountingSemaphoreMock.h" // the internal data type for the id
#include <gmock/gmock.h>            // for the mocking internal definition             

namespace osal
{
namespace CountingSemaphore
{
 
struct Id
{
  ::testing::NiceMock<osal::mocks::CountingSemaphoreMock> mData;
  
  Id(int count);
  
  ~Id();
}; 

    
} // end of namespace CountingSemaphore
  
} // end of namespace osal

