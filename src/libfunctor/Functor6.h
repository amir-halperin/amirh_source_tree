////////////////////////////////////////////////////////////////////////////////
// NAME: 
//   Functor6 -
//
// SYNOPSIS:	Callback objects library - Functors with 6 parameter
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

#ifndef FUNCTOR6_H
#define FUNCTOR6_H

#include <libfunctor/FunctorBase.h>

//------------------------------------------------------------------------------
//	Callback functor with 6 argument without return value
//------------------------------------------------------------------------------

//	General Functor abstract class
template<class P1,class P2,class P3,class P4,class P5,class P6>
class Functor6 : public FunctorBase {
protected:
	typedef	Functor6<P1,P2,P3,P4,P5,P6>					FunctorObj;
	Functor6()											{}
	Functor6(const FunctorObj& o) : FunctorBase(o)		{}
	Functor6(void* d) : FunctorBase(d)					{}
	FunctorObj&		operator = (FunctorObj& o);

public:
	virtual	~Functor6()									{}
	virtual	void	operator()(P1,P2,P3,P4,P5,P6) const	{}

	//	Implement Reference counting interface
	static FunctorObj*	NewCounter(FunctorObj* obj)		{ return obj; }
};

//	Functor for static function
template<class P1,class P2,class P3,class P4,class P5,class P6>
class Functor6Func : public Functor6<P1,P2,P3,P4,P5,P6>	{
public:
	typedef	Functor6Func<P1,P2,P3,P4,P5,P6>				FunctorObj;
	typedef	void	(*Func) (P1,P2,P3,P4,P5,P6,void* ClientData);
	Functor6Func(Func f, void* d)
		:	Functor6<P1,P2,P3,P4,P5,P6>(d), _func(f)	{}
	virtual	void	operator() (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6) const
		{	_func( p1, p2, p3, p4, p5, p6, this->_data );	}

private:
	Func			_func;

	//	Block default copy operations
	Functor6Func(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};


//	Functor for Object method
template <class Callee, class P1,class P2,class P3,class P4,class P5,class P6>
class Functor6Method : public Functor6<P1,P2,P3,P4,P5,P6>	{
public:
	typedef	Functor6Method<Callee,P1,P2,P3,P4,P5,P6>	FunctorObj;
	typedef	void	(Callee::*Method ) (P1,P2,P3,P4,P5,P6,void* ClientData);
	Functor6Method(Callee& c, Method m, void* d)
		:	Functor6<P1,P2,P3,P4,P5,P6>(d), _callee(c), _method(m)	{}
	virtual	void	operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) const
		{	(_callee.*_method) ( p1, p2, p3, p4, p5, p6, this->_data );	}

private:
	Callee&			_callee;
	Method			_method;

	//	Block default copy operations
	Functor6Method(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

#ifndef _NT
template <class Callee, class P1,class P2,class P3,class P4,class P5,class P6>
class Functor6CMethod : public Functor6<P1,P2,P3,P4,P5,P6>	{
public:
	typedef	Functor6CMethod<Callee,P1,P2,P3,P4,P5,P6>	FunctorObj;
	typedef	void	(Callee::*Method ) (P1,P2,P3,P4,P5,P6,void* ClientData) const;
	Functor6CMethod(const Callee& c, Method m, void* d)
		:	Functor6<P1,P2,P3,P4,P5,P6>(d), _callee(c), _method(m)	{}
	virtual	void	operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) const
		{	(_callee.*_method) ( p1, p2, p3, p4, p5, p6, this->_data );	}

private:
	const Callee&	_callee;
	Method			_method;

	//	Block default copy operations
	Functor6CMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};
#endif


//	The MakeFunctor6 function is Overloaded to create the right Functor object
template <class P1,class P2,class P3,class P4,class P5,class P6>
Functor6<P1,P2,P3,P4,P5,P6>*
MakeFunctor6(void (*f)(P1,P2,P3,P4,P5,P6, void*), void* d)
{
	return new Functor6Func<P1,P2,P3,P4,P5,P6>( f, d );
}

template <class Callee, class P1,class P2,class P3,class P4,class P5,class P6>
Functor6<P1,P2,P3,P4,P5,P6>*
MakeFunctor6(Callee& c, void (Callee::*m)(P1,P2,P3,P4,P5,P6,void*), void* d)
{
 	return new Functor6Method<Callee,P1,P2,P3,P4,P5,P6> ( c, m, d );
}
#ifndef _NT
template <class Callee, class P1,class P2,class P3,class P4,class P5,class P6>
Functor6<P1,P2,P3,P4,P5,P6>*
MakeFunctor6(const Callee& c, void (Callee::*m)(P1,P2,P3,P4,P5,P6,void*) const, void* d)
{
 	return new Functor6CMethod<Callee,P1,P2,P3,P4,P5,P6> ( c, m, d );
}
#endif


//------------------------------------------------------------------------------
//	Callback functor with 6 argument with return value
//------------------------------------------------------------------------------

//	General Functor abstract class
template <class RT, class P1,class P2,class P3,class P4,class P5,class P6>
class Functor6wRet : public FunctorBase {
protected:
	typedef	Functor6wRet<RT,P1,P2,P3,P4,P5,P6>			FunctorObj;
	Functor6wRet()										{}
	Functor6wRet(const FunctorObj& o) : FunctorBase(o)	{}
	Functor6wRet(void* d) : FunctorBase(d)				{}
	FunctorObj&		operator = (FunctorObj& o);

public:
	virtual	~Functor6wRet()								{}
	virtual	RT	operator() (P1,P2,P3,P4,P5,P6) const	{ return RT(); }

	//	Implement Reference counting interface
	static FunctorObj*	NewCounter(FunctorObj* obj)		{ return obj; }
};

//	Functor for static function
template <class RT, class P1,class P2,class P3,class P4,class P5,class P6>
class Functor6wRetFunc : public Functor6wRet<RT,P1,P2,P3,P4,P5,P6>	{
public:
	typedef	Functor6wRetFunc<RT,P1,P2,P3,P4,P5,P6>		FunctorObj;
	typedef	RT		(*Func) (P1,P2,P3,P4,P5,P6,void* ClientData);
	Functor6wRetFunc(Func f, void* d)
		:	Functor6wRet<RT,P1,P2,P3,P4,P5,P6>(d), _func( f )	{}
	virtual	RT	operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) const
		{	return _func( p1, p2, p3, p4, p5, p6, this->_data );	}

private:
	Func			_func;

	//	Block default copy operations
	Functor6wRetFunc(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

//	Functor for Object method
template <class Callee, class RT, class P1,class P2,class P3,class P4,class P5,class P6>
class Functor6wRetMethod : public Functor6wRet<RT,P1,P2,P3,P4,P5,P6>	{
public:
	typedef	Functor6wRetMethod<Callee,RT,P1,P2,P3,P4,P5,P6>	FunctorObj;
	typedef	RT		(Callee::*Method) (P1,P2,P3,P4,P5,P6,void* ClientData);
	Functor6wRetMethod(Callee& c, Method m, void* d)
		:	Functor6wRet<RT,P1,P2,P3,P4,P5,P6>(d), _callee(c), _method(m)	{}
	virtual	RT		operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) const
		{return (_callee.*_method) ( p1, p2, p3, p4, p5, p6, this->_data );	}

private:
	Callee&			_callee;
	Method			_method;

	//	Block default copy operations
	Functor6wRetMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

#ifndef _NT
template <class Callee, class RT, class P1,class P2,class P3,class P4,class P5,class P6>
class Functor6wRetCMethod : public Functor6wRet<RT,P1,P2,P3,P4,P5,P6>	{
public:
	typedef	Functor6wRetCMethod<Callee,RT,P1,P2,P3,P4,P5,P6>	FunctorObj;
	typedef	RT		(Callee::*Method) (P1,P2,P3,P4,P5,P6,void* ClientData) const;
	Functor6wRetCMethod(const Callee& c, Method m, void* d)
		:	Functor6wRet<RT,P1,P2,P3,P4,P5,P6>(d), _callee(c), _method(m)	{}
	virtual	RT		operator() (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) const
		{	return (_callee.*_method) ( p1, p2, p3, p4, p5, p6, this->_data );	}

private:
	const Callee&	_callee;
	Method			_method;

	//	Block default copy operations
	Functor6wRetCMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};
#endif


//	The MakeFunctor6wRet function is Overloaded to create the right Functor object
template <class RT, class P1,class P2,class P3,class P4,class P5,class P6>
Functor6wRet<RT, P1,P2,P3,P4,P5,P6>*
MakeFunctor6wRet(RT (*f)(P1,P2,P3,P4,P5,P6,void*), void* d)
{
	return new Functor6wRetFunc<RT,P1,P2,P3,P4,P5,P6>( f, d );
}

template <class Callee, class RT, class P1,class P2,class P3,class P4,class P5,class P6>
Functor6wRet<RT, P1,P2,P3,P4,P5,P6>*
MakeFunctor6wRet(Callee& c, RT (Callee::*m)(P1,P2,P3,P4,P5,P6,void*), void* d)
{
	return new Functor6wRetMethod<Callee,RT,P1,P2,P3,P4,P5,P6 > ( c, m, d );
}

#ifndef _NT
template <class Callee, class RT, class P1,class P2,class P3,class P4,class P5,class P6>
Functor6wRet<RT, P1,P2,P3,P4,P5,P6>*
MakeFunctor6wRet(const Callee& c, RT (Callee::*m)(P1,P2,P3,P4,P5,P6,void*) const, void* d)
{
	return new Functor6wRetCMethod<Callee,RT,P1,P2,P3,P4,P5,P6> ( c, m, d );
}
#endif

#endif // FUNCTOR6_H

//////////// File: Functor6.h //////////////////////////////////////////////////
