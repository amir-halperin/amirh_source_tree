//////////////////////////////////////////////////////////////////////////////
// NAME: 
//   Functor4 -
//
// SYNOPSIS:	Callback objects library - Functors with 4 parameter
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

#ifndef FUNCTOR4_H
#define FUNCTOR4_H

#include <libfunctor/FunctorBase.h>


//--------------------------------------------------------------------------------
//	Callback functor with 4 argument without return value
//--------------------------------------------------------------------------------

//	General Functor abstract class
template<class P1, class P2, class P3, class P4>
class Functor4 : public FunctorBase {
protected:
	typedef	Functor4<P1,P2,P3,P4>				FunctorObj;
	Functor4()										{}
	Functor4(const FunctorObj& o) : FunctorBase(o)	{}
	Functor4(void* d) : FunctorBase(d)				{}
	FunctorObj&		operator = (FunctorObj& o);

public:
	virtual	~Functor4()								{}
	virtual	void	operator() (P1,P2,P3,P4) const	{}

	//	Implement Reference counting interface
	static FunctorObj*	NewCounter(FunctorObj* obj)	{ return obj; }
};

//	Functor for static function
template<class P1, class P2, class P3, class P4>
class Functor4Func : public Functor4<P1, P2, P3, P4>	{
public:
	typedef	Functor4Func<P1,P2,P3,P4>					FunctorObj;
	typedef	void	(*Func) (P1, P2, P3, P4, void* ClientData);
	Functor4Func(Func f, void* d)
		:	Functor4<P1,P2,P3,P4>(d), _func(f)			{}
	virtual	void	operator() (P1 p1, P2 p2, P3 p3, P4 p4) const
		{	_func( p1, p2, p3, p4, this->_data );	}

private:
	Func			_func;

	//	Block default copy operations
	Functor4Func(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

//	Functor for Object method
template <class Callee, class P1, class P2, class P3, class P4>
class Functor4Method : public Functor4<P1, P2, P3, P4>	{
public:
	typedef	Functor4Method<Callee,P1,P2,P3,P4>			FunctorObj;
	typedef	void	(Callee::*Method ) (P1, P2, P3, P4, void* ClientData);
	Functor4Method(Callee& c, Method m, void* d)
		:	Functor4<P1,P2,P3,P4>(d), _callee(c), _method(m)	{}
	virtual	void	operator() (P1 p1, P2 p2, P3 p3, P4 p4) const
		{	(_callee.*_method) ( p1, p2, p3, p4, this->_data );	}

private:
	Callee&			_callee;
	Method			_method;

	//	Block default copy operations
	Functor4Method(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

#ifndef _NT
template <class Callee, class P1, class P2, class P3, class P4>
class Functor4CMethod : public Functor4<P1, P2, P3, P4>	{
public:
	typedef	Functor4CMethod<Callee,P1,P2,P3,P4>			FunctorObj;
	typedef	void	(Callee::*Method ) (P1, P2, P3, P4, void* ClientData) const;
	Functor4CMethod(const Callee& c, Method m, void* d)
		:	Functor4<P1,P2,P3,P4>(d), _callee(c), _method(m)	{}
	virtual	void	operator() (P1 p1, P2 p2, P3 p3, P4 p4) const
		{	(_callee.*_method) ( p1, p2, p3, p4, this->_data );	}

private:
	const Callee&	_callee;
	Method			_method;

	//	Block default copy operations
	Functor4CMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};
#endif


//	The MakeFunctor4 function is Overloaded to create the right Functor object
template <class P1, class P2, class P3, class P4>
Functor4<P1, P2, P3, P4>*
MakeFunctor4(void (*f)(P1,P2,P3,P4,void*), void* d)
{
	return new Functor4Func<P1, P2, P3, P4>( f, d );
}

template <class Callee, class P1, class P2, class P3, class P4>
Functor4<P1, P2, P3, P4>*
MakeFunctor4(Callee& c, void (Callee::*m)(P1,P2,P3,P4,void*), void* d)
{
 	return new Functor4Method<Callee, P1, P2, P3, P4> ( c, m, d );
}

#ifndef _NT
template <class Callee, class P1, class P2, class P3, class P4>
Functor4<P1, P2, P3, P4>*
MakeFunctor4(const Callee& c, void (Callee::*m)(P1,P2,P3,P4,void*) const, void* d)
{
 	return new Functor4CMethod<Callee,P1,P2,P3,P4> ( c, m, d );
}
#endif

//--------------------------------------------------------------------------------
//	Callback functor with 3 argument with return value
//--------------------------------------------------------------------------------
//	General Functor abstract class
template <class RT, class P1, class P2, class P3, class P4>
class Functor4wRet : public FunctorBase {
protected:
	typedef	Functor4wRet<RT,P1,P2,P3,P4>			FunctorObj;
	Functor4wRet()									{}
	Functor4wRet(const FunctorObj& o) : FunctorBase(o)	{}
	Functor4wRet(void* d) : FunctorBase(d)			{}
	FunctorObj&		operator = (FunctorObj& o);

public:
	virtual	~Functor4wRet()							{}
	virtual	RT		operator() (P1,P2,P3,P4) const	{ return RT(); }

	//	Implement Reference counting interface
	static FunctorObj*	NewCounter(FunctorObj* obj)	{ return obj; }
};

//	Functor for static function
template <class RT, class P1, class P2, class P3, class P4>
class Functor4wRetFunc : public Functor4wRet<RT, P1, P2, P3, P4>	{
public:
	typedef	Functor4wRetFunc<RT,P1,P2,P3,P4>			FunctorObj;
	typedef	RT		(*Func) (P1, P2, P3, P4, void* ClientData);
	Functor4wRetFunc(Func f, void* d)
		:	Functor4wRet<RT,P1,P2,P3,P4>(d), _func(f)	{}
	virtual	RT		operator() (P1 p1, P2 p2, P3 p3, P4 p4) const
		{	return _func( p1, p2, p3, p4, this->_data );	}

private:
	Func			_func;

	//	Block default copy operations
	Functor4wRetFunc(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

//	Functor for Object method
template <class Callee, class RT, class P1, class P2, class P3, class P4>
class Functor4wRetMethod : public Functor4wRet<RT, P1, P2, P3, P4>	{
public:
	typedef	Functor4wRetMethod<Callee,RT,P1,P2,P3,P4>	FunctorObj;
	typedef	RT		(Callee::*Method) (P1, P2, P3, P4, void* ClientData);
	Functor4wRetMethod(Callee& c, Method m, void* d)
		:	Functor4wRet<RT,P1,P2,P3,P4>(d), _callee(c), _method(m)	{}
	virtual	RT		operator() (P1 p1, P2 p2, P3 p3, P4 p4) const
		{	return (_callee.*_method) ( p1, p2, p3, p4, this->_data );	}

private:
	Callee&			_callee;
	Method			_method;

	//	Block default copy operations
	Functor4wRetMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

#ifndef _NT
template <class Callee, class RT, class P1, class P2, class P3, class P4>
class Functor4wRetCMethod : public Functor4wRet<RT, P1, P2, P3, P4>	{
public:
	typedef	Functor4wRetCMethod<Callee,RT,P1,P2,P3,P4>	FunctorObj;
	typedef	RT		(Callee::*Method) (P1, P2, P3, P4, void* ClientData) const;
	Functor4wRetCMethod(const Callee& c, Method m, void* d)
		:	Functor4wRet<RT,P1,P2,P3,P4>(d), _callee(c), _method(m)	{}
	virtual	RT		operator() (P1 p1, P2 p2, P3 p3, P4 p4) const
		{	return (_callee.*_method) ( p1, p2, p3, p4, this->_data );	}

private:
	const Callee&	_callee;
	Method			_method;

	//	Block default copy operations
	Functor4wRetCMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};
#endif


//	The MakeFunctor4wRet function is Overloaded to create the right Functor object
template <class RT, class P1, class P2, class P3, class P4>
Functor4wRet<RT, P1, P2, P3, P4>*
MakeFunctor4wRet(RT (*f)(P1,P2,P3,P4,void*), void* d)
{
	return new Functor4wRetFunc<RT, P1, P2, P3, P4>( f, d );
}

template <class Callee, class RT, class P1, class P2, class P3, class P4>
Functor4wRet<RT, P1, P2, P3, P4>*
MakeFunctor4wRet(Callee& c, RT (Callee::*m)(P1,P2,P3,P4, void*), void* d)
{
	return new Functor4wRetMethod<Callee, RT, P1, P2, P3, P4> ( c, m, d );
}

#ifndef _NT
template <class Callee, class RT, class P1, class P2, class P3, class P4>
Functor4wRet<RT, P1, P2, P3, P4>*
MakeFunctor4wRet(const Callee& c, RT (Callee::*m)(P1,P2,P3,P4, void*) const, void* d)
{
	return new Functor4wRetCMethod<Callee,RT,P1,P2,P3,P4> ( c, m, d );
}
#endif


#endif // FUNCTOR4_H

//////////// File: Functor4.h //////////////////////////////////////////////////
