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
// EXAMPLES:	TestFunctors.cxx
// FILES:		Functor.h, Functor.cxx
// BUGS:
// SEE ALSO:
// TYPE:		LIBRARY-C++
//////////////////////////////////////////////////////////////////////////////////

#ifndef FUNCTOR_H
#define FUNCTOR_H

#include "libfunctor/Functor0.h"
#include "libfunctor/Functor1.h"
#include "libfunctor/Functor2.h"
#include "libfunctor/Functor3.h"
#include "libfunctor/Functor4.h"
#include "libfunctor/Functor5.h"
#include "libfunctor/Functor6.h"
#include "libfunctor/Functor7.h"


#endif // FUNCTOR_H

//////////// File: Functor.h ///////////////////////////////////////////////////
