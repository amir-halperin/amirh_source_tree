#ifndef MUTEXVXWORKS__HPP
#define MUTEXVXWORKS__HPP

#include "osal/Mutex.h"              // this file header file (we are doing the implementation here)
#include "SemHandle.h"	 // implement the semaphore handle
#include "OsalTimeUtils.h"	         // Milli2Ticks
#include "CriticalSection.h"		     // to guard the error handling functions
#include "ExitFunctionHolder.h"	     // to define the list of error handling functions
#include <semLib.h>		               // semaphore function calls in vxworks
#include <vxWorks.h>	               // vxworks types
#include <taskLib.h>                 // taskIdSelf
#include <errnoLib.h>                // errno function and values
#include <assert.h>                  // assert macro
#include <memory>                    // auto_ptr

namespace osal
{
  

namespace Mutex
{

namespace
{

void HandleSysCallError(int errno);
bool HandleTimeoutSemTake(int errno);
bool HandleTrySemTake(int errno);
Private::ExitFunctionHolder& ExitFunctionsList();


Private::ExitFunctionHolder& ExitFunctionsList()
{
  static Private::ExitFunctionHolder holder;
  return holder;
}

}	// end of local namespace

struct Id : public semvx::SemHandle
{
	Id()
	{
	}
	
	virtual ~Id()
	{
	  
	}
	
	virtual void Verify() const
	{
	  // do nothing here
	}
	
	virtual void SetOwner()
	{
	  
	}
	
};

struct IdNoRecuse : public Id
{
  IdNoRecuse() : mCurrentOwner(-1)
  {
  }
  
  virtual void Verify() const
  {
    // we want to make sure that only the task that took the semaphore would release it
    OSAL_ASSERT_CONDITION(mCurrentOwner == taskIdSelf(), ExitFunctionsList(), "release called with invalid owner");
  }
  
  virtual void SetOwner()
  {
    mCurrentOwner = taskIdSelf();
  }
  
  int mCurrentOwner;
};

void RegisterAtExit(at_error_fun func)
{
  ExitFunctionsList().Push(func);
}

Id* Create()
{
  std::auto_ptr<Id> id(new IdNoRecuse);
  if (!id->Open("createNoRecursive"))
  {
    ExitFunctionsList().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
  }
  return id.release();
}

Id* CreateRecursive(Protection prioritySafe)
{
	std::auto_ptr<Id> id(new Id);
	if (!id->Open(prioritySafe == PRIORITY_SAFE))
	{
	  ExitFunctionsList().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
	}
	return id.release();
}

void Lock(Id* id)
{
  OSAL_ASSERT_CONDITION(id, ExitFunctionsList(), "invalid mutex id");
	if (!id->Take())
	{
	  ExitFunctionsList().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
	}
	else
	{
	  id->SetOwner();
	}
}

bool TryLock(Id* id)
{
  OSAL_ASSERT_CONDITION(id, ExitFunctionsList(), "invalid mutex id");
	if (!id->TryTake())
	{
	  return ExitFunctionsList().TryFail(errnoGet(), __FUNCTION__, __LINE__);
	}
	else
	{
	  id->SetOwner();
		return true;
	}
}

bool TimedLock(Id* id, milliseconds_t milliDuration)
{
  OSAL_ASSERT_CONDITION(id, ExitFunctionsList(), "invalid mutex id");
	if (!id->Take(TimeUtils::Milli2Ticks(milliDuration)))
	{
	  return ExitFunctionsList().TimedOut(errnoGet(), __FUNCTION__, __LINE__);
	}
	else
	{
	  id->SetOwner();
		return true;
	}
}

void Release(Id* id)
{
  OSAL_ASSERT_CONDITION(id, ExitFunctionsList(), "invalid mutex id");
	id->Verify();    // this function if fail will not return
	if (!id->Give())
	{
	  ExitFunctionsList().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
	}
}

void Delete(Id*& id)
{
	if (id)
	{
		delete id;	// would call the close function for this object
		id = 0;
	}
}

}	// end of Mutex namespace
}	// end of osal namespace

#else
#	error "you cannot include this file inside header file"
#endif	// MUTEXVXWORKS__HPP
