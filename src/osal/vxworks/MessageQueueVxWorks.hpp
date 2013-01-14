#ifndef MESSAGE_QUEUE_VXWORKS__HPP
#define MESSAGE_QUEUE_VXWORKS__HPP

#include "ExitFunctionHolder.h"     // to handle functions called on exit
#include "SemHandle.h" // vxworks semaphore handle
#include "CriticalSection.h"        // critical section pattern
#include "OsalTimeUtils.h"          // shared resources and functions in osal
#include <msgQLib.h>                // vxworks message queue interface
#include <assert.h>                 // assert
#include <errnoLib.h>               // errno
#include <stdio.h>                  // perror

namespace osal
{
  
namespace MessageQueue
{

namespace
{
  
  Private::ExitFunctionHolder& ExitFunctionsList();
  
  Private::ExitFunctionHolder& ExitFunctionsList()
  {
    static Private::ExitFunctionHolder theList;
    return theList;
  }
  
} // end of local namespace

///////////////////////////////////////////////////////////////////////////////
  
struct Id
{
  Id() : mValue(NULL)
  {
  } 
  
  Id(MSG_Q_ID id) : mValue(id)
  {
    
  }
  ~Id()
  {
    msgQDelete(mValue);
  }
  
  MSG_Q_ID  mValue; 
};

void RegisterAtExit(at_error_fun func)
{
}

Id* Create(unsigned int queueSize, unsigned int maxMessageSize)
{
  MSG_Q_ID qid = msgQCreate(queueSize, maxMessageSize, MSG_Q_PRIORITY);
  if (!qid)
  {
	  ExitFunctionsList().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
  }
  return new Id(qid);
  
}

void Send(Id* mqId, CONST_MESSAGE char* msgBuff, unsigned int msgSize, bool highPriority)
{
  assert(mqId);   // this function cannot be called with invalid id
  if (msgQSend(mqId->mValue, msgBuff, msgSize, WAIT_FOREVER,  highPriority ? MSG_PRI_URGENT : MSG_PRI_NORMAL) != OK)
  {
	  ExitFunctionsList().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
  }
}

bool TrySend(Id* mqId, CONST_MESSAGE char* msgBuff, unsigned int msgSize, bool highPriority)
{
  assert(mqId);   // this function cannot be called with invalid id
  if (msgQSend(mqId->mValue, msgBuff, msgSize, NO_WAIT,  highPriority ? MSG_PRI_URGENT : MSG_PRI_NORMAL) != OK)
  {
	  return ExitFunctionsList().TryFail(errnoGet(), __FUNCTION__, __LINE__);
  }
  else
  {
    return true;
  }
  
}

bool TimedSend(Id* mqId, CONST_MESSAGE char* msgBuff, unsigned int msgSize, 
               milliseconds_t milliDuration, bool highPriority)
{
  assert(mqId);   // this function cannot be called with invalid id
  if (msgQSend(mqId->mValue, msgBuff, msgSize, TimeUtils::Milli2Ticks(milliDuration),  highPriority ? MSG_PRI_URGENT : MSG_PRI_NORMAL) != OK)
  {
	  return ExitFunctionsList().TimedOut(errnoGet(), __FUNCTION__, __LINE__);
  }
  else
  {
    return true;
  }  
}

int Receive(Id* mqId, char* msgBuff, unsigned int buffSize)
{
  assert(mqId);   // this function cannot be called with invalid id
  int ret = msgQReceive(mqId->mValue, msgBuff, buffSize, WAIT_FOREVER);
  if (ret == ERROR)
  {
	  ExitFunctionsList().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
  }
  return ret;
  
}

int TryReceive(Id* mqId, char* msgBuff, unsigned int buffSize)
{
  assert(mqId);   // this function cannot be called with invalid id
  int ret = msgQReceive(mqId->mValue, msgBuff, buffSize, NO_WAIT);
  if (ret == ERROR)
  {
	  ExitFunctionsList().TryFail(errnoGet(), __FUNCTION__, __LINE__);
  }
  return ret;
  
}

int TimedReceive(Id* mqId, char* msgBuff, unsigned int buffSize, milliseconds_t milliDuration)
{
  assert(mqId);   // this function cannot be called with invalid id
 
  int ret = msgQReceive(mqId->mValue, msgBuff, buffSize, TimeUtils::Milli2Ticks(milliDuration));
  if (ret == ERROR)
  {
	  ExitFunctionsList().TimedOut(errnoGet(), __FUNCTION__, __LINE__);
  }
  return ret;
  
}

int CurrentCount(Id* mqId)
{
  assert(mqId);   // this function cannot be called with invalid id
  return msgQNumMsgs(mqId->mValue);
}

void Delete(Id*& mqId)
{
  if (mqId)
  {
    delete mqId;
    mqId = 0;
  }
}
  
}  // end of namespace MessageQueue
  
} // end of namespace osal

#else
# error "you are trying to include a file that must no include in header file"
#endif  // MESSAGE_QUEUE_VXWORKS__HPP
