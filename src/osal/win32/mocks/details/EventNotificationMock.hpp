#ifndef EVENT_NOTIFICATION_IMPLE_MOCK_HPP
#define EVENT_NOTIFICATION_IMPLE_MOCK_HPP

#include "osal/mocks/EventNotificationMock.h" // header file for this class

namespace osal { namespace mocks {

EventNotificationMock::EventNotificationMock()
{
  using ::testing::Return;
  ON_CALL(*this, TimeWait()).WillByDefault(Return(true));
  ON_CALL(*this, TryWait()).WillByDefault(Return(true));
}
  
} // end of namespace mocks
} // end of namespace osal

#else
# error "you must not include this file inside another header file"
#endif //EVENT_NOTIFICATION_IMPLE_MOCK_HPP
