#include "SemHandle.h"  // header file for this class
#include <vxWorks.h>  // vxworks types
//#include <stdio.h>    // remove this one

namespace semvx
{

SemHandle::SemHandle() : mData(0)
{
}

SemHandle::SemHandle(bool inverseProtection) : mData(0)
{
  Open(inverseProtection);
}
	
SemHandle::~SemHandle()
{
	if (Get())
	{
		Close();
	}
}
	
bool SemHandle::Open()
{
		// open binary sempahore - alway make it empty!
	if (!Get())
	{
		mData = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
		return mData != 0;
	}
	else
	{
		return false;
	}
}

bool SemHandle::Open(const char*)
{
  if (!Get())
  {
    mData = semBCreate(SEM_Q_PRIORITY, SEM_FULL);
		//printf("creating new binary semaphore with id = %p\r\n", mData);
    return mData != 0;
  }
  else
  {
    return false; // trying to re-open
  }
}
	
bool SemHandle::Open(int initialCount)
{
	// create couting semaphore
	if (!Get())
	{
		//printf("creating new semaphore with id = %p\r\n", mData);
		mData = semCCreate(SEM_Q_PRIORITY, initialCount);
		return mData != 0;
	}
	else
	{
		return false;
	}
}
	
bool SemHandle::Open(bool inverseProtection)
{
	if (!Get())
	{
		mData = semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE | (inverseProtection & SEM_INVERSION_SAFE));
		//printf("creating new mutex with id = %p\r\n", mData);
		return mData != 0;
	}
	else
	{
		return false;
	}
}
	
bool SemHandle::Close()
{
  static const int DEFAULT_TIMEOUT = 100; // this is about 1 second
  //printf("deleting semaphore %p\r\n", get());
	if (semTake(Get(), DEFAULT_TIMEOUT) == 0) 
	{
		semDelete(Get());
		mData = 0;
		return true;
	}
	else
	{
		return false;
	}
}
	
bool SemHandle::Take() const
{
	return semTake(Get(), WAIT_FOREVER) == OK;
}
	
bool SemHandle::Give() const
{
	return semGive(Get()) == OK;
}
	
bool SemHandle::Take(int ticks) const
{
	return semTake(Get(), ticks) == 0;
}
	
bool SemHandle::TryTake() const
{
	return semTake(Get(), NO_WAIT) == 0;
}
	
SemHandle::operator SemHandle::boolean_overload () const
{
	return Get() ? &SemHandle::ForBoolOverload : 0;
}

bool SemHandle::ReleaseAll() const
{
  return semFlush(Get()) == 0;
}
	
SemHandle::value_type SemHandle::Get() const
{
	return mData;
}

}	//	semvx
