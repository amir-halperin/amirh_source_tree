////////////////////////////////////////////////////////////////////////////////
// NAME: 
//   Functor2 -
//
// SYNOPSIS:	Callback objects library - Functors with 2 parameter
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
//////////////////////////////////////////////////////////////////////////////////

#ifndef FUNCTOR2_H
#define FUNCTOR2_H

#include <libfunctor/FunctorBase.h>


//------------------------------------------------------------------------------
//	Callback functor with 2 argument without return value
//------------------------------------------------------------------------------

//	General Functor abstract class
template<class P1, class P2>
class Functor2 : public FunctorBase {
protected:
	typedef	Functor2<P1,P2>			FunctorObj;
	Functor2()										{}
	Functor2(const FunctorObj& o) : FunctorBase(o)	{}
	Functor2(void* d) : FunctorBase(d)				{}
	FunctorObj&		operator = (FunctorObj& o);

public:
	virtual	~Functor2()								{}
	virtual	void	operator() (P1, P2) const		{}

	//	Implement Reference counting interface
	static FunctorObj*	NewCounter(FunctorObj* obj)	{ return obj; }
};

//	Functor for static function
template<class P1, class P2>
class Functor2Func : public Functor2<P1, P2>	{
public:
	typedef	Functor2Func<P1,P2>				FunctorObj;
	typedef	void	(*Func) (P1, P2, void* ClientData);
	Functor2Func(Func f, void* d)
		:	Functor2<P1,P2>(d), _func(f)			{}
	virtual	void	operator() (P1 p1, P2 p2) const
		{	_func( p1,p2,this->_data );	}

private:
	Func			_func;

	//	Block default copy operations
	Functor2Func(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

//	Functor for Object method
template <class Callee, class P1, class P2>
class Functor2Method : public Functor2<P1, P2>	{
public:
	typedef	Functor2Method<Callee,P1,P2>	FunctorObj;
	typedef	void	(Callee::*Method ) (P1, P2, void* ClientData);
	Functor2Method(Callee& c, Method m, void* d)
		:	Functor2<P1,P2>(d), _callee(c), _method(m)	{}
	virtual	void	operator() (P1 p1, P2 p2) const
		{	(_callee.*_method)(p1,p2,this->_data);	}

private:
	Callee&			_callee;
	Method			_method;

	//	Block default copy operations
	Functor2Method(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

#ifndef _NT
template <class Callee, class P1, class P2>
class Functor2CMethod : public Functor2<P1, P2>	{
public:
	typedef	Functor2CMethod<Callee,P1,P2>	FunctorObj;
	typedef	void	(Callee::*Method ) (P1, P2, void* ClientData) const;
	Functor2CMethod(const Callee& c, Method m, void* d)
		:	Functor2<P1,P2>(d), _callee(c), _method(m)	{}
	virtual	void	operator() (P1 p1, P2 p2) const
		{	(_callee.*_method)(p1,p2,this->_data);	}

private:
	const Callee&	_callee;
	Method			_method;

	//	Block default copy operations
	Functor2CMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};
#endif


//	The MakeFunctor2 function is Overloaded to create the right Functor object
template <class P1, class P2>
Functor2<P1, P2>*
MakeFunctor2(void (*f)(P1,P2,void*), void* d)
{
	return new Functor2Func<P1, P2>( f, d );
}

template <class Callee, class P1, class P2>
Functor2<P1, P2>*
MakeFunctor2(Callee& c, void (Callee::*m) (P1,P2,void*), void* d)
{
 	return new Functor2Method<Callee, P1, P2> ( c, m, d );
}

#ifndef _NT
template <class Callee, class P1, class P2>
Functor2<P1, P2>*
MakeFunctor2(const Callee& c, void (Callee::*m) (P1,P2,void*) const, void* d)
{
 	return new Functor2CMethod<Callee, P1, P2> ( c, m, d );
}
#endif


//--------------------------------------------------------------------------------
//	Callback functor with 2 argument with return value
//--------------------------------------------------------------------------------
//	General Functor abstract class
template <class RT, class P1, class P2>
class Functor2wRet : public FunctorBase {
protected:
	typedef	Functor2wRet<RT,P1,P2>					FunctorObj;
	Functor2wRet()									{}
	Functor2wRet(const FunctorObj& o) : FunctorBase(o)	{}
	Functor2wRet(void* d) : FunctorBase(d)			{}
	FunctorObj&		operator = (FunctorObj& o);

public:
	virtual	~Functor2wRet()							{}
	virtual	RT		operator() (P1, P2) const		{ return RT(); }

	//	Implement Reference counting interface
	static FunctorObj*	NewCounter(FunctorObj* obj)	{ return obj; }
};

//	Functor for static function
template <class RT, class P1, class P2>
class Functor2wRetFunc : public Functor2wRet<RT, P1, P2>	{
public:
	typedef	Functor2wRetFunc<RT,P1,P2>				FunctorObj;
	typedef	RT		(*Func) (P1, P2, void* ClientData);
	Functor2wRetFunc(Func f, void* d)
		:	Functor2wRet<RT,P1,P2>(d), _func(f)		{}
	virtual	RT		operator() (P1 p1, P2 p2) const
		{	return _func(p1,p2,this->_data);	}

private:
	Func			_func;

	//	Block default copy operations
	Functor2wRetFunc(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

//	Functor for Object method
template <class Callee, class RT, class P1, class P2>
class Functor2wRetMethod : public Functor2wRet<RT, P1, P2>	{
public:
	typedef	Functor2wRetMethod<Callee,RT,P1,P2>		FunctorObj;
	typedef	RT		(Callee::*Method) (P1, P2, void* ClientData);
	Functor2wRetMethod(Callee& c, Method m, void* d)
		:	Functor2wRet<RT,P1,P2>(d), _callee(c), _method(m)	{}
	virtual	RT		operator() (P1 p1, P2 p2) const
		{	return (_callee.*_method)(p1, p2, this->_data);	}

private:
	Callee&			_callee;
	Method			_method;

	//	Block default copy operations
	Functor2wRetMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

#ifndef _NT
template <class Callee, class RT, class P1, class P2>
class Functor2wRetCMethod : public Functor2wRet<RT, P1, P2>	{
public:
	typedef	Functor2wRetCMethod<Callee,RT,P1,P2>	FunctorObj;
	typedef	RT		(Callee::*Method) (P1, P2, void* ClientData) const;
	Functor2wRetCMethod(const Callee& c, Method m, void* d)
		:	Functor2wRet<RT,P1,P2>(d), _callee(c), _method(m)	{}
	virtual	RT		operator() (P1 p1, P2 p2) const
		{	return (_callee.*_method)(p1,p2,this->_data);	}

private:
	const Callee&	_callee;
	Method			_method;

	//	Block default copy operations
	Functor2wRetCMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};
#endif


//	The MakeFunctor2wRet function is Overloaded to create the right Functor object
template <class RT, class P1, class P2>
Functor2wRet<RT, P1, P2>*
MakeFunctor2wRet(RT (*f)(P1, P2, void*), void* d)
{
	return new Functor2wRetFunc<RT, P1, P2>( f, d );
}

template <class Callee, class RT, class P1, class P2>
Functor2wRet<RT, P1, P2>*
MakeFunctor2wRet(Callee& c, RT (Callee::*m)(P1,P2,void*), void* d)
{
	return new Functor2wRetMethod<Callee, RT, P1, P2> ( c, m, d );
}

#ifndef _NT
template <class Callee, class RT, class P1, class P2>
Functor2wRet<RT, P1, P2>*
MakeFunctor2wRet(const Callee& c, RT (Callee::*m)(P1,P2,void*) const, void* d)
{
	return new Functor2wRetCMethod<Callee,RT,P1,P2> ( c, m, d );
}
#endif

#endif // FUNCTOR2_H

//////////// File: Functor2.h ///////////////////////////////////////////////////
