#ifndef MESSAGE_QUEUE_IMPLE_MOCK_HPP
#define MESSAGE_QUEUE_IMPLE_MOCK_HPP

#include "osal/mocks/MessageQueueMock.h"  // header file for this class

namespace osal { namespace mocks {

MessageQueueMock::MessageQueueMock()
{
  using ::testing::Return;
  using ::testing::_;
  
  ON_CALL(*this, TimeSend(_, _, _)).WillByDefault(Return(true));
  ON_CALL(*this, TrySend(_, _, _)).WillByDefault(Return(true));
  ON_CALL(*this, TryReceive(_, _)).WillByDefault(Return(0));
  ON_CALL(*this, TimeReceive(_, _)).WillByDefault(Return(0));
  ON_CALL(*this, Receive(_, _)).WillByDefault(Return(0));
    
}

} // end of namespace mocks
} // end of namespace osal
#else
# error "you must not include this file inside another header file"
#endif //MESSAGE_QUEUE_IMPLE_HPPP
