#pragma once
/**
@file MessageQueue.h
This the interface for a message queue
The message queue allow two or more threads to pass information (events)
between them in a thread safe way and without the need to implement 
busy wait. normally we have one thread that write to the queue
and another that reads from it. it is an error to read and write
from within the same thread!!
The message queue in C has a given initial size and restrict the 
max size for the each message

To use this do as follow
Before you start any thread that would use this message queue create the queue

MessageQueue::Id* qid = MessageQueue::Create(MAX_QUEUE_SIZE, MAX_MQ_MSG_SIZE);

then create at least two threads one for reading and another for writing

void consumer(MessageQueue::Id* mqId)
{
	char message[MAX_MQ_MSG_SIZE];
	while (keepRunning)
	{
		int ret = MessageQueue::Pop(mqId, message, MAX_MQ_MSG_SIZE);
		if (ret > 0)
		{
			// do something with the message
		}
	}
}

void producer(MessageQueue::Id* mqId)
{
	char message[MAX_MQ_MSG_SIZE];
	while (keepRunning)
	{
		// create a message in the buffer
		unsigned int msgLen = CreateMessage(message, MAX_MQ_MSG_SIZE);
		MessageQueue::Push(mqId, message, msgLen, 0);
	}
}

once those threads finished you must delete the queue

MessageQueue::Delete(qid);



NOTE about interrupts safty - the implementation would be interrupt safe to
the same level as VxWorks native calls (see http://www-kryo.desy.de/documents/vxWorks/V5.4/vxworks/ref/msgQLib.html)
On other operation systems it cannot assume that it can handle iterrupts safly
As for signals safty, the implementation should try to be as much 
signal safe as possible but without trying to make any actions that may
affect the application itself (that is, if the application assume it would
recieve signals the implentation would not mask signals from the applications)
*/

#include "osal/OsalGeneralDefines.h"


namespace osal
{

namespace MessageQueue
{

struct Id;

/**
@brief this function would register a callback function to be called when critical error has happened
@param func the function to be called at exit
*/
void RegisterAtExit(at_error_fun func);

/**
@brief create a new message queue
This function would create a new message queue to be used later to send messages between threads
This function must be called before any other function is trying to use the queue
@param queueSize the max number of messages that can be placed in the queue before sending is blocked
@param maxMessageSize the max size for a given message in the queue
@return MessageQueueId pointer. Note that this function would never return NULL, if it would fail internally
it would assert on the failure
*/
Id* Create(unsigned int queueSize, unsigned int maxMessageSize);

/**
@brief this function would place a new message in the queue
The function would block the caller if the queue is full
@param mqId the id to which the message is pushed
@param msgBuff the data to be placed in the message queue
@param msgSize the size of the data to be placed in the queue (less than maxMessageSize!)
@param highPriority tell whether to place this at the head of the queue
*/
void Send(Id* mqId, CONST_MESSAGE char* msgBuff, unsigned int msgSize, bool highPriority);

/**
@brief this function would place a new message in the queue. None blocking
The function would not block the caller if the queue is full
@param mqId the id to which the message is pushed
@param msgBuff the data to be placed in the message queue
@param msgSize the size of the data to be placed in the queue (less than maxMessageSize!)
@param highPriority tell whether to place this at the head of the queue
@return true value if successfully add the message
*/
bool TrySend(Id* mqId, CONST_MESSAGE char* msgBuff, unsigned int msgSize, bool highPriority);

/**
@brief this function would place a new message in the queue. blocking with timeout
The function would block the caller if the queue is full up until the timeout value
@param mqId the id to which the message is pushed
@param msgBuff the data to be placed in the message queue
@param msgSize the size of the data to be placed in the queue (less than maxMessageSize!)
@param milliDuration the max time to wait if the queue is full
@param highPriority tell whether to place this at the head of the queue
@return true value if successfully add the message
*/
bool TimedSend(Id* mqId, CONST_MESSAGE char* msgBuff, unsigned int msgSize, 
               milliseconds_t milliDuration, bool highPriority);
	
/**
@brief extract a message from the queue
This function would read a message from the queue. it would block if queue is empty
@param mqId the id from which the message is read
@param msgBuff the data is read to this message
@param buffSize the size of the buffer to which message is read. If this is less than the message, not all message is read
@return -1 if failed to read, else the message size that was read
*/	
int Receive(Id* mqId, char* msgBuff, unsigned int buffSize);

/**
@brief extract a message from the queue
This function would read a message from the queue. it would not block if queue is empty
@param mqId the id from which the message is read
@param msgBuff the data is read to this message
@param buffSize the size of the buffer to which message is read. If this is less than the message, not all message is read
@return -1 if failed to read, 0 if timedout, else the message size that was read
*/	
int TryReceive(Id* mqId, char* msgBuff, unsigned int buffSize);

/**
@brief extract a message from the queue
This function would read a message from the queue. it would block if queue is empty up until timeout value
@param mqId the id from which the message is read
@param msgBuff the data is read to this message
@param buffSize the size of the buffer to which message is read. If this is less than the message, not all message is read
@param milliDuration max time to be blocked if message queue is empty
@return -1 if failed to read, 0 if timedout, else the message size that was read
*/	
int TimedReceive(Id* mqId, char* msgBuff, unsigned int buffSize, milliseconds_t milliDuration);

/**
@brief return the number of pending messages in the queue (note that this may not be correct!)
@param mqId message queue from which to read number of waiting messages
@return -1 if error, otherwise the number of messages still in the queue
*/
int CurrentCount(Id* mqId);

/**
@brief delete the message queue and release any resources allocated by it
This function must not be called if the message queue is still in use. Call this function
once the use of the message  queue is finished
@param mqId the message queue to be deleted
*/
void Delete(Id*& mqId);


} // end of namespace MessageQueue

} // end of namespace osal


