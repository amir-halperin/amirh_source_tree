//////////////////////////////////////////////////////////////////////////////////
// NAME: 
//   RCTest -
//
// SYNOPSIS:	Reference count test program
// DESCRIPTION:	
// EXAMPLES:	
// FILES:		RCTest.cpp Reference.h
// BUGS:
// SEE ALSO:
// TYPE:		PROGRAM
//////////////////////////////////////////////////////////////////////////////////

#include <librefcount/Reference.h>
#include <librefcount/RCObjects.h>
#include <iostream.h>
#include <fstream.h>
#include <ctype.h>
#include <stdlib.h>


typedef		const char*	Except;
Except		ex = "Stupid exception";


//--------------------------------------------------------------------------------
//	Printing operator template for the various References
//--------------------------------------------------------------------------------
template< class T, class Counter >
ostream&	print (ostream& o, const char* name, const ConstRef< T, Counter >& ref)
{
	o << name << "  [" << ref.Count() << " @ ";
	if( ref.Empty() )		o << "NULL]" << endl;
	else					o << (void*) &(*ref) << "] " << *ref << endl;
	return o;
}

typedef	ConstRef< Int, RefCount< Int > >		rcInt;
typedef	ConstRef1< Int >						rcInt1;
ostream&	operator << (ostream& o, const rcInt& ref)
{
	return print( o, "ConstRef ", ref);
}

typedef	LockedRef< Int, RefCount< Int > >		lInt;
typedef	LockedRef1< Int >						lInt1;
ostream&	operator << (ostream& o, const lInt& ref )
{
	return print( o, "LockedRef", ref);
}

typedef	SharedRef< Int, RefCount< Int > >		shInt;
typedef	SharedRef1< Int >						shInt1;
ostream&	operator << (ostream& o, const shInt& ref )
{
	return print( o, "SharedRef", ref);
}

//--------------------------------------------------------------------------------
//      Reference count test
//--------------------------------------------------------------------------------
rcInt	Dive( rcInt val, int deeper )
{
	if( deeper )
		return Dive( val, deeper-1 );
	else
		return ( new Int( val.Count() ) );
}

void	TestRC()
{
	cout << endl << "Testing Reference counting." << endl;
	int		count = 0;

	try {
		rcInt	val( new Int( 8 ) ), Nil;
		{
			rcInt	a, Nil;
			a = val;
			val = Dive( val, 42 );
			rcInt1	b( a );
			count = *val;
			a = b = Nil;
		}
		cout << count << " Int references were created." << endl
			 << val.Count()  << " are still left." << endl;
		cout << *Nil << endl;		//	BAD: Don't dereference a NULL ref!!
		throw ex;
	} catch( Except msg ) { cout << "Caught " << msg << endl; }
}

//--------------------------------------------------------------------------------
//      Locked reference test
//--------------------------------------------------------------------------------
void	TestLock()
{
	cout << endl << "Testing Locked Reference." << endl;

	try {
		lInt	a( new Int( 42 ) );
		rcInt	b( a ), f;
		lInt1	c( b ), d, x, y, Nil;
		{ d = x = b; }				//	Assign to Null Ref
		x = Nil;					//	Assign a Null Ref
		lInt	e( new Int( 23 ) );
		f = new Int( *b + e.Get() );
		*a = 18;					//	Change a and see if any other object changed
		d = e;
		int		i = e.Get();		//	Check effects of read only access
		cout << "a: " << a << "b: " << b << "c: " << c
			 << "d: " << d << "e: " << e << "f: " << f << "Nil: " << Nil;
		cout << *Nil << endl;		//	BAD: Don't dereference a NULL ref!!
		throw ex;
	} catch( Except msg ) { cout << "Caught " << msg << endl; }
}

//--------------------------------------------------------------------------------
//      Locked reference test
//--------------------------------------------------------------------------------
void	TestShare()
{
	cout << endl << "Testing Shared References." << endl;

	try {
		rcInt	a( new Int( 42 ) );
		shInt1	b( a ), Nil;
		shInt	c, x;
		c = b;
		x = a;						//	Assign to Null Ref
		x = Nil;					//	Assign a Null Ref
		rcInt	d( b );
		shInt	e( new Int( 23 ) );
		*b = 18;					//	Change a and see if any other object changed
		c = e;
		cout << "a: " << a << "b: " << b << "c: " << c
			 << "d: " << d << "e: " << e << "Nil: " << Nil;
		*Nil = 120;					//	BAD!!
		throw ex;
	} catch( Except msg ) { cout << "Caught " << msg << endl; }
}

//--------------------------------------------------------------------------------
//      Stream IO test
//--------------------------------------------------------------------------------
void	TestIO()
{
	cout << endl << "Testing Reference Stream IO." << endl;

	typedef	ConstRef< Char, RefCount< Char > >		rcChar;
	typedef	LockedRef1< Char >						lChar;
	typedef	SharedRef1< Char >						shChar;
	rcChar	a( new Char( '!' ) );
	lChar	b( a );
	shChar	c( a );
	cout << "Writing:" << endl;
	print(cout, "a: Const  ", a);
	print(cout, "b: Locked ", b);
	print(cout, "c: Shared ", c);
	{
		ofstream	f("RCTest.dat");
		f << a << endl << b << endl << c << endl;
	}
	cout << "Reading:" << endl;
	{
		ifstream	f("RCTest.dat");
		f >> a >> b >> c;
	}
	print(cout, "a: Const  ", a);
	print(cout, "b: Locked ", b);
	print(cout, "c: Shared ", c);
}

//--------------------------------------------------------------------------------
//      Locked reference test
//--------------------------------------------------------------------------------
typedef	RefwCmp< ConstRef<Int, RefCountwCmp<Int> > >		ccInt;
typedef	RefwCmp< LockedRef<Int, RefCountwCmp<Int> > >		lcInt;

void	TestComparison()
{
	cout << "Testing Int reference comparison operators." << endl;
	ccInt			i1, i2, i3( new Int(42) ), i4( new Int(7) ); 

	cout << "Two NULLs are " << ( i1 == i2? "" : "NOT ") << "equal." << endl;
	cout << "NULL is" << ( i1 == i3? "!" : " not") << " equal to " << *i3 << endl;
	cout << "NULL is" << ( i1 != i3? " not" : "!") << " equal to " << *i3 << endl;
	cout << *i3 << " is" << ( i1 == i3? "!" : " not") << " equal to NULL" << endl;
	cout << *i3 << " is " << ( i3 == i3? "" : "NOT! ") << "equal to itself."  << endl;
	cout << "NULL is " << ( i1 < i3? "" : "NOT! ") << "smaller then " << *i3 << endl;
	cout << *i3 << " is" << ( i3 < i1? "! " : " not") << " smaller then NULL"  << endl;
	cout << *i4 << " is " << ( i4 < i3? "" : "NOT! ") << "smaller then " << *i3 << endl;
	cout << *i3 << " is " << ( i3 > i1? "" : "NOT! ") << "bigger then NULL"  << endl;
	cout << *i4 << " is" << ( i4 > i3? "!" : " not") << " bigger then " << *i3 << endl << endl;
}

//--------------------------------------------------------------------------------
//      Poly Locked reference test
//--------------------------------------------------------------------------------
typedef ConstRef< B, RefCount<B> >		BConstRef ;
typedef LockedRef< B, RefCount<B> >		BLockedRef ;
typedef PolyLockedRef< B, RefCount<B> >	BPolyRef ;
typedef LockedRef1< B >					BLockedRef1 ;
typedef PolyLockedRef1< B >				BPolyRef1 ;

void TestPoly()
{
	BConstRef	a( new D );
	cout << "Here is how locked-ref fails to copy derived class: " << endl ;
	cout << "b1 is initialize with Derived class No. 1" << endl ;
	BLockedRef	b1( a ) ;
	BPolyRef1	b4( a ), b5 ;
	b5 = a;

	cout << "b2 is a LockedRef copy of b1, but he says ..." << endl ;
	BLockedRef	b2( b1 ) ;
	(*b2).Who() ;

	cout << "b3 is a PolyLockedRef copy of b1, and he says ..." << endl ;
	BPolyRef	b3( b1 ) ;
	(*b3).Who() ;
}
  


//--------------------------------------------------------------------------------
//      Main program
//--------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	char	c;
	while ((c = getopt(argc, argv, "cCiIlLpPrRsS")) != EndOfOpt)
	{
		switch( toupper(c) )
		{
		case 'C':
			TestComparison();
			break;
		case 'I':
			TestIO();
			break;
		case 'R':
			TestRC();
			break;
		case 'L':
			TestLock();
			break;
		case 'S':
			TestShare();
			break;
		case 'P':
			TestPoly() ;
			break ;
		default:
			cout << "Usage: RCTest -C[omparison] -L[ockedRef] -P[olymorphic] "
					"-R[efCount] -S[haredRef]." << endl;
		break;
		}
	}
	return 0;
}

//////////// File: RCTest.cpp ////////////////////////////////////////////////////
