#pragma once
/**
@file Mutax.h
@brief this would have two types of mutal exclusion types to guard resources

The mutual exclusion details are in here
http://en.wikipedia.org/wiki/Mutex
In general we would use this to sync the access to shared resource from within the thread (but in intrathread manner)
what this mean is that if we have some shared resource that we need exclusive access to, we would put a mutex on it
then when a thread need to read or change it, it would take the mutex, do what ever it need to do with it and release the
mutex. The difference between this and the semaphore use case is that in semaphore we wait on the semaphore
in one thread and give it in another thread. In this case we are both waiting and given the mutex from the same 
thread. Also the order does mater since you must first take it, then release it, doing the reverse would
block you forever!

The use of mutex is something along those lines - 

Before you start any thread that would use the mutex call the create function - 
Mutex::Id* mid = MutexCreateSimple();

Then start the threads that would use it
in the code that need to be thread safe do - 

void SameThreaSafeFucntion(Mutex::Id* lock)
{
	// in the code path that need to be thread safe - 
	MutexLock(lock);
	// .. do something that need to co-ordinate with other threads here..
	// then make sure that you release it!
	MutexRelease(lock);
}

once the threads finished delete the mutex!!
MutexDelete(mid);



NOTE about interrupts safty - the implementation would be interrupt safe to
the same level as VxWorks native calls (see http://www-kryo.desy.de/documents/vxWorks/V5.4/vxworks/ref/semLib.html)
On other operation systems it cannot assume that it can handle iterrupts safly
As for signals safty, the implementation should try to be as much 
signal safe as possible but without trying to make any actions that may
affect the application itself (that is, if the application assume it would
recieve signals the implentation would not mask signals from the applications)

*/
#include "osal/OsalGeneralDefines.h"	/* general types and definition */


/* note that under C++ this code is under namespace osal!! */
namespace osal
{

namespace Mutex
{

struct Id;

enum Protection
{
	PRIORITY_SAFE,
	PRIORITY_UNSAFE
};

/**
@brief this function would register a callback function to be called when critical error has happened
@param func the function to be called at exit
*/
void RegisterAtExit(at_error_fun func);

/**
@brief create a mutex with little overhead and no logical error testing
This function would create a mutex that do not have support for any type of 
error testing - this include no support for recursive locking correction and
no priority invention protection. This should be the mutex of choice in most cases
because in most cases we would not like to hide these errors (which are design errors)
of locking the same mutex more than once from within the same thread or having priority
issues which are not portable!
@return MutexId pointer which is allocated on the head. Note that this function
cannot fail. If it fail we would assert for it inside the function, so the
use does no need to test this!
*/
Id* Create();

/**
@brief this would create a mutex with recursive call protection and possibly priority inversion protection
This function would create a mutex that would support against deadlocks if the same thread
would try to lock the same mutex more than once. It can also support priority inversion protection
which mean that if one thread with low priority took the mutex and a thread with higher priority
is trying to get the same lock, and a third thread with middle priority is no letting the
low priority thread with the mutex to run, than the high priority thread cannot run.
with priority inversion safe mutex, the low priority thread would get the priority 
of the highest waiting thread so that it would not block the thread that waiting on the
mutex because it cannot run.
While this is all nice, code that relay on this protection is less portable and the code
to execute is slower than Create(), so in most cases don't use this option
@return MutexId pointer which is allocated on the head. Note that this function
cannot fail. If it fail we would assert for it inside the function, so the
use does no need to test this!
*/
Id* CreateRecursive(Protection prioritySafe);

/**
@brief lock a mutex, if the mutex is locked by another thread, block until it become available
This call once completed would grant exclusive access to a piece of code that 
is guarded by the mutex. Note that for each of these there must be a call to
MutexRelease function! failing to do so would lock any other threads that would try
to access the same code path.
@param id is the mutex to lock
Please note that this function would return any value, any failure inside would be asserted!
*/
void Lock(Id* id);

/**
@brief lock a mutex, if the mutex is locked by another thread, the 
calling thread would be blocket until it become available
This call once completed would grant exclusive access to a piece of code that 
is guarded by the mutex. Note that for each of these there must be a call to
MutexRelease function! failing to do so would lock any other threads that would try
Please note that unlike MutexLock, this function would not block the caller, hence the 
return value for this function.
to access the same code path.
@param id is the mutex to lock
@return true value if the function succeeded, false value if failed to lock the mutex
*/
bool TryLock(Id* id);

/**
@brief lock a mutex, if the mutex is locked by another thread, 
the calling thread would be block until the lock is available
This call once completed would grant exclusive access to the code that 
is guarded by the mutex. Note that for each of these there must be a call to
Release function! failing to do so would lock any other threads at would try
Please note that unlike Lock, this function would not block the caller more than
timeout value that provides to the function as parameter, hence the 
return value for this function.
to access the same code path.
@param id is the mutex to lock
@param milliDuration max time to wait for the lock before return false, in milliseconds (no absolute time!)
@return true value if the function succeeded, false value if failed to lock the mutex
*/
bool TimedLock(Id* id, milliseconds_t milliDuration);

/**
@brief this function must be call after each successful lock function, it would release the mutex for other threads
This function must be call once the thread finish with the mutex. Failing to do so would lead to locking
out other thread. Note that this function must be call from the same thread in which the lock is taken!!!
@param id is the mutex to release
*/
void Release(Id* id);

/**
@brief release the mutex resource (this must called only when any thread using the mutex is not using it any more)
This function would release the mutex resources. Once this is called the mutex cannot be used any more
@param id is the mutex to delete
*/
void Delete(Id*& id);


}   // end of namespace Mutex

}   // end of namespace osal



