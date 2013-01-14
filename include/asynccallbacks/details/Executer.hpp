#include <osal/StopWatch.h>                             // to measure how long we have being in the queue
#include <osal/Mutex.h>                                 // we need thread safe entry function
#include <osal/EventNotification.h>                     // we need to know when the thread is starting

namespace asynccallbacks {

namespace details
{
  // add a collection of functions that can be used to drive
  // the internal task
  inline InternalWork::InternalWork(WorkingQueue& queue, osal::milliseconds_t timeout) : mQueue(queue), mMaxTimeHandling(timeout)
  {    
  }
  
  inline InternalWork::~InternalWork()
  {    
  }
  
  inline void InternalWork::Timeout(osal::milliseconds_t newTime)
  {
    mMaxTimeHandling = newTime;
  }
  
  inline void InternalWork::operator ()()
  {
    HandleRequests(mItem);
  }
  
  inline WorkingQueue& InternalWork::Queue()
  {
    return mQueue;
  }
  
  inline osal::milliseconds_t InternalWork::Timeout() const
  {
    return mMaxTimeHandling;
  }
  
  struct NoTimeoutHandle : InternalWork
  {
    NoTimeoutHandle(WorkingQueue& queue, osal::milliseconds_t timeout) : InternalWork(queue, timeout)
    {
    }
    
private:    
    void HandleRequests(WorkingQueueEntry& item)
    {
      if (Queue().Pop(item))
      {
        item();
      }
    }
  };
  
  template<typename F>
  struct ExcuterTimeoutHandler : InternalWork
  {
    ExcuterTimeoutHandler(WorkingQueue& queue, osal::milliseconds_t timeout, F userAction) : 
            InternalWork(queue, timeout), UserFunction(userAction)
    {
    }
    
  private:    
    void HandleRequests(WorkingQueueEntry& item)
    {
      // in this case we would like to make sure that 
      // we don't read from the queue for more than the timeout value
      // that passes to this as max timeout for handling
      // the requests. To make sure that this is working
      // we need to measure the time that pass between the call to
      // queue and the time we returned
      ReadFromQueue(item);
      UserFunction();
    }
    
    void ReadFromQueue(WorkingQueueEntry& item)
    {
      osal::milliseconds_t timeout = Timeout();
      osal::milliseconds_t operTime = 0;
      osal::StopWatchOper sw;
      while (timeout > operTime)
      {
        if (!Queue().Pop(item, (timeout - operTime)))
        {
          return; // there is nothing that needs to be done any more
        }
        else
        {
          item(); // run the operation
          operTime = sw.Pause();  // measure the time it took to execute this
        }
      }
    }
    
  private:
    F UserFunction;
  };
  
  template<typename F>
  InternalWork* GenerateHandler(WorkingQueue& queue, osal::milliseconds_t timeout, F func)
  {
    if (timeout)
    {
      return new ExcuterTimeoutHandler<F>(queue, timeout, func);
    }
    else
    {
      return new NoTimeoutHandle(queue, timeout);
    }
  }
  
} //end of namespace details
  
template<typename T>
Executer<T>::Executer(T* thisPtr, unsigned int qLen) : mInstance(thisPtr), mQueue(qLen), 
                                                       mGuard(0), mThreadStarted(0), mThreadEnded(0),
                                                       mStopThread(true), mWorkingThread(0)
{
#if !defined(USE_SYNC_CALL_FOR_EXECUTER_OBJECT)
  mGuard = osal::Mutex::Create();
  mThreadStarted = osal::EventNotification::Create();
  mThreadEnded = osal::EventNotification::Create();
#endif  // USE_SYNC_CALL_FOR_EXECUTER_OBJECT
}

template<typename T>
Executer<T>::~Executer()
{
  this->Stop(); // make sure that the thread is not running any more
  
#if !defined(USE_SYNC_CALL_FOR_EXECUTER_OBJECT)
  osal::Mutex::Delete(mGuard);
  osal::EventNotification::Delete(mThreadStarted);
  osal::EventNotification::Delete(mThreadEnded);
#endif // USE_SYNC_CALL_FOR_EXECUTER_OBJECT
}

template<typename T>
void Executer<T>::StartThread(const char* name, osal::Thread::PriorityType prio)
{
#if !defined(USE_SYNC_CALL_FOR_EXECUTER_OBJECT)
  std::auto_ptr<WorkingQueueEntry> item = MakeWorkingQueueEntry(&Executer<T>::MainLoop, this);
  Runner::mAction = item.release();
 
  static const unsigned int MIN_STACK_SIZE = 1024*1024;
  unsigned int stackSize = mQueue.MaxSize()*10;
  if (stackSize < MIN_STACK_SIZE)
  {
    stackSize = MIN_STACK_SIZE;
  }
  if (prio == osal::Thread::INVALID_PRIORITY)
  {
    prio = osal::Thread::Self::Priority(); // same priority as this one
  }
  osal::Thread::Attributes attr(name, stackSize, prio);
  mWorkingThread = osal::Thread::Create(attr, Runner::Start);
  osal::EventNotification::Wait(mThreadStarted);
  Runner::mAction = 0;  // clean it up
#endif  // USE_SYNC_CALL_FOR_EXECUTER_OBJECT
}

template<typename T>
template<typename F>
void Executer<T>::Start(const char* name, F userFunc, 
             osal::milliseconds_t maxTimeut, osal::Thread::PriorityType prio)
{
#if !defined(USE_SYNC_CALL_FOR_EXECUTER_OBJECT)
  utils::CriticalSection cs(mGuard); // protect this function we my have multi thread access here
#endif  // USE_SYNC_CALL_FOR_EXECUTER_OBJECT
  if (!mWorkingThread)
  {
    mRequestHandler.reset(details::GenerateHandler(mQueue, maxTimeut, userFunc));
    mStopThread = false;
    StartThread(name, prio);
  }
}

template<typename T>
void Executer<T>::Start(const char* name, osal::Thread::PriorityType prio) 
{
#if !defined(USE_SYNC_CALL_FOR_EXECUTER_OBJECT)
  utils::CriticalSection cs(mGuard); // protect this function we my have multi thread access here
#endif  // USE_SYNC_CALL_FOR_EXECUTER_OBJECT
  if (!mWorkingThread)
  {
    mRequestHandler.reset(details::GenerateHandler(mQueue, 0, DoNothingFunction()));
    mStopThread = false;
    StartThread(name, prio);
  }
}

template<typename T>
void Executer<T>::Stop()
{
  mStopThread = true;
#if !defined(USE_SYNC_CALL_FOR_EXECUTER_OBJECT)
  utils::CriticalSection cs(mGuard); // protect this function we my have multi thread access here
  if (mWorkingThread)
  {
    WorkingQueueEntryAutoPtr p = MakeWorkingQueueEntry(&Executer<T>::StopFunction, this);
    mQueue.Push(p.release());
    osal::EventNotification::Wait(mThreadEnded);
    osal::Thread::Clean(mWorkingThread);
    mWorkingThread = 0;
  }
#endif  // USE_SYNC_CALL_FOR_EXECUTER_OBJECT
}

template<typename T> template<typename MF>
bool Executer<T>::Call(MF mem_fn)
{
  if (!mStopThread)
  {
    InsertJobIntoQueue(MakeWorkingQueueEntry(mem_fn, mInstance));
  }
  return !mStopThread;
}

template<typename T> template<typename MF, typename A>
bool Executer<T>::Call(MF mem_fn, A a)
{
  if (!mStopThread)
  {
    InsertJobIntoQueue( MakeWorkingQueueEntry(mem_fn, mInstance, a));
  }
  return !mStopThread;
}

template<typename T> template<typename MF, typename A, typename A2>
bool Executer<T>::Call(MF mem_fn, A a, A2 a2)
{
  if (!mStopThread)
  {
    InsertJobIntoQueue( MakeWorkingQueueEntry(mem_fn, mInstance, a, a2));
  }
  return !mStopThread;
}

template<typename T> template<typename MF, typename A, typename A2, typename A3>
bool Executer<T>::Call(MF mem_fn, A a, A2 a2, A3 a3)
{
  if (!mStopThread)
  {
    InsertJobIntoQueue( MakeWorkingQueueEntry(mem_fn, mInstance, a, a2, a3));
  }
  return !mStopThread;
}

template<typename T> template<typename MF, typename A, typename A2, typename A3, typename A4>
bool Executer<T>::Call(MF mem_fn, A a, A2 a2, A3 a3, A4 a4)
{
  if (!mStopThread)
  {
    InsertJobIntoQueue(MakeWorkingQueueEntry(mem_fn, mInstance, a, a2, a3, a4));
  }
  return !mStopThread;
}

template<typename T> template<typename MF, typename A, typename A2, typename A3, typename A4, typename A5>
bool Executer<T>::Call(MF mem_fn, A a, A2 a2, A3 a3, A4 a4, A5 a5)
{
  if (!mStopThread)
  {
    InsertJobIntoQueue(MakeWorkingQueueEntry(mem_fn, mInstance, a, a2, a3, a4, a5));
  }
  return !mStopThread;
}

template<typename T> template<typename MF, typename A, typename A2, typename A3, typename A4, typename A5, typename A6>
bool Executer<T>::Call(MF mem_fn, A a, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
{
  if (!mStopThread)
  {
    InsertJobIntoQueue(MakeWorkingQueueEntry(mem_fn, mInstance, a, a2, a3, a4, a5, a6));
  }
  return !mStopThread;
}

// this function is called by the thread as the entry point
// this is the function that is starting from the new thread created with member function 
// start and would read requests from the queue
template<typename T>
void Executer<T>::MainLoop()
{
#if !defined(USE_SYNC_CALL_FOR_EXECUTER_OBJECT)
  osal::EventNotification::Signal(mThreadStarted);  // tell the main thread that we started
  
  while (!mStopThread)
  {
    // extract item from the queue and execute them
    (*mRequestHandler)();
  }
  osal::EventNotification::Signal(mThreadEnded);  // notify that the thread is no longer running
#endif  // USE_SYNC_CALL_FOR_EXECUTER_OBJECT
}

template<typename T>
void Executer<T>::InsertJobIntoQueue(WorkingQueueEntryAutoPtr p)
{
#ifdef USE_SYNC_CALL_FOR_EXECUTER_OBJECT
//#warning "compile without support for async operation"
  (*p)(); // execute it here..
#else
  mQueue.Push(p.release());
#endif  // USE_SYNC_CALL_FOR_EXECUTER_OBJECT
}

// static 
template<typename T>
void Executer<T>::Runner::Start()
{
  //assert(mAction);
  if (!mAction)
  {
    return;
  }
  (*mAction)();
}

template<typename T>
void Executer<T>::ResetQueueTimeout(osal::milliseconds_t timeout)
{
  if (mRequestHandler)
  {
    mRequestHandler->Timeout(timeout);
  }
}
// must initialized the static member of runner here
template<typename T>
WorkingQueueEntry* Executer<T>::Runner::mAction = 0;

} // namespace asyccallbacks
