//////////////////////////////////////////////////////////////////////////////////
// NAME: 
//   Functor -
//
// SYNOPSIS:	Callback objects library
// DESCRIPTION:	Functor objects encapsulating object method or static function.
//				A diffrent class is provided for each number of arguments with or
//				without a return value.
//				Client function/method also receive a typeless client data.
//				Callback objects are created using:
//				MakeFunctor##(object, method, client data)
//				where ## stands for argument & return value (e.g. MakeFunctor1wRet)
//				Classes are named:	
//				Functor# (abstract base), Functor#Func (derived function ptr class),
//				Functor#Method (derived method ptr class).
// EXAMPLES:	TestFunctors.cxx
// FILES:		Functor.h, Functor.cxx
// BUGS:
// SEE ALSO:
// TYPE:		LIBRARY-C++
//////////////////////////////////////////////////////////////////////////////////


//--------------------------------------------------------------------------------
//	Library is compeletely inline. Dummy function until libfunctor.a removed.
//--------------------------------------------------------------------------------
void FunctorDummy()
{
}


//////////// File: Functor.cpp ///////////////////////////////////////////////
