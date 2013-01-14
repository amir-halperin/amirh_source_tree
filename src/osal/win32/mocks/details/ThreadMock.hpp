#ifndef THREAD_IMPLE_MOCK__HPP
#define THREAD_IMPLE_MOCK__HPP
#include "osal/mocks/ThreadMock.h"  // header file for this class 

namespace osal { namespace mocks {
  
ThreadMock::ThreadMock()
{
  using ::testing::Return;
  using ::testing::_;
  
  ON_CALL(*this, EqualsId(_, _)).WillByDefault(Return(true));
  ON_CALL(*this, Priority()).WillByDefault(Return(osal::Thread::LOWSET_PRIORITY));
  ON_CALL(*this, Name()).WillByDefault(Return("TaskMock"));
}
  
} // end of namespace mocks
} // end of namespace osal
#else
# error "you must not include this file inside another header file"
#endif //THREAD_IMPLE_MOCK__HPP
