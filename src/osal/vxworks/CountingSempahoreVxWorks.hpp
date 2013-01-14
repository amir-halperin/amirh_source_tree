#ifndef COUNTINGSEMPAHOREVXWORKS_HPP
#define COUNTINGSEMPAHOREVXWORKS_HPP

#include "OsalTimeUtils.h"	     // Milli2Ticks and error handling functions
#include "SemHandle.h"	// the vxworks semaphore handle is implemted here
#include "CriticalSection.h"		// to guard the error handling functions
#include "ExitFunctionHolder.h"		// to define the list of error handling functions
#include <errnoLib.h>             // errnoGet and the errno values
#include <semLib.h>               // vxworks semaphore API
#include <assert.h>               // assert macro
#include <vxworks.h>              // vxworks types
#include <memory>                 // auto_ptr

namespace osal
{

namespace CountingSemaphore
{

namespace
{
// local functions
Private::ExitFunctionHolder& ErrorFunctionsHandler();


Private::ExitFunctionHolder& ErrorFunctionsHandler()
{
	static Private::ExitFunctionHolder functions;
	return functions;
}

}	// end of local namespace 

///////////////////////////////////////////////////////////////////////////////

struct  Id : public semvx::SemHandle
{
	Id()
	{
	}
};

void RegisterAtExit(at_error_fun func)
{
  ErrorFunctionsHandler().Push(func);
}

Id* Create(int count)
{
	std::auto_ptr<Id> id(new Id);
	if (id->Open(count))
	{
		return id.release();
	}
	else
	{
	  ErrorFunctionsHandler().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
		return 0;	// just so that we would not have a warning about not returnning
	}
}

void Wait(Id* on)
{
	assert(on);
	if (!on->Take())
	{
	  ErrorFunctionsHandler().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
	}
}

bool TryWait(Id* on)
{
	assert(on);
	if (!on->TryTake())
	{
	  return ErrorFunctionsHandler().TryFail(errnoGet(), __FUNCTION__, __LINE__);
	}
	else
	{
		return true;	// all ok
	}
}

bool TimedWait(Id* on, milliseconds_t milliDuration)
{
	assert(on);
	if (!on->Take(TimeUtils::Milli2Ticks(milliDuration)))
	{
	  return ErrorFunctionsHandler().TimedOut(errnoGet(), __FUNCTION__, __LINE__);
	}
	else
	{
		return true;	// all is OK
	}
}

void Post(Id* on)
{
	assert(on);
	if (!on->Give())
	{
	  ErrorFunctionsHandler().CriticalError(errnoGet(), __FUNCTION__, __LINE__);
	}
}

void Delete(Id*& what)
{
	if (what)
	{
	  delete what;
	  what = 0;
	}
}

}	// end of namespace CountingSemaphore
}	// end of namespace osal
#else
#	error "you are including a file that should never be included in header file"
#endif	// COUNTINGSEMPAHOREVXWORKS_HPP
