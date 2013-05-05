////////////////////////////////////////////////////////////////////////////////
// NAME: 
//   Functor1 -
//
// SYNOPSIS:	Callback objects library - Functors with 1 parameter
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

#ifndef FUNCTOR1_H
#define FUNCTOR1_H

#include <libfunctor/FunctorBase.h>


//------------------------------------------------------------------------------
//	Callback functor with 1 argument without return value
//------------------------------------------------------------------------------

//	General Functor abstract class
template<class P1>
class Functor1  : public FunctorBase {
protected:
	typedef	Functor1<P1>							FunctorObj;
	Functor1()										{}
	Functor1(const FunctorObj& o) : FunctorBase(o)	{}
	Functor1(void* d) : FunctorBase(d)				{}
	FunctorObj&			operator = (FunctorObj& o);

public:
	virtual	~Functor1()								{}
	virtual	void		operator() (P1) const		{}

	//	Implement Reference counting interface
	static FunctorObj*	NewCounter(FunctorObj* obj)	{	return obj;	}
};


//	Functor for static function
template<class P1>
class Functor1Func : public Functor1<P1>	{
public:
	typedef	Functor1Func<P1>						FunctorObj;
	typedef	void	(*Func) (P1, void* ClientData);
	Functor1Func(Func f, void* d)
		:	Functor1<P1>(d), _func(f)				{}
	virtual	void	operator() (P1 p1) const
		{	_func(p1, this->_data);	}

private:
	Func			_func;

	//	Block default copy operations
	Functor1Func(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};


//	Functor for Object method
template <class Callee, class P1>
class Functor1Method : public Functor1<P1>	{
public:
	typedef	Functor1Method<Callee, P1>				FunctorObj;
	typedef	void	(Callee::*Method ) (P1, void* ClientData);
	Functor1Method(Callee& c, Method m, void* d)
		:	Functor1<P1>(d), _callee(c), _method(m)	{}
	virtual	void	operator() (P1 p1) const
		{	(_callee.*_method)(p1,this->_data);	}

private:
	Callee&			_callee;
	Method			_method;

	//	Block default copy operations
	Functor1Method(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

#ifndef _NT
template <class Callee, class P1>
class Functor1CMethod : public Functor1<P1>	{
public:
	typedef	Functor1CMethod<Callee, P1>				FunctorObj;
	typedef	void	(Callee::*Method ) (P1, void* ClientData) const;
	Functor1CMethod(const Callee& c, Method m, void* d)
		:	Functor1<P1>(d), _callee(c), _method(m)	{}
	virtual	void	operator() (P1 p1) const
		{	(_callee.*_method)(p1,this->_data);	}

private:
	const Callee&	_callee;
	Method			_method;

	//	Block default copy operations
	Functor1CMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};
#endif


//	The MakeFunctor1 function is Overloaded to create the right Functor object
template <class P1>
Functor1<P1>*
MakeFunctor1(void (*f)(P1, void* ClientData), void* d )
{
	return new Functor1Func<P1>( f, d );
}

template <class Callee, class P1>
Functor1<P1>*
MakeFunctor1(Callee& c, void (Callee::*m) (P1, void*), void* d )
{
 	return new Functor1Method<Callee, P1> ( c, m, d );
}

#ifndef _NT
template <class Callee, class P1>
Functor1<P1>*
MakeFunctor1(const Callee& c, void (Callee::*m) (P1, void*) const, void* d)
{
 	return new Functor1CMethod<Callee,P1> ( c, m, d );
}
#endif


//------------------------------------------------------------------------------
//	Callback functor with 1 argument with return value
//------------------------------------------------------------------------------

//	General Functor abstract class
template <class RT, class P1>
class Functor1wRet  : public FunctorBase {
protected:
	typedef	Functor1wRet<RT,P1>						FunctorObj;
	Functor1wRet()									{}
	Functor1wRet(const FunctorObj& o) : FunctorBase(o)	{}
	Functor1wRet(void* d) : FunctorBase(d)			{}
	FunctorObj&		operator = (FunctorObj& o);

public:
	virtual	~Functor1wRet()							{}
	virtual	RT		operator() (P1) const			{ return RT(); }

	//	Implement Reference counting interface
	static FunctorObj*	NewCounter(FunctorObj* obj)	{ return obj;	}
};

//	Functor for static function
template <class RT, class P1>
class Functor1wRetFunc : public Functor1wRet<RT, P1>	{
public:
	typedef	Functor1wRetFunc<RT,P1>					FunctorObj;
	typedef	RT		(*Func) (P1, void* ClientData);
	Functor1wRetFunc(Func f, void* d)
		:	Functor1wRet<RT, P1>(d), _func(f)		{}
	virtual	RT		operator() (P1 p1) const
		{	return _func(p1,this->_data);	}

private:
	Func			_func;

	//	Block default copy operations
	Functor1wRetFunc(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

//	Functor for Object method
template <class Callee, class RT, class P1>
class Functor1wRetMethod : public Functor1wRet<RT, P1>	{
public:
	typedef	Functor1wRetMethod<Callee,RT,P1>		FunctorObj;
	typedef	RT		(Callee::*Method) (P1, void* ClientData);
	Functor1wRetMethod(Callee& c, Method m, void* d)
		:	Functor1wRet<RT,P1>(d), _callee(c), _method(m)	{}
	virtual	RT		operator() (P1 p1) const
		{	return (_callee.*_method)(p1,this->_data);	}

private:
	Callee&			_callee;
	Method			_method;

	//	Block default copy operations
	Functor1wRetMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};

#ifndef _NT
template <class Callee, class RT, class P1>
class Functor1wRetCMethod : public Functor1wRet<RT, P1>	{
public:
	typedef	Functor1wRetCMethod<Callee,RT,P1>		FunctorObj;
	typedef	RT		(Callee::*Method) (P1, void* ClientData) const;
	Functor1wRetCMethod(const Callee& c, Method m, void* d)
		:	Functor1wRet<RT,P1>(d), _callee(c), _method(m)	{}
	virtual	RT		operator() (P1 p1) const
		{	return (_callee.*_method)(p1,this->_data);	}

private:
	const Callee&	_callee;
	Method			_method;

	//	Block default copy operations
	Functor1wRetCMethod(const FunctorObj& o);
	FunctorObj&		operator =(const FunctorObj& o);
};
#endif


//	The MakeFunctor1wRet function is Overloaded to create the right Functor object
template <class RT, class P1>
Functor1wRet<RT, P1>*
MakeFunctor1wRet(RT (*f)(P1,void*), void* d )
{
	return new Functor1wRetFunc<RT, P1>( f, d );
}

template <class Callee, class RT, class P1>
Functor1wRet<RT, P1>*
MakeFunctor1wRet(Callee& c, RT (Callee::*m)(P1,void*), void* d )
{
	return new Functor1wRetMethod<Callee, RT, P1> ( c, m, d );
}

#ifndef _NT
template <class Callee, class RT, class P1>
Functor1wRet<RT, P1>*
MakeFunctor1wRet(const Callee& c, RT (Callee::*m)(P1,void*) const, void* d)
{
	return new Functor1wRetCMethod<Callee, RT, P1> ( c, m, d );
}
#endif


#endif // FUNCTOR1_H

//////////// File: Functor1.h ///////////////////////////////////////////////////
