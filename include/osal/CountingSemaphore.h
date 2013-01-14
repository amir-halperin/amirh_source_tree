#pragma once

/**
@file CountingSempahore.h
@brief this is an interface to the counting semaphore ITC

For more details about what is count semaphore you can read
http://en.wikipedia.org/wiki/Semaphore_(programming)
in short we have a guard that sync between two or more threads/processes
the use case for it is that one thread would post on the semaphore and thus
increase its count, while the other will wait on it, and if this count
is greater than zero would have access to the resource. The number of available
resources is set when the semaphore is created. The count cannot go bellow
0 without blocking the thread that waits on the semaphore. A very common use
case for the use of this semaphore is that of consumer/producer

before starting the consumer/producer threads you should call 
struct CountingSemaphore::Id* semId = CountingSemaphore::Create(SOME_INITIAL_COUNT);

then in the consumer thread you should have something like this - 
void consumer(struct CountingSemaphore::Id* semId, SharedResource* resource)
{
	while (dontStop)
	{
		CountingSemaphore::Wait(semId);
		ReadResource(resource);
	}
}

in the producer thread you should do 
void producer(struct CountingSemaphore::Id* semId, SharedResource* resource)
{
	while (dontStop)
	{
		CreateResource(resource);
		CountingSemaphore::Post(semId);
	}
}

once you finish with these threads you MUST call
CountingSemaphore::Delete(semId);


NOTE about interrupts safty - the implementation would be interrupt safe to
the same level as VxWorks native calls (see http://www-kryo.desy.de/documents/vxWorks/V5.4/vxworks/ref/semLib.html)
On other operation systems it cannot assume that it can handle iterrupts safly
As for signals safty, the implementation should try to be as much 
signal safe as possible but without trying to make any actions that may
affect the application itself (that is, if the application assume it would
recieve signals the implentation would not mask signals from the applications)
*/

#include "osal/OsalGeneralDefines.h"	/* contain general definitions for this library */


/* note that under C++ this code is under namespace osal!! */
namespace osal
{

namespace CountingSemaphore
{

/** The type CountSemId is opaque to the application programmer */
struct Id;

/**
@brief this function would register a callback function to be called when critical error has happened
@param func the function to be called at exit
*/
void RegisterAtExit(at_error_fun func);

/**
@brief create a counting semaphore with initial value 
This function would create a counting semaphore that has some initial value
since the type of the counting semaphore is opaque at the application level
This is the only way to create it. Please note that this imply that the resource
is created on the heap. in any case this is the only way to do it since this resource
is shared among threads/tasks
@param count the initial count for the semaphore
@return pointer to the semaphore that is created
Please note that this function cannot fail!! under testing this would assert for failure
so you don't need to check that it returned successfully!. Under C++ this would also throw
an exception if failed
*/
Id* Create(int count);

/**
@brief this function would wait on a semaphore.
This function would wait on the semaphore until its count
is no longer 0. note that if the count is not zero to begin with, it 
would return without waiting at all
@param on this is the semaphore on which to wait
*/
void Wait(Id* on);

/**
@brief this function would wait on a semaphore with timeout value.
This function would wait on the semaphore until its count
is no longer 0. note that if the count is not zero to begin with, it 
would return without waiting at all. Please note that this function
would not wait more than the timeout value. So if the semaphore
is not ready it would return with an error
@param on this is the semaphore on which to wait
@param milliDuration the max time to wait in milliseconds. If this timeout passed and semaphore is not ready return -1
@return true on successful operation
*/
bool TimedWait(Id* on, milliseconds_t milliDuration);

/**
@brief this function would wait on a semaphore - return without blocking if not available
This function would wait on the semaphore until its count
is no longer 0. note that if the count is not zero to begin with, it 
would return without waiting at all. Please note that this function
would not block the caller if the semaphore is not avilable
@param on this is the semaphore on which to wait
@return true on successfull operation
*/
bool TryWait(Id* on);

/**
@brief this function would increment semaphore count
This function would increment semaphore count by one. If there is 
another thread blocked on this semaphore, the thread (the one that at
the head of the waiting threads if there is more than one) would be released to
continue it work
@param on this is the semaphore on which to post
@return true on successful operation
*/
void Post(Id* on);

/**
@brief this function would release the resource held by this semaphore
Note this function should not be called if you have threads that are still
using this semaphore. once this function finished, the semaphore is no 
longer available for use
@param what this is the semaphore to delete
*/
void Delete(Id*& what);

}   // end of namespace CountingSemaphore

}   // end of namespace osal


