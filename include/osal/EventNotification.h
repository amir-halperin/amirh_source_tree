#pragma once
/**
@file EventNotification.h

This would be the interface for event notification between threads
The use case for this is when one thread need to notify another thread that
it can continue doing something (what this task that it needs to perform is
not part of this interface)
For example - if we have a one thread that need to be notify that some variable
was change, and until than it do not want to poll on its value. And this variable
is change by another thread. Then the first thread would wait on the event notification
and the other thread would release it when it finish changing it
on VxWorks this is done using binary semaphore and configuring the semaphore
with initial value to empty, then the first thread in this case would 
do semTake and would be blocked. The second thread would then do semGive
when it done changing the variable
In POSIX we would use condition variable for the same operation

The use case for this module is as follow - 

first create a shared resource that would be used between tasks/threads - 

EventNotification::Id enid = EventNotification::Create();

then from the task/thread that need to wait for some condition/other issue to happen do - 

EventNotification::Wait(enid);

in the other task once you need to signal that the other thread should copntinue do - 

EventNotification::Signal(enid); 

this would release the other task (the one that did the wait) to continue working.
once the tasks finished using the resource we must do - 
EventNotification::Delete(enid);

For more information about this you can look at http://www.ibm.com/developerworks/linux/library/l-posix3/
Note that on VxWorks this is implemented using semBCreate with initial state set to empty so
that the first task that would try to do wait would be blocked until some
other task would release it

NOTE about interrupts safty - the implementation would be interrupt safe to
the same level as VxWorks native calls (see http://www-kryo.desy.de/documents/vxWorks/V5.4/vxworks/ref/semLib.html)
On other operation systems it cannot assume that it can handle iterrupts safly
As for signals safty, the implementation should try to be as much 
signal safe as possible but without trying to make any actions that may
affect the application itself (that is, if the application assume it would
recieve signals the implentation would not mask signals from the applications)
*/
#include "osal/OsalGeneralDefines.h"


namespace osal
{

namespace EventNotification
{

struct Id;

/**
@brief this function would register a callback function to be called when critical error has happened
@param func the function to be called at exit
*/
void RegisterAtExit(at_error_fun func);

/**
@brief create a new modification event object
This would create a notification event object that can be use
to sync between threads
@return EventNotifyId pointer allocated on the head
please note that this function would never return NULL. If the creation failed
then the function would assert internally!
*/
Id* Create();

/**
@brief wait to be notify that a condition was signalled
This would block the calling thread until a notification is set to it over
 the notification  event object, which mean that the thread can continue its work
 @param on the notification object to wait on
*/
void Wait(Id* on);

/**
@brief wait to be notify that a condition was signalled. This would not block the caller
This would test notification event without blocking the calling thread whether a notification is set to it over
 the notification event object, which mean that the thread can continue its work
 @param on the notification object to wait on
 @return true value if event was signalled
*/
bool TryWait(Id* on);

/**
@brief wait to be notify that a condition was signal. This would not block the caller
This would block on notification event for up to a give timeout value. If this is not signaled
then we would not 
 @param on the notification object to wait on
 @param milliDuration the timeout value to wait before returning from this function with failure
 @return true value if event was signal
*/
bool TimedWait(Id* on, milliseconds_t milliDuration);

/**
@brief This would signal that the event happened
This would make the notification object signalled. If there is a thread waiting
on an object then this other thread would be notified that it can continue with its task
note that if there are more than single thread waiting on the object only one would 
receive the notification, other would still be blocked
@param on the object to send the notification to
*/
void Signal(Id* on);

/**
@brief This would signal that the event happened
This would notify object signalled. If there is a thread waiting
on an object then this other thread would notify that it can continue with its task
note that any waited thread on the notification would be made unblocked and would be
able to continue its work
@param on the object to send the notification to
*/
void SignalAll(Id* on);

/**
@brief delete the event notification object and free any resources
Make sure that this function is called only when all threads that are 
using this object is over using it, as once this is called this object
is no longer valid for use
@param id the object to de-allocate
*/
void Delete(Id*& id);

}   // end of namespace EventNotification

}   // end of namespace osal

