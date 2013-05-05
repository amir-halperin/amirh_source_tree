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

#include <iostream.h>


//--------------------------------------------------------------------------------
//      A primitive int class
//--------------------------------------------------------------------------------
class	Int {
public:
	Int() : val( 0 )							{}
	Int( int i ) : val( i )						{}
	Int( const Int& i ) : val( i.val )			{}

	operator int() const						{	return val; }

private:
	int					val;
};

ostream& operator << (ostream&str,const Int&i)	{	str << (int)i; return str;	}
istream& operator >> (istream& str, Int& i)		{	int _i; str >> _i;	i=_i; return str;	}

//--------------------------------------------------------------------------------
//      A primitive char class
//--------------------------------------------------------------------------------
class	Char {
public:
	Char() : val( 0 )							{}
	Char( char i ) : val( i )					{}
	Char( const Char& i ) : val( i.val )		{}

	operator char() const						{	return val; }

private:
	char					val;
};

ostream& operator << (ostream&str,const Char&c)	{	str << (char)c; return str;	}
istream& operator >> (istream& str,Char& c)		{	char _c; str >> _c;	c=_c; return str;	}


//--------------------------------------------------------------------------------
//      Poly Locked reference test
//--------------------------------------------------------------------------------
class B
{
public:
	virtual void Who()		{ cout << "I'm the Base Class" << endl ; }
	virtual B* clone()		{ return new B(*this) ; }
	operator int() const	{ return 1; }		//	Support comparison op.
};

class D : public B
{
public: 
	virtual void Who()		{ cout << "I'm the Derived Class" << endl ; }
	virtual B* clone()		{ return new D(*this) ; }
	operator int() const	{ return 2; }		//	Support comparison op.
};


//////////// File: RCObjects.h ///////////////////////////////////////////////////
