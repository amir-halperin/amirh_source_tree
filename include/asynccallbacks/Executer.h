#pragma once
/**
 * @file Executer.h
 * 
 * @brief holds the class Executer that would be used to register and execute request from some class
 * 
 * Use to class as a member to some class of which you are wishing to turn into an object that 
 * turn its member function into execution of async work
 */

#include <osal/Thread.h>                                // this class would have internal thread
#include <osal/OsalGeneralDefines.h>                    // milliseconds typedef
#include <asynccallbacks/details/WorkingQueue.h>        // the "jobs" would be placed and read from here
#include <asynccallbacks/details/WorkingQueueEntry.h>           // the items that we would execute
#include <asynccallbacks/details/AsyncCallbackUtils.h>  // CriticalSection and Runner classes
#include <boost/noncopyable.hpp>                        // to make this object none copyable
#include <boost/shared_ptr.hpp>                         // smart pointer from boost

namespace osal { 
  namespace EventNotification { struct Id;  } //  namespace EventNotification
  namespace Mutex { struct Id;  } // namespace Mutex;
}  // /namespace osal



namespace asynccallbacks {
  
namespace details
{
  // we would have an object here that would allow us
  // to run more elaborated logic inside the main loop
  // so that the internal thread can do more than
  // "just" executing "things" from the queue
  struct InternalWork
  {
    InternalWork(WorkingQueue& queue, osal::milliseconds_t maxTime);
    
    virtual ~InternalWork();
    
    void operator () ();
    
    void Timeout(osal::milliseconds_t newTime);
    
  protected:
    WorkingQueue& Queue();
    osal::milliseconds_t Timeout() const;
    
  private:
    
    virtual void HandleRequests(WorkingQueueEntry& item) = 0;
    
    WorkingQueue& mQueue;
    osal::milliseconds_t mMaxTimeHandling;
    WorkingQueueEntry    mItem;
  };
} // end of namespace details
  
/**
 * @class Executer
 * @brief this would be the class to use to execute member function in another thread
 * 
 * This class would allow some client class to register requests in it and
 * execute them in some internal thread
 * normally you would like to make this a member in your client class and register
 * whatever member function you would like that would be executed not in the same thread
 * context that your client object is running
 * note that if this is to be used inside a unit test and the async operation is not 
 * required please define USE_SYNC_CALL_FOR_EXECUTER_OBJECT before including this file
 */
template<typename T>
class Executer : boost::noncopyable
{
  typedef std::auto_ptr<WorkingQueueEntry>  WorkingQueueEntryAutoPtr;
  // This is the default "function" if we only want to run requests
  // from the queue. this is the default, if the user of this class
  // wants to do some internal work in the context of the thread
  // we are creating here, he/she needs to pass some other function like
  // object, such as function pointer or functor (class with operator () in it)
  // to override this defaults
  struct DoNothingFunction
  {
    void operator () () const
    {      
    }
  };
  
public:
  /**
   * the ctor for this object would create mutex and event notification object
   * to protect the thread startup. 
   * @param thisPtr pass here a this pointer to the object who's memmber functions we are registering
   * @param qLen the size of the message queue between the external thread and the thread we creating here
   */
  Executer(T* thisPtr, unsigned int qLen);
  
  /**
   * will close the running thread and delete all OS resources
   * @return none
   */
  ~Executer();
  /**
   * Start the internal thread that would execte the requests that are registered in the queue
   * note that if this function is not called you cannot register any request to make sure
   * that the queue is not fill up and there is noone to read from it
   * note also that it is safe to call this fucntion from multiple threads externally
   * @param name the name of the new thread that would execute the requests
   * @param prio the priority in which to run the internal thread
   */ 
  void Start(const char* name, osal::Thread::PriorityType prio = osal::Thread::Self::Priority()); 
  
  /**
   * This function is the same as the function above and only differ in that the user can
   * pass here its own logic operation when not reading from the queue. In which case
   * the user must! pass a timeout value. The type of F is any callable object (be it a function pointer
   * function object - functor or boost::function)
   * @param name the name of the new task that would be created here
   * @param userFunc the function that user wants to run when not reading from the queue
   * @param prio the new task's priority
   * @param maxTimeut the max timeout to wait on the queue. when this is pass userFunc will be called
   */
  template<typename F>
  void Start(const char* name, F userFunc, 
             osal::milliseconds_t maxTimeut, osal::Thread::PriorityType prio = osal::Thread::Self::Priority());
  
  /**
   * Stop the internal thread from executing more requests - note
   * that after calling this function you cannot register more request until
   * the thread is started again
   */
  void Stop();
  /**
   * register member function to be executed here
   * @param mem_fn a pointer to member function from class object_type that accept no paramters
   * @return false if the member function start was not called yet!
   */
  template<typename MF>
  bool Call(MF mem_fn);
  /**
   * register member function to be executed here
   * @param mem_fn a pointer to member function from class object_type that accept singel parameter
   * @param a the function parameter
   * @return false if the member function start was not called yet!
   */
  template<typename MF, typename A>
  bool Call(MF mem_fn, A a);
  /**
   * register member function to be executed here
   * @param mem_fn a pointer to member function from class object_type that accept two parameters
   * @param a the function parameter
   * @param a2 the function parameter
   * @return false if the member function start was not called yet!
   */
  template<typename MF, typename A, typename A2>
  bool Call(MF mem_fn, A a, A2 a2);
  /**
   * register member function to be executed here
   * @param mem_fn a pointer to member function from class object_type that accept 3 parameters
   * @param a the function parameter
   * @param a2 the function parameter
   * @param a3 the function parameter
   * @return false if the member function start was not called yet!
   */
  template<typename MF, typename A, typename A2, typename A3>
  bool Call(MF mem_fn, A a, A2 a2, A3 a3);
  /**
   * register member function to be executed here
   * @param mem_fn a pointer to member function from class object_type that accept 4 parameters
   * @param a the function parameter
   * @param a2 the function parameter
   * @param a3 the function parameter
   * @param a4 the function parameter
   * @return false if the member function start was not called yet!
   */
  template<typename MF, typename A, typename A2, typename A3, typename A4>
  bool Call(MF mem_fn, A a, A2 a2, A3 a3, A4 a4);  
  /**
   * register member function to be executed here
   * @param mem_fn a pointer to member function from class object_type that accept 5 parameters
   * @param a the function parameter
   * @param a2 the function parameter
   * @param a3 the function parameter
   * @param a4 the function parameter
   * @param a5 the function parameter
   * @return false if the member function start was not called yet!
   */
  template<typename MF, typename A, typename A2, typename A3, typename A4, typename A5>
  bool Call(MF mem_fn, A a, A2 a2, A3 a3, A4 a4, A5 a5);
  /**
   * register member function to be executed here
   * @param mem_fn a pointer to member function from class object_type that accept 6 parameters
   * @param a the function parameter
   * @param a2 the function parameter
   * @param a3 the function parameter
   * @param a4 the function parameter
   * @param a5 the function parameter
   * @param a6 the function parameter
   * @return false if the member function start was not called yet!
   */
  template<typename MF, typename A, typename A2, typename A3, typename A4, 
           typename A5, typename A6>
  bool Call(MF mem_fn, A a, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6);
  
  /**
   * this function would allow to reset the timeout value to handle requests - note
   * that if the thread has started without a user function inside it it would have no
   * effect
   * @param timeout the timeout that is allowed to read from queue
   */
  void ResetQueueTimeout(osal::milliseconds_t timeout);
  
private:
  
  void StartThread(const char* name, osal::Thread::PriorityType prio);
  
  // note that this must be here since we may have to initialized
  // for differenet types and we don't want dependencies
  struct Runner
  {
    static WorkingQueueEntry* mAction;
    static void Start();    
  };
  // this function is used so that we would stop the internal thread only
  void StopFunction() const {}
  // insert new functor into queue
  void InsertJobIntoQueue(WorkingQueueEntryAutoPtr p);
  // this function is called by the thread as the entry point
  // this is the function that is starting from the new thread created by member function Start
  // Read functors from the queue
  void MainLoop();
  
private:
  
private:
  T*                           mInstance;
  WorkingQueue                 mQueue;
  osal::Mutex::Id*             mGuard;
  osal::EventNotification::Id* mThreadStarted;
  osal::EventNotification::Id* mThreadEnded;
  bool                         mStopThread;
  osal::Thread::Id*            mWorkingThread;
  boost::shared_ptr<details::InternalWork> mRequestHandler;
}; 

} // end of namespace asynccallbacks

// Executer clas implementation details
#include <asynccallbacks/details/Executer.hpp>
