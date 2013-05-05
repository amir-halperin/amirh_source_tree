////////////////////////////////////////////////////////////////////////////////
// NAME: 
//   Functor3 -
//
// SYNOPSIS:	Callback objects library - Functors with 3 parameter
// DESCRIPTION:	Functor objects encapsulating object method or static function.
//				A diffrent class is provided for each number of arguments with or
//				without a return value.
//				Client function/method also receive a typeless client data.
//				Callback objects are created using:
//				MakeFunctor##(object, method, client data)
//				where ## stands for argument & return value (e.g. MakeFunctor1wRet)
//		NT does not differentiate const methods from non const ones		
// EXAMPLES:	TestFunctors.cxx
// FILES:		Functor.h, Functor.cxx
// BUGS:
// SEE ALSO:
// TYPE:		LIBRARY-C++
////////////////////////////////////////////////////////////////////////////////

#ifndef FUNCTOR3_H
#define FUNCTOR3_H

#include <libfunctor/FunctorBase.h>


//------------------------------------------------------------------------------
//	Callback functor with 3 argument without return value
//------------------------------------------------------------------------------

//	General Functor abstract class
template<class P1, class P2, class P3>
class Functor3 : public FunctorBase {
protected:
	typedef	Functor3<P1,P2,P3>				FunctorObj;
	Functor3()										{}
	Functor3(const FunctorObj& o) : FunctorBase(o)	{}
	Functor3(void* d) : FunctorBase(d)				{}
	FunctorObj&		operator = (FunctorObj& o);

public:
	virtual	~Functor3()								{}
	virtual	void	operator() (P1, P2, P3) const	{}

	//	Implement Reference counting interface
	static FunctorObj*	NewCounter(FunctorObj* obj)	{	return obj;	}
};

//	Functor for static function
template<class P1, class P2, class P3>
class Functor3Func : public Functor3<P1, P2, P3>	{
public:
	typedef	Functor3Func<P1,P2,P3>					FunctorObj;
	typedef	void	(*Func) (P1, P2, P3, void* ClientData);
	Functor3Func(Func f, void* d)
		:	Functor3<P1,P2,P3>(d), _func(f)						{}
	virtual	void	operator() (P1 p1, P2 p2, P3 p3) const
		{	_func( p1, p2, p3, this->_data );	}

private:
	Func			_func;

	//	Block default copy operations
	Functor3Func(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

//	Functor for Object method
template <class Callee, class P1, class P2, class P3>
class Functor3Method : public Functor3<P1, P2, P3>	{
public:
	typedef	Functor3Method<Callee,P1,P2,P3>			FunctorObj;
	typedef	void	(Callee::*Method ) (P1, P2, P3, void* ClientData);
	Functor3Method(Callee& c, Method m, void* d)
		:	Functor3<P1,P2,P3>(d), _callee(c), _method(m)		{}
	virtual	void	operator() (P1 p1, P2 p2, P3 p3) const
		{	(_callee.*_method) ( p1, p2, p3, this->_data );	}

private:
	Callee&			_callee;
	Method			_method;

	//	Block default copy operations
	Functor3Method(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

#ifndef _NT
template <class Callee, class P1, class P2, class P3>
class Functor3CMethod : public Functor3<P1, P2, P3>	{
public:
	typedef	Functor3CMethod<Callee,P1,P2,P3>		FunctorObj;
	typedef	void	(Callee::*Method ) (P1, P2, P3, void* ClientData) const;
	Functor3CMethod(const Callee& c, Method m, void* d)
		:	Functor3<P1,P2,P3>(d), _callee(c), _method(m)		{}
	virtual	void	operator() (P1 p1, P2 p2, P3 p3) const
		{	(_callee.*_method) ( p1, p2, p3, this->_data );	}

private:
	const Callee&	_callee;
	Method			_method;

	//	Block default copy operations
	Functor3CMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};
#endif


//	The MakeFunctor3 function is Overloaded to create the right Functor object
template <class P1, class P2, class P3>
Functor3<P1, P2, P3>*
MakeFunctor3(void (*f)(P1, P2, P3, void* ClientData), void* d)
{
	return new Functor3Func<P1, P2, P3>( f, d );
}

template <class Callee, class P1, class P2, class P3>
Functor3<P1, P2, P3>*
MakeFunctor3(Callee& c, void (Callee::*m)(P1,P2,P3,void*), void* d)
{
 	return new Functor3Method<Callee, P1, P2, P3> ( c, m, d );
}

#ifndef _NT
template <class Callee, class P1, class P2, class P3>
Functor3<P1, P2, P3>*
MakeFunctor3(const Callee& c, void (Callee::*m)(P1,P2,P3,void*) const, void* d)
{
 	return new Functor3CMethod<Callee, P1, P2, P3> ( c, m, d );
}
#endif


//--------------------------------------------------------------------------------
//	Callback functor with 3 argument with return value
//--------------------------------------------------------------------------------
//	General Functor abstract class
template <class RT, class P1, class P2, class P3>
class Functor3wRet : public FunctorBase {
protected:
	typedef	Functor3wRet<RT,P1,P2,P3>				FunctorObj;
	Functor3wRet()									{}
	Functor3wRet(const FunctorObj& o) : FunctorBase(o)	{}
	Functor3wRet(void* d) : FunctorBase(d)			{}
	FunctorObj&			operator = (FunctorObj& o);

public:
	virtual	~Functor3wRet()							{}
	virtual	RT		operator() (P1, P2, P3) const	{ return RT(); }

	//	Implement Reference counting interface
	static FunctorObj*	NewCounter(FunctorObj* obj)	{ return obj; }
};

//	Functor for static function
template <class RT, class P1, class P2, class P3>
class Functor3wRetFunc : public Functor3wRet<RT, P1, P2, P3>	{
public:
	typedef	Functor3wRetFunc<RT,P1,P2,P3>			FunctorObj;
	typedef	RT		(*Func) (P1, P2, P3, void* ClientData);
	Functor3wRetFunc(Func f, void* d)
		:	Functor3wRet<RT,P1,P2,P3>(d), _func(f)		{}
	virtual	RT		operator() (P1 p1, P2 p2, P3 p3) const
		{	return _func( p1, p2, p3, this->_data );	}

private:
	Func			_func;

	//	Block default copy operations
	Functor3wRetFunc(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};


//	Functor for Object method
template <class Callee, class RT, class P1, class P2, class P3>
class Functor3wRetMethod : public Functor3wRet<RT, P1, P2, P3>	{
public:
	typedef	Functor3wRetMethod<Callee,RT,P1,P2,P3>	FunctorObj;
	typedef	RT		(Callee::*Method) (P1, P2, P3, void* ClientData);
	Functor3wRetMethod(Callee& c, Method m, void* d)
		:	Functor3wRet<RT,P1,P2,P3>(d), _callee(c), _method(m)	{}
	virtual	RT		operator() (P1 p1, P2 p2, P3 p3) const
		{	return (_callee.*_method) ( p1, p2, p3, this->_data );	}

private:
	Callee&			_callee;
	Method			_method;

	//	Block default copy operations
	Functor3wRetMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

#ifndef _NT
template <class Callee, class RT, class P1, class P2, class P3>
class Functor3wRetCMethod : public Functor3wRet<RT, P1, P2, P3>	{
public:
	typedef	Functor3wRetCMethod<Callee,RT,P1,P2,P3>	FunctorObj;
	typedef	RT		(Callee::*Method) (P1, P2, P3, void* ClientData) const;
	Functor3wRetCMethod(const Callee& c, Method m, void* d)
		:	Functor3wRet<RT,P1,P2,P3>(d), _callee(c), _method(m)	{}
	virtual	RT		operator() (P1 p1, P2 p2, P3 p3) const
		{	return (_callee.*_method) ( p1, p2, p3, this->_data );	}

private:
	const Callee&	_callee;
	Method			_method;

	//	Block default copy operations
	Functor3wRetCMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};
#endif


//	The MakeFunctor3wRet function is Overloaded to create the right Functor object
template <class RT, class P1, class P2, class P3>
Functor3wRet<RT, P1, P2, P3>*
MakeFunctor3wRet(RT (*f)(P1, P2, P3, void* ClientData), void* d)
{
	return new Functor3wRetFunc<RT, P1, P2, P3>( f, d );
}

template <class Callee, class RT, class P1, class P2, class P3>
Functor3wRet<RT, P1,P2,P3>*
MakeFunctor3wRet(Callee& c, RT (Callee::*m)(P1,P2,P3,void*), void* d)
{
	return new Functor3wRetMethod<Callee, RT, P1, P2, P3> ( c, m, d );
}

#ifndef _NT
template <class Callee, class RT, class P1, class P2, class P3>
Functor3wRet<RT, P1,P2,P3>*
MakeFunctor3wRet(const Callee& c, RT (Callee::*m)(P1,P2,P3,void*) const, void* d)
{
	return new Functor3wRetCMethod<Callee,RT,P1,P2,P3> ( c, m, d );
}
#endif


#endif // FUNCTOR3_H

//////////// File: Functor3.h //////////////////////////////////////////////////
