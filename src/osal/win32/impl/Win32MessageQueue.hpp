#ifndef WIN32_MESSAGE_QUEUE_IMPLE_HPP
#define WIN32_MESSAGE_QUEUE_IMPLE_HPP

#include "../../details/MessageQueue.hpp"  // actaul imple

namespace osal
{
namespace MessageQueue
{
  
struct Id : public details::MessageQueue
{
  Id(unsigned int qSize, unsigned int msgSize) : details::MessageQueue(qSize, msgSize)
  {
    
  }
};
  
Id* Create(unsigned int queueSize, unsigned int maxMessageSize)
{
  return new Id(queueSize, maxMessageSize);
}

void Send(Id* mqId, CONST_MESSAGE char* msgBuff, unsigned int msgSize, bool highPriority)
{
  assert(mqId);
  mqId->Push(msgBuff, msgSize);
}

bool TrySend(Id* mqId, CONST_MESSAGE char* msgBuff, unsigned int msgSize, bool highPriority)
{
  assert(mqId);
  return mqId->TryPush(msgBuff, msgSize);
}

bool TimedSend(Id* mqId, CONST_MESSAGE char* msgBuff, unsigned int msgSize, 
               milliseconds_t milliDuration, bool highPriority)
{
  assert(mqId);
  return mqId->TimePush(msgBuff, msgSize, milliDuration);
}

int Receive(Id* mqId, char* msgBuff, unsigned int buffSize)
{
  assert(mqId);
  return mqId->Pop(msgBuff, buffSize);
}

int TryReceive(Id* mqId, char* msgBuff, unsigned int buffSize)
{
  assert(mqId);
  return mqId->TryPop(msgBuff, buffSize);
}

int TimedReceive(Id* mqId, char* msgBuff, unsigned int buffSize, milliseconds_t milliDuration)
{
  assert(mqId);
  return mqId->TimePop(msgBuff, buffSize, milliDuration); 
}

int CurrentCount(Id* mqId)
{
  assert(mqId);
  return mqId->Size();
}

void Delete(Id*& mqId)
{
  delete mqId;
  mqId = 0;
}

} // end of namespace MessageQueue
  
} // end of namespace osal

#else
# error "you must not include this file inside header file"
#endif  // WIN32_MESSAGE_QUEUE_IMPLE_HPP
