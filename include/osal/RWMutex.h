#pragma once

#include "osal/OsalGeneralDefines.h" // shared data types and functions between all OSAL modules

/**
 * @file RWMutex.h
 * @brief mutex with multi readers access or single writes
 * A readers-writer lock is like a mutex, in that it controls access to a shared resource, 
 * allowing concurrent access to multiple threads for reading but restricting access 
 * to a single thread for writes (or other changes) to the resource. 
 * A common use might be to control access to a data structure in memory that can't be 
 * updated atomically but isn't valid (and shouldn't be read by another thread) until the 
 * update is complete.
 * 
 * Note that this is a mutex - that is the same task/thread that took it (either for reading or for writing)
 * must also release it! Note also that you cannot lock for reading and then while still holding the
 * lock lock it again for writing (and the other way around). So if a thread need different level
 * of access from the one it currently holds, it must first release the mutex then lock it 
 * again with the new level of locking
 * 
 * tipical use case for this library is - 
 * 
 * - first in a thread that run before any lock is needed we are doing:
 * osal::RWMutex::Id* rwMutex = osal::RWMutex::Create();
 * 
 * - have some functions that would be called from thread and shared resource
 * 
 * int sharedResource = 10;
 * 
 * int GetResourceValue()
 * {
 *    osal::RWMutex::ReadLock(rwMutex);
 *    int val = sharedResource;
 *    osal::RWMutex::Release(rwMutex);
 * }
 * 
 * void SetResourceValue(int newValue)
 * {
 *    osal::RWMutex::WriteLock(rwMutex);
 *    sharedResource = newValue;
 *    osal::RWMutex::Release(rwMutex);
 * }
 * 
 * void ReadOnlyFunc1()
 * {
 *  for (int i = 0; i < 10000; i++) 
 *  {
 *    int val = GetResourceValue();
 *    DoSomethingWithValue(val);
 *  }
 * }
 * 
 * void ReadOnlyFunc2()
 * {
 *  for (int i = 0; i < 10000; i++) 
 *  {
 *    if (GetResourceValue() > someValue)
 *    {
 *      DoSomethingElse();
 *    }
 *  }
 * }
 * 
 * void WriteOnlyFunc()
 * {
 *  for (int i = 0; i < 100000; i++)
 *  {
 *    SetResourceValue(rand()); // set the value from this thread
 *  }
 * }
 * 
 * - create some threads now -
 * osal::Thread::Create(CreateAttribute(..), ReadOnlyFunc1); 
 * osal::Thread::Create(CreateAttribute(..), ReadOnlyFunc2); 
 * osal::Thread::Create(CreateAttribute(..), WriteOnlyFunc); 
 * 
 * - note that the functions ReadOnlyFunc1 and ReadOnlyFunc2 would not block each other, but if
 *   WriteOnlyFunc function would try to modify the value it would be blocked if either  ReadOnlyFunc
 *   are holding the lock and vice versa
 * - important notice - on VxWorks this is recursive safe for write locks. On other platfroms it may
 *   not be safe to call write lock without first releasing the lock (that is it may not be recursive safe)
 *   For sure on Windows platforms this would deadlock the calling thread!
 */

namespace osal
{
  
namespace RWMutex
{
  
struct Id;  // forward declare only so that the internals are unknown

/**
@brief this function would register a callback function to be called when critical error has happened
@param func the function to be called at exit
*/
void RegisterAtExit(at_error_fun func);

/**
 * @brief this function would allocate a read/write mutex object
 * 
 * Note that this function must be called before the read/write mutex can be
 * used. Note also that  from the application POV this function would alway
 * return a valid value (if some expected error was raised this function would
 * handle it internally)
 * @return a valid read/write mutex handle
 */
Id* Create();

/**
 * @brief lock the mutex for reading
 * This function would lock the mutex nonexclusive in the sense that
 * other threads can also access the same lock as long as it is for reading only
 * If the lock is taken by for writing the thread would be blocked until the lock
 * is released without any timeout value (unless the mutex is deleted)
 * @param id the mutex id
 */
void ReadLock(Id* id);

/**
 * @brief lock the mutex for reading
 * This function would lock the mutex nonexclusive in the sense that
 * other threads can also access the same lock as long as it is for reading only
 * If the lock is taken by for writing the thread would not be blocked 
 * the the function would return false
 * @param id the mutex id
 * @return false if the mutex was not given from reading, true if the mutex was taken
 */
bool TryReadLock(Id* id);

/**
 * @brief lock the mutex for reading
 * This function would lock the mutex nonexclusive in the sense that
 * other threads can also access the same lock as long as it is for reading only
 * If the lock is taken by for writing the thread would be blocked 
 * for no more than the timeout value in milliseconds
 * @param id the mutex id
 * @param milliTimeout the timeout to wait for the lock to become available in milliseconds
 * @return false if the mutex was not given from reading, true if the mutex was taken
 */
bool TimedReadLock(Id* id, milliseconds_t milliTimeout);

/**
 * @brief lock the mutex for writing
 * This function would lock the mutex exclusively in the sense that
 * no other threads can access the same lock
 * If the lock is taken for writing or reading by other thread this thread 
 * would be blocked until the lock
 * is released without any timeout value (unless the mutex is deleted)
 * @param id the mutex id
 */
void WriteLock(Id* id);

/**
 * @brief lock the mutex for writing
 * This function would lock the mutex exclusively in the sense that
 * no other threads can access the same lock
 * If the lock is taken for writing or reading by other thread this thread 
 * would not be blocked because the lock is taken by other thread, but the function would return false
 * @param id the mutex id
 * @return false if the thread did not lock the mutex, true if it did
 */
bool TryWriteLock(Id* id);

/**
 * @brief lock the mutex for writing
 * This function would lock the mutex exclusively in the sense that
 * no other threads can access the same lock
 * If the lock is taken for writing or reading by other thread this thread 
 * would be blocked until for no more than the timeout value (if the lock is taken by other thread)
 * @param id the mutex id
 * @param milliTimeout the max timeout to wait before return false
 * @return false if the thread did not lock the mutex, true if it did
 */
bool TimedWriteLock(Id* id, milliseconds_t milliTimeout);

/**
 * @brief any thread that locked this mutex must call this function to release it to other threads
 * This function must be called when ever the mutex was successfully locked (by any of the
 * functions above) to ensure that other threads can lock the mutex and to prevent from
 * deadlocking the system. This function MUST be called from the same thread that
 * locked the mutex
 * @param id the id of the mutex to release
 */
void Release(Id* id);

/**
 * @brief deallocate any resources that were allocated when the mutex was created
 * This function would make mutex un-usable any more. After calling this function
 * you can no longer use this mutex. Any resources that were allocated in
 * the create function would be released
 */
void Delete(Id*& id);
  
} // end of namespace RWMutex
  
} // end of namespace osal
