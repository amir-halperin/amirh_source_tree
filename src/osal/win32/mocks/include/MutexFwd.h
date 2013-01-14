#pragma once
// this file can be used for unit testing where we need access to some internal data
// so you can include this one and then use the internal ID structure for more elaborate unit testing
#include "osal/Mutex.h"           // namespace mutex
#include "osal/mocks/MutexMock.h" // the internal data type for the id
#include <gmock/gmock.h>          // for the mocking internal definition             

namespace osal
{
namespace Mutex
{
 
struct Id
{
  ::testing::NiceMock<osal::mocks::MutexMock> mData;
  
  Id();
  
  Id(int p);
  
  ~Id();
}; 

    
} // end of namespace mutex
  
} // end of namespace osal

