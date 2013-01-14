#ifndef MESSAGE_QUEUE_IMPLE_HPP
#define MESSAGE_QUEUE_IMPLE_HP
// This file would implement message queue based on boost (which would soon be part of 
// C++ standard implementation) for message queue
// since this is portable by itself we can use this for both win32 and posix plaftfroms

#include "osal/OsalGeneralDefines.h"  // milliseconds_t
#include <boost/message_queue/message_queue.hpp>  // the actual impl is here

namespace osal
{
  
namespace MessageQueue
{
  
namespace details
{
  
class MessageQueue
{
public:
  typedef boost::opaque_message_queue value_type;
  
  MessageQueue(value_type::size_type cap, value_type::size_type ms) : mData(cap, ms)
  {    
  }
  
  void Push(const char* item, value_type::size_type len)
  {
    mData.push_front(item, len);
  }
  
  bool TryPush(const char* item, value_type::size_type len)
  {
    return mData.try_push(item, len);
  }
  
  bool TimePush(const char* item, value_type::size_type len, milliseconds_t timeout)
  {
    return mData.try_push(item, len, boost::posix_time::milliseconds(timeout));
  }
  
  int  Pop(char* buff, value_type::size_type maxLen)
  {
    mData.pop_back(buff, maxLen);
    return (int)maxLen;
  }
  
  int TryPop(char* buff, value_type::size_type maxLen)
  {
    if (mData.try_pop(buff, maxLen))
    {
       return (int)maxLen;
    }
    else
    {
      return -1;
    }
  }
  
  int TimePop(char* buff, value_type::size_type maxLen, milliseconds_t timeout)
  {
    if (mData.try_pop(buff, maxLen, boost::posix_time::milliseconds(timeout)))
    {
       return (int)maxLen;
    }
    else
    {
      return -1;
    }
    
  }
  
  int Size() const
  {
    return mData.size();
  }
  
private:
  value_type mData;
}; 
  
} // end of namespace details
  
} // end of namespace MessageQueue
} // end of namespace osal

#else
# error "you must not include this file inside header file"
#endif  //MESSAGE_QUEUE_IMPLE_HP 
