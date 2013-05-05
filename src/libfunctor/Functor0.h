////////////////////////////////////////////////////////////////////////////////
// NAME: 
//   Functor0 -
//
// SYNOPSIS:	Callback objects library - Functors without parameters ("0")
// DESCRIPTION:	Functor objects encapsulating object method or static function.
//				A diffrent class is provided for each number of arguments with or
//				without a return value.
//				Client function/method also receive a typeless client data.
//				Callback objects are created using:
//				MakeFunctor##(object, method, client data)
//				where ## stands for argument & return value (e.g. MakeFunctor1wRet)
//				NT does not differentiate const methods from non const ones		
// EXAMPLES:	TestFunctors.cxx
// FILES:		Functor.h, Functor.cxx
// BUGS:
// SEE ALSO:
// TYPE:		LIBRARY-C++
////////////////////////////////////////////////////////////////////////////////

#ifndef FUNCTOR0_H
#define FUNCTOR0_H

#include <libfunctor/FunctorBase.h>


//------------------------------------------------------------------------------
//	Callback functor without arguments or return value
//------------------------------------------------------------------------------

//	General Functor abstract class
class Functor0 : public FunctorBase	{
protected:
	typedef	Functor0				FunctorObj;
	Functor0()										{}
	Functor0(const Functor0& o)	: FunctorBase(o)	{}
	Functor0(void* d) : FunctorBase(d)				{}
	Functor0&			operator = (FunctorObj& o);

public:
	virtual	~Functor0()								{}
	virtual	void		operator() () const			{}

	//	Implement Reference counting interface
	static FunctorObj*	NewCounter(FunctorObj* obj)	{	return obj;	}
};	


//	Functor for static function
class Functor0Func : public Functor0	{
public:
	typedef	Functor0Func				FunctorObj;
	typedef	void	(*Func) (void* ClientData);
	Functor0Func(Func f, void* d)
		:	Functor0(d), _func(f)					{}
	virtual	void		operator() () const
		{	_func( this->_data );	}

private:
	Func				_func;

	//	Block default copy operations
	Functor0Func(const FunctorObj& o);
	FunctorObj&			operator =(const FunctorObj& o);
};

//	Functor for Object method
template <class Callee>
class Functor0Method : public Functor0	{
public:
	typedef	Functor0Method<Callee>		FunctorObj;
	typedef	void	(Callee::*Method ) (void* ClientData);
	Functor0Method(Callee& c, Method m, void* d)
		:	Functor0(d), _callee(c), _method(m)		{}
	virtual	void		operator() () const
		{	(_callee.*_method)(this->_data);	}

private:
	Callee&				_callee;
	Method				_method;

	//	Block default copy operations
	Functor0Method(const FunctorObj& o);
	FunctorObj&			operator =(const FunctorObj& o);
};

#ifndef _NT
template <class Callee>
class Functor0CMethod : public Functor0	{
public:
	typedef	Functor0CMethod<Callee>		FunctorObj;
	typedef	void	(Callee::*Method ) (void* ClientData) const;
	Functor0CMethod(const Callee& c, Method m, void* d)
		:	Functor0(d), _callee(c), _method(m)		{}
	virtual	void		operator() () const
		{	(_callee.*_method)( this->_data );	}

private:
	const Callee&		_callee;
	Method				_method;

	//	Block default copy operations
	Functor0CMethod(const FunctorObj& o);
	FunctorObj&			operator =(const FunctorObj& o);
};
#endif


//	The MakeFunctor0 function is Overloaded to create the right Functor object
inline	Functor0*
MakeFunctor0(void (*f)(void*), void* d )
{
	return new Functor0Func( f, d );
}

template <class Callee>
Functor0*
MakeFunctor0(Callee& c, void (Callee::*m)(void*), void* d )
{
 	return new Functor0Method<Callee> ( c, m, d );
}

#ifndef _NT
template <class Callee>
Functor0*
MakeFunctor0(const Callee& c, void (Callee::*m)(void*) const, void* d )
{
 	return new Functor0CMethod<Callee> ( c, m, d );
}
#endif

//------------------------------------------------------------------------------
//	Callback functor with return value without arguments
//------------------------------------------------------------------------------
//	General Functor abstract class
template <class RT>
class Functor0wRet : public FunctorBase	{
protected:
	typedef	Functor0wRet<RT>			FunctorObj;
	Functor0wRet()									{}
	Functor0wRet(void* d) : FunctorBase(d)			{}
	FunctorObj&			operator = (FunctorObj& o);

public:
	virtual	~Functor0wRet()							{}
	virtual	RT			operator() () const			{	return RT();	}

	//	Implement Reference counting interface
	static FunctorObj*	NewCounter(FunctorObj* obj)	{	return obj;	}
};


//	Functor for static function
template <class RT>
class Functor0wRetFunc : public Functor0wRet<RT>	{
public:
	typedef	Functor0wRetFunc<RT>				FunctorObj;
	typedef	RT		(*Func) (void* ClientData);
	Functor0wRetFunc(Func f, void* d)
		:	Functor0wRet<RT>(d), _func(f)			{}
	virtual	RT			operator() () const
		{	return _func( this->_data );	}

private:
	Func				_func;

	//	Block default copy operations
	Functor0wRetFunc(const FunctorObj& o);
	FunctorObj&			operator =(const FunctorObj& o);
};

//	Functor for Object method
template <class Callee, class RT>
class Functor0wRetMethod : public Functor0wRet<RT>	{
public:
	typedef	Functor0wRetMethod<Callee,RT>		FunctorObj;
	typedef	RT		(Callee::*Method ) (void* ClientData);
	Functor0wRetMethod(Callee& c, Method m, void* d)
		:	Functor0wRet<RT>(d), _callee(c), _method(m)	{}
	virtual	RT			operator() () const
		{	return (_callee.*_method)(this->_data);	}

private:
	Callee&				_callee;
	Method				_method;

	//	Block default copy operations
	Functor0wRetMethod(const FunctorObj& o);
	FunctorObj&			operator =(const FunctorObj& o);
};

#ifndef _NT
template <class Callee, class RT>
class Functor0wRetCMethod : public Functor0wRet<RT>	{
public:
	typedef	Functor0wRetCMethod<Callee,RT>		FunctorObj;
	typedef	RT		(Callee::*Method ) (void* ClientData) const;
	Functor0wRetCMethod(const Callee& c, Method m, void* d)
		:	Functor0wRet<RT>(d), _callee(c), _method(m)	{}
	virtual	RT			operator() () const
		{	return (_callee.*_method)(this->_data);	}

private:
	const Callee&		_callee;
	Method				_method;

	//	Block default copy operations
	Functor0wRetCMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};
#endif


//	The MakeFunctor0wRet function is Overloaded to create the right Functor object
template <class RT>
Functor0wRet<RT>*
MakeFunctor0wRet(RT (*f)(void*), void* d )
{
	return new Functor0wRetFunc<RT>( f, d );
}

template <class Callee, class RT>
Functor0wRet<RT>*
MakeFunctor0wRet(Callee& c, RT (Callee::*m)(void*), void* d )
{
	return new Functor0wRetMethod<Callee, RT> ( c, m, d );
}

#ifndef _NT
template <class Callee, class RT>
Functor0wRet<RT>*
MakeFunctor0wRet(const Callee& c, RT (Callee::*m)(void*) const, void* d )
{
	return new Functor0wRetCMethod<Callee, RT> ( c, m, d );
}
#endif

#endif // FUNCTOR0_H

//////////// File: Functor0.h //////////////////////////////////////////////////
