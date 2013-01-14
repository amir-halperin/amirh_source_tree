#ifndef RW_MUTEX_IMPLE_MOCK_HPP
#define RW_MUTEX_IMPLE_MOCK_HPP

#include "osal/mocks/RWMutexMock.h" // header file for this class

namespace osal { namespace mocks {

RWMutexMock::RWMutexMock()
{
  
  using ::testing::Return;
  using ::testing::_;
  
  ON_CALL(*this, TimeReadLock()).WillByDefault(Return(true));
  ON_CALL(*this, TryReadLock()).WillByDefault(Return(true));
  ON_CALL(*this, TimeWriteLock()).WillByDefault(Return(true));
  ON_CALL(*this, TryWriteLock()).WillByDefault(Return(true));
}
  
} // end of namespace mocks
} // end of namespace osal
#else
# error "you must not include this file inside another header file"
#endif //RW_MUTEX_IMPLE_MOCK_HPP
