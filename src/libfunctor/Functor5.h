////////////////////////////////////////////////////////////////////////////////
// NAME: 
//   Functor5 -
//
// SYNOPSIS:	Callback objects library - Functors with 5 parameter
// DESCRIPTION:	Functor objects encapsulating object method or static function.
//				A diffrent class is provided for each number of arguments with or
//				without a return value.
//				Client function/method also receive a typeless client data.
//				Callback objects are created using:
//				MakeFunctor##(object, method, client data)
//				where ## stands for argument & return value (e.g. MakeFunctor1wRet)
// 		NT does not differentiate const methods from non const ones
//
// EXAMPLES:	TestFunctors.cxx
// FILES:		Functor.h, Functor.cxx
// BUGS:
// SEE ALSO:
// TYPE:		LIBRARY-C++
//////////////////////////////////////////////////////////////////////////////////

#ifndef FUNCTOR5_H
#define FUNCTOR5_H

#include <libfunctor/FunctorBase.h>

//------------------------------------------------------------------------------
//	Callback functor with 5 argument without return value
//------------------------------------------------------------------------------

//	General Functor abstract class
template<class P1, class P2, class P3, class P4, class P5>
class Functor5 : public FunctorBase {
protected:
	typedef	Functor5<P1,P2,P3,P4,P5>				FunctorObj;
	Functor5()										{}
	Functor5(const FunctorObj& o) : FunctorBase(o)	{}
	Functor5(void* d) : FunctorBase(d)				{}
	FunctorObj&		operator = (FunctorObj& o);

public:
	virtual	~Functor5()								{}
	virtual	void	operator() (P1, P2, P3, P4, P5) const	{}

	//	Implement Reference counting interface
	static FunctorObj*	NewCounter(FunctorObj* obj)	{ return obj; }
};

//	Functor for static function
template<class P1, class P2, class P3, class P4, class P5>
class Functor5Func : public Functor5<P1, P2, P3, P4, P5>	{
public:
	typedef	Functor5Func<P1,P2,P3,P4,P5>			FunctorObj;
	typedef	void	(*Func) (P1, P2, P3, P4, P5,void* ClientData);
	Functor5Func(Func f, void* d)
		:	Functor5<P1,P2,P3,P4,P5>(d), _func(f)	{}
	virtual	void	operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
		{	_func( p1, p2, p3, p4, p5, this->_data );	}

private:
	Func			_func;

	//	Block default copy operations
	Functor5Func(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};


//	Functor for Object method
template <class Callee, class P1, class P2, class P3, class P4, class P5>
class Functor5Method : public Functor5<P1, P2, P3, P4, P5>	{
public:
	typedef	Functor5Method<Callee,P1,P2,P3,P4,P5>	FunctorObj;
	typedef	void	(Callee::*Method ) (P1, P2, P3, P4, P5,void* ClientData);
	Functor5Method(Callee& c, Method m, void* d)
		:	Functor5<P1,P2,P3,P4,P5>(d), _callee(c), _method(m)	{}
	virtual	void	operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
		{	(_callee.*_method) ( p1, p2, p3, p4, p5, this->_data );	}

private:
	Callee&			_callee;
	Method			_method;

	//	Block default copy operations
	Functor5Method(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

#ifndef _NT
template <class Callee, class P1, class P2, class P3, class P4, class P5>
class Functor5CMethod : public Functor5<P1, P2, P3, P4, P5>	{
public:
	typedef	Functor5CMethod<Callee,P1,P2,P3,P4,P5>	FunctorObj;
	typedef	void	(Callee::*Method ) (P1, P2, P3, P4, P5,void* ClientData) const;
	Functor5CMethod(const Callee& c, Method m, void* d)
		:	Functor5<P1,P2,P3,P4,P5>(d), _callee(c), _method(m)	{}
	virtual	void	operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
		{	(_callee.*_method) ( p1, p2, p3, p4, p5, this->_data );	}

private:
	const Callee&	_callee;
	Method			_method;

	//	Block default copy operations
	Functor5CMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};
#endif


//	The MakeFunctor5 function is Overloaded to create the right Functor object
template <class P1, class P2, class P3, class P4, class P5>
Functor5<P1, P2, P3, P4, P5>*
MakeFunctor5(void (*f)(P1,P2,P3,P4,P5,void*), void* d)
{
	return new Functor5Func<P1, P2, P3, P4, P5>( f, d );
}

template <class Callee, class P1, class P2, class P3, class P4, class P5>
Functor5<P1, P2, P3, P4, P5>*
MakeFunctor5(Callee& c, void (Callee::*m)(P1,P2,P3,P4,P5,void*), void* d)
{
 	return new Functor5Method<Callee, P1, P2, P3, P4, P5> ( c, m, d );
}
#ifndef _NT
template <class Callee, class P1, class P2, class P3, class P4, class P5>
Functor5<P1, P2, P3, P4, P5>*
MakeFunctor5(const Callee& c, void (Callee::*m)(P1,P2,P3,P4,P5,void*) const, void* d)
{
 	return new Functor5CMethod<Callee, P1, P2, P3, P4, P5> ( c, m, d );
}
#endif


//------------------------------------------------------------------------------
//	Callback functor with 5 argument with return value
//------------------------------------------------------------------------------

//	General Functor abstract class
template <class RT, class P1, class P2, class P3, class P4, class P5>
class Functor5wRet : public FunctorBase {
protected:
	typedef	Functor5wRet<RT,P1,P2,P3,P4,P5>			FunctorObj;
	Functor5wRet()									{}
	Functor5wRet(const FunctorObj& o) : FunctorBase(o)	{}
	Functor5wRet(void* d) : FunctorBase(d)			{}
	FunctorObj&		operator = (FunctorObj& o);

public:
	virtual	~Functor5wRet()							{}
	virtual	RT		operator() (P1, P2, P3, P4, P5) const { return RT(); }

	//	Implement Reference counting interface
	static FunctorObj*	NewCounter(FunctorObj* obj)	{ return obj; }
};

//	Functor for static function
template <class RT, class P1, class P2, class P3, class P4, class P5>
class Functor5wRetFunc : public Functor5wRet<RT, P1, P2, P3, P4, P5>	{
public:
	typedef	Functor5wRetFunc<RT,P1,P2,P3,P4,P5>		FunctorObj;
	typedef	RT		(*Func) (P1, P2, P3, P4, P5,void* ClientData);
	Functor5wRetFunc(Func f, void* d)
		:	Functor5wRet<RT,P1,P2,P3,P4,P5>(d), _func(f)	{}
	virtual	RT		operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
		{	return _func( p1, p2, p3, p4, p5, this->_data );	}

private:
	Func			_func;

	//	Block default copy operations
	Functor5wRetFunc(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

//	Functor for Object method
template <class Callee, class RT, class P1, class P2, class P3, class P4, class P5>
class Functor5wRetMethod : public Functor5wRet<RT, P1, P2, P3, P4, P5>	{
public:
	typedef	Functor5wRetMethod<Callee,RT,P1,P2,P3,P4,P5>	FunctorObj;
	typedef	RT		(Callee::*Method) (P1, P2, P3, P4, P5,void* ClientData);
	Functor5wRetMethod(Callee& c, Method m, void* d)
		:	Functor5wRet<RT,P1,P2,P3,P4,P5>(d), _callee(c), _method(m)	{}
	virtual	RT		operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
		{	return (_callee.*_method) ( p1, p2, p3, p4, p5, this->_data );	}

private:
	Callee&			_callee;
	Method			_method;

	//	Block default copy operations
	Functor5wRetMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

#ifndef _NT
template <class Callee, class RT, class P1, class P2, class P3, class P4, class P5>
class Functor5wRetCMethod : public Functor5wRet<RT, P1, P2, P3, P4, P5>	{
public:
	typedef	Functor5wRetCMethod<Callee,RT,P1,P2,P3,P4,P5>	FunctorObj;
	typedef	RT		(Callee::*Method) (P1, P2, P3, P4, P5,void* ClientData) const;
	Functor5wRetCMethod(const Callee& c, Method m, void* d)
		:	Functor5wRet<RT,P1,P2,P3,P4,P5>(d), _callee(c), _method(m)	{}
	virtual	RT		operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
		{	return (_callee.*_method) ( p1, p2, p3, p4, p5, this->_data );	}

private:
	const Callee&	_callee;
	Method			_method;

	//	Block default copy operations
	Functor5wRetCMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};
#endif


//	The MakeFunctor5wRet function is Overloaded to create the right Functor object
template <class RT, class P1, class P2, class P3, class P4, class P5>
Functor5wRet<RT, P1, P2, P3, P4, P5>*
MakeFunctor5wRet(RT (*f)(P1, P2, P3, P4, P5, void*), void* d)
{
	return new Functor5wRetFunc<RT, P1, P2, P3, P4, P5>( f, d );
}

template <class Callee, class RT, class P1, class P2, class P3, class P4, class P5>
Functor5wRet<RT, P1, P2, P3, P4, P5>*
MakeFunctor5wRet(Callee& c, RT (Callee::*m)(P1,P2,P3,P4,P5,void*), void* d)
{
	return new Functor5wRetMethod<Callee,RT, P1, P2, P3, P4, P5 > ( c, m, d );
}

#ifndef _NT
template <class Callee, class RT, class P1, class P2, class P3, class P4, class P5>
Functor5wRet<RT, P1, P2, P3, P4, P5>*
MakeFunctor5wRet(const Callee& c, RT (Callee::*m)(P1,P2,P3,P4,P5,void*) const, void* d)
{
	return new Functor5wRetCMethod<Callee,RT,P1,P2,P3,P4,P5> ( c, m, d );
}
#endif

#endif // FUNCTOR5_H

//////////// File: Functor5.h //////////////////////////////////////////////////
