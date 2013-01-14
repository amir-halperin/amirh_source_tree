#ifndef MUTEX_MOCK_IMPLE_HPP
#define MUTEX_MOCK_IMPLE_HPP
#include "osal/mocks/MutexMock.h" // header file for this class

namespace osal { namespace mocks {

MutexMock::MutexMock()
{
  using ::testing::Return;
  using ::testing::_;
  
  ON_CALL(*this, TimeLock()).WillByDefault(Return(true));
  ON_CALL(*this, TryLock()).WillByDefault(Return(true));
  
}
  
} // end of namespace mocks
} // end of namespace osal
#else
# error "you must not include this file inside another header file"
#endif //MUTEX_MOCK_IMPLE_HPP
