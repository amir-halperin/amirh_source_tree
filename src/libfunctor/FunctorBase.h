////////////////////////////////////////////////////////////////////////////////
// NAME: 
//   FunctorBase -
//
// SYNOPSIS:	Abstract Base class for all Callback objects
// DESCRIPTION:	Provides support for reference counting
// EXAMPLES:	TestFunctors.cpp
// FILES:		FunctorN.h
// BUGS:
// SEE ALSO:
// TYPE:		LIBRARY-C++
////////////////////////////////////////////////////////////////////////////////

#ifndef FUNCTORBASE_H
#define FUNCTORBASE_H


class FunctorBase {
public:
	virtual	void*	Data() const					{	return _data;	}

	//	Implement Reference counting interface
	unsigned int	GetCount() const				{	return _count;	}
	void			Connect()						{	++_count;	}
	void			Disconnect()					{	if(!--_count)  delete this;	}

protected:
	FunctorBase() : _count(0), _data(0)				{}
	FunctorBase(void* data) : _count(0), _data(data){}
	FunctorBase(const FunctorBase& o)
		:	_count(o._count), _data(o._data)		{}
	virtual	~FunctorBase()							{}
	FunctorBase&	operator =(const FunctorBase& o)
		{	_count=o._count;	_data=o._data;	return *this;	}

	unsigned int	_count;							//	Count object references
	void*			_data;
};	


#endif // FUNCTORBASE_H

//////////// File: FunctorBase.h ///////////////////////////////////////////////
