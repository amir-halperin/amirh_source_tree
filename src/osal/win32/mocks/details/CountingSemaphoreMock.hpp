#ifndef COUNTING_SEMAPHOREMOCK_IMPLE_HPP 
#define COUNTING_SEMAPHOREMOCK_IMPLE_HPP 

#include "osal/mocks/CountingSemaphoreMock.h" // header file for this cpp file

namespace osal { namespace mocks {

CountingSemaphoreMock::CountingSemaphoreMock()
{
  using ::testing::Return;
  ON_CALL(*this, TimeWait()).WillByDefault(Return(true));
  ON_CALL(*this, TryWait()).WillByDefault(Return(true));
}

} // end of namespace mocks
} // end of namesapce osal

#else
# error "you must not include this file inside another header file"
#endif //COUNTING_SEMAPHOREMOCK_IMPLE_HPP 
