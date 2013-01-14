#ifndef SEMAPHOREVXWORKSHANDLE__H
#define SEMAPHOREVXWORKSHANDLE__H

#ifndef __VXWORKS__
#	error "you cannot compile this not for vxworks target"
#endif	// __VXWORKS__

#include <semLib.h>

namespace semvx
{

struct SemHandle
{	
	typedef SEM_ID value_type;
	typedef void (SemHandle::*boolean_overload)() const;
	
	enum types
	{
		BINARY_SEM, 
	};
	
	// default would do nothing
	SemHandle();
	
	~SemHandle();
	
	// to allow the creation of this object with a valid mutex
	explicit SemHandle(bool inverseProtection);
	// open bianry semaphore
	bool Open();
	// open counting semaphore
	bool Open(int initialCount);
	// open mutex
	bool Open(bool inverseProtection);
	
	// for overloading only - this is to support
	// binary semaphore that is full at creation
	bool Open(const char*);
	
	bool Close();
	
	bool Take() const;
	
	bool Give() const;
	
	bool ReleaseAll() const;
	
	bool Take(int ticks) const;
	
	bool TryTake() const;
	
	operator boolean_overload () const;
	
	value_type Get() const;
	
	
private:
	// don't allow copy and assign for this object!
	SemHandle(const SemHandle&);
	SemHandle& operator = (const SemHandle&);

	void ForBoolOverload() const {}

	value_type mData;
	
};

}	// end of namespace semvx

#endif	// SEMAPHOREVXWORKSHANDLE__H
