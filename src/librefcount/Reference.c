////////////////////////////////////////////////////////////////////////////////
// NAME: 
//   Reference -
//
// SYNOPSIS:	Non-obstrusive Reference counted pointer class template.
// DESCRIPTION:	'ConstRef' is a reference counting pointer to a CONST shared object.
//				It allows sharing objects and leaving exception without deleting objects.
//				Use op. -> to access (const) methods or op. * to access the T&.
//
//				Clients can hold the reference and pass it (by value) around.
//				When last reference is disconnected referenced object is deleted.
//				Uses a RefCount internal class to handle counting and hold the object.
//
// 				'LockedRef' is a reference to a PRIVATE copy that can be modified.
//				Hold LockedRef only when you need to change the object.
//				NOTE:	When you create a LockedRef from ConstRef and change it, the
//						original ConstRef does NOT see the modified value.
//						Pass LockedRef1& to functions to see the modified object.
//						Use the Get() method for read-only operations!!
//
// 				'SharedRef' is a reference to a Shared modifiable copy
//				Changes in one copy will affect other copies!!
//				Do NOT create a ConstRef from a SharedRef!!!
// NOTE:		Pass LockedRef1& to functions to see modified object
//
// EXAMPLES:	See RCTest.cpp
//				typedef	ConstRef1< T >			rcT;
//				rcT		t1( new T );
//				t1->aConstTMethod(...);
//
//				typedef	LockedRef1< T >	lT;
//				lT		t2( t1 );
//				t2->anyTMethod(...);		//	t1 is not affected by the changes
//
//				typedef	SharedRef1< T >	shT;
//				shT		t3( t1 ), t4( t1 );
//				rcT		t5( t4 );			//	DON'T do this!! 
//				t4->anyTMethod(...);		//	t3 (and t5!) affected by the changes
//
// FILES:		Reference.h
// BUGS:
// SEE ALSO:
// TYPE:		LIBRARY-C++
////////////////////////////////////////////////////////////////////////////////

#include "Reference.h"

#include "rw/epersist.h"
// #include "streamer/list.h"


RWDEFINE_PERSISTABLE_TEMPLATE_2(ConstRef)

//------------------------------------------------------------------------------
//  Access object reference
//------------------------------------------------------------------------------
template< class T, class RefCounter >
T& ConstRef<T,RefCounter> :: GetObject() const
{
	static	char	msg[] = "***	Referenced NULL ConstRef!	***";
	if( !_object )									//	Check for NULL Reference
	{
		cerr << endl << endl << msg << endl << endl;
		throw "Empty Ref dereferenced!";
	}
	return *_object;
}


template< class T, class RefCounter >
void ConstRef<T,RefCounter> :: Set( RefCounter* o )
{
	Disconnect();
	_object = o;
	if( _object )
		_object->Connect();
}

template< class T, class RefCounter >				//	Assignment operators
ConstRef<T,RefCounter>& ConstRef<T,RefCounter> :: operator = ( const ConstRefObj& o )	// @2
{
	if( this != &o )
		Set( o._object );
	return *this;
}

//------------------------------------------------------------------------------
//	Object Streaming support
//	NOTE:	Must handle explicitly Nil Pointers!
//------------------------------------------------------------------------------
template <class T, class C, class S>
void rwSaveGuts(S& str, const ConstRef<T,C>& r)
{
	const T*	t = r;
	char		nil = (t == 0);
	str << nil;
	if( !nil )
		str << *t;
}

template <class T, class C, class S>
void rwRestoreGuts(S& str, ConstRef<T,C>& r)
{
	T*			t = 0;
	char		nil = True;
	str >> nil;
	if( !nil )
		str >> t;
	r = t; 
}

//------------------------------------------------------------------------------
//	Stream operator i/o
//------------------------------------------------------------------------------
template< class T, class RefCounter >
ostream& operator << (ostream& s, const ConstRef<T,RefCounter> & obj)
{
	if( !obj.Empty() )
		s << *obj;
	return s;
}
template< class T, class RefCounter >
istream& operator >> (istream& s, ConstRef<T,RefCounter>& obj)
{
	T	t;
	s >> t;
	obj = new T( t );
	return s;
}


template< class T, class RefCounter >
ConstRef<T,RefCounter> :: ConstRef( const T* ptr )	:	_object(0)
{
	T*		t =  (T*) ptr;							//	AIX does not support const_cast!!
	if( ptr )
		Set( RefCounter::NewCounter( t ) );
}


//------------------------------------------------------------------------------
//	Copy On Write
//------------------------------------------------------------------------------
template< class T, class RefCounter >
void LockedRef<T,RefCounter> :: Cow()				//	Copy On Write: make private copy
{
	if( Base::Count() > 1 )							// @1
		Set( new RefCounter( *Base::_object ));		//	Use copy Ctor to clone Object @1
}

template< class T, class RefCounter >
void PolyLockedRef<T,RefCounter> :: Cow()			//	Copy On Write: make private copy
{
	if( Base::Count() > 1 )							// @1
		Set( RefCounter::NewCounter
			( ConstBase::GetObject().clone() ));	//	Use clone method to clone Object @1
}

//////////// File: Reference.cpp ///////////////////////////////////////////////
