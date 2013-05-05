////////////////////////////////////////////////////////////////////////////////
// NAME: 
//   Functor7 -
//
// SYNOPSIS:	Callback objects library - Functors with 7 parameter
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

#ifndef FUNCTOR7_H
#define FUNCTOR7_H

#include <libfunctor/FunctorBase.h>


//------------------------------------------------------------------------------
//	Callback functor with 7 argument without return value
//------------------------------------------------------------------------------

//	General Functor abstract class
template<class P1,class P2,class P3,class P4,class P5,class P6,class P7>
class Functor7 : public FunctorBase {
protected:
	typedef	Functor7<P1,P2,P3,P4,P5,P6,P7>				FunctorObj;
	Functor7()											{}
	Functor7(const FunctorObj& o) : FunctorBase(o)		{}
	Functor7(void* d) : FunctorBase(d)					{}
	FunctorObj&		operator = (FunctorObj& o);

public:
	virtual	~Functor7()									{}
	virtual	void	operator() (P1,P2,P3,P4,P5,P6,P7) const	{}

	//	Implement Reference counting interface
	static FunctorObj*	NewCounter(FunctorObj* obj)		{ return obj; }
};

//	Functor for static function
template<class P1,class P2,class P3,class P4,class P5,class P6,class P7>
class Functor7Func : public Functor7<P1,P2,P3,P4,P5,P6,P7>	{
public:
	typedef	Functor7Func<P1,P2,P3,P4,P5,P6,P7>			FunctorObj;
	typedef	void	(*Func) (P1,P2,P3,P4,P5,P6,P7,void* ClientData);
	Functor7Func(Func f, void* d)
		:	Functor7<P1,P2,P3,P4,P5,P6,P7>(d), _func(f)		{}
	virtual	void	operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) const
		{	_func( p1, p2, p3, p4, p5, p6, p7, this->_data );	}

private:
	Func			_func;

	//	Block default copy operations
	Functor7Func(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

//	Functor for Object method
template <class Callee, class P1,class P2,class P3,class P4,class P5,class P6,class P7>
class Functor7Method : public Functor7<P1,P2,P3,P4,P5,P6,P7>	{
public:
	typedef	Functor7Method<Callee,P1,P2,P3,P4,P5,P6,P7>	FunctorObj;
	typedef	void	(Callee::*Method ) (P1,P2,P3,P4,P5,P6,P7,void* ClientData);
	Functor7Method(Callee& c, Method m, void* d)
		:	Functor7<P1,P2,P3,P4,P5,P6,P7>(d), _callee(c), _method(m)	{}
	virtual	void	operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) const
		{	(_callee.*_method) ( p1, p2, p3, p4, p5, p6, p7, this->_data );	}

private:
	Callee&			_callee;
	Method			_method;

	//	Block default copy operations
	Functor7Method(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

#ifndef _NT
template <class Callee, class P1,class P2,class P3,class P4,class P5,class P6,class P7>
class Functor7CMethod : public Functor7<P1,P2,P3,P4,P5,P6,P7>	{
public:
	typedef	Functor7CMethod<Callee,P1,P2,P3,P4,P5,P6,P7>	FunctorObj;
	typedef	void	(Callee::*Method ) (P1,P2,P3,P4,P5,P6,P7,void* ClientData) const;
	Functor7CMethod(const Callee& c, Method m, void* d)
		:	Functor7<P1,P2,P3,P4,P5,P6,P7>(d), _callee(c), _method(m)	{}
	virtual	void	operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) const
		{	(_callee.*_method) ( p1, p2, p3, p4, p5, p6, p7, this->_data );	}

private:
	const Callee&	_callee;
	Method			_method;

	//	Block default copy operations
	Functor7CMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};
#endif


//	The MakeFunctor7 function is Overloaded to create the right Functor object
template <class P1,class P2,class P3,class P4,class P5,class P6,class P7>
Functor7<P1,P2,P3,P4,P5,P6,P7>*
MakeFunctor7(void (*f)(P1,P2,P3,P4,P5,P6,P7,void*), void* d)
{
	return new Functor7Func<P1,P2,P3,P4,P5,P6,P7>( f, d );
}

template <class Callee, class P1,class P2,class P3,class P4,class P5,class P6,class P7>
Functor7<P1,P2,P3,P4,P5,P6,P7>*
MakeFunctor7(Callee& c, void (Callee::*m)(P1,P2,P3,P4,P5,P6,P7,void*), void* d)
{
 	return new Functor7Method<Callee,P1,P2,P3,P4,P5,P6,P7> ( c, m, d );
}
#ifndef _NT
template <class Callee, class P1,class P2,class P3,class P4,class P5,class P6,class P7>
Functor7<P1,P2,P3,P4,P5,P6,P7>*
MakeFunctor7(const Callee& c, void (Callee::*m)(P1,P2,P3,P4,P5,P6,P7,void*) const, void* d)
{
 	return new Functor7CMethod<Callee,P1,P2,P3,P4,P5,P6,P7> ( c, m, d );
}
#endif


//------------------------------------------------------------------------------
//	Callback functor with 7 argument with return value
//------------------------------------------------------------------------------

//	General Functor abstract class
template <class RT, class P1,class P2,class P3,class P4,class P5,class P6,class P7>
class Functor7wRet : public FunctorBase {
protected:
	typedef	Functor7wRet<RT,P1,P2,P3,P4,P5,P6,P7>		FunctorObj;
	Functor7wRet()										{}
	Functor7wRet(const FunctorObj& o) : FunctorBase(o)	{}
	Functor7wRet(void* d) : FunctorBase(d)				{}
	FunctorObj&		operator = (FunctorObj& o);

public:
	virtual	~Functor7wRet()								{}
	virtual	RT		operator() (P1,P2,P3,P4,P5,P6,P7) const	{ return RT(); }

	//	Implement Reference counting interface
	static FunctorObj*	NewCounter(FunctorObj* obj)		{ return obj; }
};

//	Functor for static function
template <class RT, class P1,class P2,class P3,class P4,class P5,class P6,class P7>
class Functor7wRetFunc : public Functor7wRet<RT,P1,P2,P3,P4,P5,P6,P7>	{
public:
	typedef	Functor7wRetFunc<RT,P1,P2,P3,P4,P5,P6,P7>	FunctorObj;
	typedef	RT		(*Func) (P1,P2,P3,P4,P5,P6,P7, void* ClientData);
	Functor7wRetFunc(Func f, void* d)
		:	Functor7wRet<RT,P1,P2,P3,P4,P5,P6,P7>(d), _func(f)	{}
	virtual	RT		operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) const
		{	return _func( p1, p2, p3, p4, p5, p6, p7, this->_data );	}

private:
	Func			_func;

	//	Block default copy operations
	Functor7wRetFunc(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

//	Functor for Object method
template <class Callee,class RT,class P1,class P2,class P3,class P4,class P5,class P6,class P7>
class Functor7wRetMethod : public Functor7wRet<RT,P1,P2,P3,P4,P5,P6,P7>	{
public:
	typedef	Functor7wRetMethod<Callee,RT,P1,P2,P3,P4,P5,P6,P7>	FunctorObj;
	typedef	RT		(Callee::*Method) (P1,P2,P3,P4,P5,P6,P7,void* ClientData);
	Functor7wRetMethod(Callee& c, Method m, void* d)
		:	Functor7wRet<RT,P1,P2,P3,P4,P5,P6,P7>(d), _callee(c), _method(m)	{}
	virtual	RT		operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) const
		{	return (_callee.*_method) ( p1, p2, p3, p4, p5, p6, p7, this->_data );	}

private:
	Callee&			_callee;
	Method			_method;

	//	Block default copy operations
	Functor7wRetMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

#ifndef _NT
template <class Callee,class RT,class P1,class P2,class P3,class P4,class P5,class P6,class P7>
class Functor7wRetCMethod : public Functor7wRet<RT,P1,P2,P3,P4,P5,P6,P7>	{
public:
	typedef	Functor7wRetCMethod<Callee,RT,P1,P2,P3,P4,P5,P6,P7>	FunctorObj;
	typedef	RT		(Callee::*Method) (P1,P2,P3,P4,P5,P6,P7,void* ClientData) const;
	Functor7wRetCMethod(const Callee& c, Method m, void* d)
		:	Functor7wRet<RT,P1,P2,P3,P4,P5,P6,P7>(d), _callee(c), _method(m)	{}
	virtual	RT		operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) const
		{	return (_callee.*_method) ( p1, p2, p3, p4, p5, p6, p7, this->_data );	}

private:
	const Callee&	_callee;
	Method			_method;

	//	Block default copy operations
	Functor7wRetCMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};
#endif


//	The MakeFunctor7wRet function is Overloaded to create the right Functor object
template <class RT, class P1,class P2,class P3,class P4,class P5,class P6,class P7>
Functor7wRet<RT, P1,P2,P3,P4,P5,P6,P7>*
MakeFunctor7wRet(RT (*f)(P1,P2,P3,P4,P5,P6,P7, void*), void* d)
{
	return new Functor7wRetFunc<RT,P1,P2,P3,P4,P5,P6,P7>( f, d );
}

template <class Callee,class RT,class P1,class P2,class P3,class P4,class P5,class P6,class P7>
Functor7wRet<RT,P1,P2,P3,P4,P5,P6,P7>*
MakeFunctor7wRet(Callee& c, RT (Callee::*m)(P1,P2,P3,P4,P5,P6,P7,void*), void* d)
{
	return new Functor7wRetMethod<Callee,RT,P1,P2,P3,P4,P5,P6,P7> ( c, m, d );
}
#ifndef _NT
template <class Callee,class RT,class P1,class P2,class P3,class P4,class P5,class P6,class P7>
Functor7wRet<RT,P1,P2,P3,P4,P5,P6,P7>*
MakeFunctor7wRet(const Callee& c, RT (Callee::*m)(P1,P2,P3,P4,P5,P6,P7,void*) const, void* d)
{
	return new Functor7wRetCMethod<Callee,RT,P1,P2,P3,P4,P5,P6,P7> ( c, m, d );
}
#endif

#endif // FUNCTOR7_H

//////////// File: Functor7.h //////////////////////////////////////////////////
