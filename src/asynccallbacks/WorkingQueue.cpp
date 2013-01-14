#include "asynccallbacks/details/WorkingQueue.h"
#include "asynccallbacks/details/WorkingQueueEntry.h"  // the data that would be placed in this queue
#include "osal/MessageQueue.h"  // we would use a message queue from osal to sync between threads

namespace asynccallbacks
{
  
namespace
{    
   const unsigned int SIZEOF_OBJECT_TYPE = sizeof(WorkingQueueEntry);
}

WorkingQueue::WorkingQueue(size_t s) : mId(0), mSize(s)
{
  mId = osal::MessageQueue::Create(s, SIZEOF_OBJECT_TYPE);
}

WorkingQueue::~WorkingQueue()
{
  osal::MessageQueue::Delete(mId);  // delete the queue
}

bool WorkingQueue::Push(WorkingQueueEntry* val)
{
  osal::MessageQueue::Send(mId, (char*)val, SIZEOF_OBJECT_TYPE, false);
  return true;
}
  
bool WorkingQueue::Pop(WorkingQueueEntry& val)
{
  return osal::MessageQueue::Receive(mId, (char*)&val, SIZEOF_OBJECT_TYPE) == (int)SIZEOF_OBJECT_TYPE;
}

bool WorkingQueue::Pop(WorkingQueueEntry& val, osal::milliseconds_t maxTimeout)
{
  return osal::MessageQueue::TimedReceive(mId, (char*)&val, SIZEOF_OBJECT_TYPE, maxTimeout);
}
  
std::size_t WorkingQueue::MaxSize() const
{
  return mSize*SIZEOF_OBJECT_TYPE;
}
} // end of namespace asynccallbacks

