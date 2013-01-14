#pragma once
/**
 * @file asynccallbacks/details/WorkingQueue.h
 * 
 * @brief contain the class WorkingQueue that is used to register work
 * 
 * This class would hold a queue that would have items for to be executed
 * later in some other thread. This is based on the osal message queue
 * but unlike that osal message queue it would only allow to register
 * and extract data in blocking manner
 */

#include <osal/OsalGeneralDefines.h>  // milliseconds type
#include <cstddef>  // size_t
#include <boost/noncopyable.hpp>  // make it none copyable

namespace osal { namespace MessageQueue { struct Id; }}

namespace asynccallbacks
{

class WorkingQueueEntry;

class WorkingQueue : boost::noncopyable
{
public:
  
  /**
   * the ctor would create the internal queue that would save the messages
   * @param s the size of the queue
   */
  WorkingQueue(std::size_t s);
  
  /**
   * close the queue
   */
  ~WorkingQueue();
  
  /**
   * add new element to the queue. if queue is full wait forever
   * @param val a new entry into the queue - we assum that this was allocated on the heap
   * @return return true if the message was pushed into the queue
   */
  bool Push(WorkingQueueEntry* val);
  
  /**
   * read entry from the queue, wait for ever if nothing in the queue
   * @param val the value to read from the queue
   * @return true if successfully read value from the queue
   */
  bool Pop(WorkingQueueEntry& val);
  
  /**
   * reard a new entry from the 
   * @param val the vale from the queue would be set here
   * @param maxTimeout the timeout to wait for the entry
   * @return true if read entry within the given timeout, false if timeout elapsed
   */
  bool Pop(WorkingQueueEntry& val, osal::milliseconds_t maxTimeout);
  
  /**
   * @return the number of iteams from the queue
   */
  std::size_t MaxSize() const;
  
private:
  osal::MessageQueue::Id* mId;
  std::size_t mSize;
};

} // end of namespace asynccallbacks
