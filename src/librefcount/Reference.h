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
//
// NOTE:		Pass LockedRef1& to functions to see modified object
//
//				ANSI standard does not recognize members inherited from base
//				class. Must add 'this->' or  'Base::' before each one (marked @1).
//				AIX does not support shorthand notation inside class decl
//				Had to specify arguments (marked @2).
//				AIX and NT require template argument class to supply EVERY method
//				used by every template feature EVEN if that feature is not used!
//				To use Comparison operators you have to instantiate a RefwCmp with the
//				Reference you want to use (see below).
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
//				typedef	RefwCmp< ConstRef<T, RecCountwCmp> >	ccT;
//				typedef	RefwCmp< ConstRef<T, T> >				ccT; // T support counting
//
// FILES:		Reference.h
// BUGS:
// SEE ALSO:
// TYPE:		LIBRARY-C++
////////////////////////////////////////////////////////////////////////////////


#ifndef REFERENCE_H
#define REFERENCE_H

#include <bntypes/bntypes.h>
#include <rw/edefs.h>


//#include <iostream.h>

//------------------------------------------------------------------------------
//      Counter classes:
//	Instances	Counts total number of instances of ALL classes who add it as data member
//	Counter		A Counter objects
//	RefCount	Counts references to an object. Delete itself when not referenced.
//------------------------------------------------------------------------------
template< class T >
class InstanceCount	{
public:
	InstanceCount()								{	++instances; }
	~InstanceCount()							{	--instances; }
	static	long		Count()					{	return instances; }
private:
	static	long		instances;
};

template< class T >
long	InstanceCount< T > :: instances = 0;
typedef	InstanceCount< int >		Instances;


//------------------------------------------------------------------------------
//	Counter		A Counter objects
//------------------------------------------------------------------------------
class Counter	{
public:
	Counter() : _count( 0 )						{}
	Counter(const Counter& o) : _count(o._count){}
	~Counter()									{}

	operator	unsigned int() const			{	return _count;	}
	Counter&			operator ++ ()			{	_count++;	return *this;	}
	Counter&			operator -- ()			{	_count--;	return *this;	}

private:
	unsigned int		_count;
};


//------------------------------------------------------------------------------
//	RefCount		Counts references to an object. Delete itself when not referenced.
//	RefCountwCmp	A counter that support comparison operators
//------------------------------------------------------------------------------
template< class T >
class RefCount {
public:
	RefCount(T* obj) : _object( obj )			{}
	RefCount(const RefCount<T>& obj)			{	_object = new T(*obj._object);	}
	~RefCount()									{	delete _object; }

	unsigned int		GetCount() const		{	return _count;	}
	void				Connect()				{	++_count;	}
	void				Disconnect()			{	if(!--_count)  delete this;	}

	operator			T& () const				{	return *_object; }
	static RefCount<T>*	NewCounter( T* object )	{	return new RefCount<T>( object ); }

protected:
	Counter				_count;
	T*					_object;
};

template< class T >
class RefCountwCmp : public RefCount<T> {
public:
	RefCountwCmp(T* obj) : RefCount<T>( obj )				{}
	RefCountwCmp(const RefCountwCmp& obj) : RefCount<T>(obj){}
	~RefCountwCmp()											{}

	//	Comparison operators
	int			operator == (const RefCount<T>& o) const	{ return (get() == *o);	}
	int			operator <  (const RefCount<T>& o) const	{ return (get() <  *o);	}
	int			operator >  (const RefCount<T>& o) const	{ return (get() >  *o);	}

	static RefCountwCmp<T>*		NewCounter( T* o )			{ return new RefCountwCmp<T>(o); }

private:
	const T& 	get() const									{ return *RefCount<T>::_object; }
};

//------------------------------------------------------------------------------
//      Base Reference Count - points to a const T instance
//------------------------------------------------------------------------------
template< class T, class RefCounter >
class ConstRef	{
public:
	typedef		T							RefType;
	typedef		ConstRef<T,RefCounter>		ConstRefObj;

	ConstRef() : _object(0)						{}
	ConstRef(const T* ptr );
	ConstRef(const ConstRefObj& o) : _object(0)	{	Set( o._object );	}	// @2

    ~ConstRef()									{	Disconnect();	}

    ConstRefObj&	operator = ( const T* t )	{	ConstRefObj r(t);	*this = r;	return *this;	}	// @2
    ConstRefObj&	operator = ( const ConstRef<T,RefCounter>& o );

    //	Provide Reference information
	bool			Identical(const ConstRefObj& o) const	{	return (_object == o._object);	}
	bool			Empty() const				{	return (_object == 0);	}
	long			Count() const				{	return (_object? _object->GetCount() : 0);	}

    // Provide Read only access to object.
	const T&		Get () const				{	return  GetObject();	}
	operator const T* () const					{	return &GetObject();	}
	const T*		operator -> () const		{	return &GetObject();	}
	const T&		operator * () const			{	return  GetObject();	}


protected:
	T&				GetObject () const;
	void			Disconnect()				{	if(_object)		_object->Disconnect();	}

	//	Utility method to set pointed object.
	void			Set( RefCounter* o );

	RefCounter*		_object;
};


//	Stream operator i/o support
RWDECLARE_PERSISTABLE_TEMPLATE_2(ConstRef)

template <class T, class C, class S>
void rwSaveGuts(S& str, const ConstRef<T,C>& r);
template <class T, class C, class S>
void rwRestoreGuts(S& str, ConstRef<T,C>& r);


template< class T, class RefCounter >
ostream& operator << (ostream& s, const ConstRef<T,RefCounter> & obj);
template< class T, class RefCounter >
istream& operator >> (istream& s, ConstRef<T,RefCounter>& obj);



//------------------------------------------------------------------------------
//	Reference with support for comparison operators
//------------------------------------------------------------------------------

template< class Ref >
class	 RefwCmp	:	public	Ref {
public:
	typedef	typename	Ref::RefType	RefType;
	RefwCmp() : Ref()									{}
	RefwCmp( RefType* ptr ) : Ref( ptr )				{}
	RefwCmp( const RefwCmp<Ref>& o) : Ref(o)			{}
	RefwCmp( const Ref& o ) : Ref( o )					{}
	~RefwCmp()											{}

	//	Comparison operators
	int		operator == (const RefwCmp<Ref>& o) const;
	int		operator != (const RefwCmp<Ref>& o) const	{ return !operator ==(o); }
	int		operator <  (const RefwCmp<Ref>& o) const;
	int		operator <= (const RefwCmp<Ref>& o) const	{ return operator ==(o) || operator < (o); }
	int		operator >  (const RefwCmp<Ref>& o) const;
	int		operator >= (const RefwCmp<Ref>& o) const	{ return operator ==(o) || operator > (o); }
private:
	const RefType*	object() const						{ return &Ref::Get(); }
};

//	Comparison operators
template< class Ref >
int	RefwCmp<Ref> :: operator == (const RefwCmp<Ref>& o) const	// @2
{
	return (Ref::Identical(o) ||			//	equal if If same object (or NULL!)
		   (!this->Empty() && !o.Empty() && (*object() == *o.object())) );	
}
template< class Ref >
int	RefwCmp<Ref> :: operator < (const RefwCmp<Ref>& o) const	// @2
{
	return ( !o.Empty() && (this->Empty() || (*object() < *o.object())) );
}
template< class Ref >
int	RefwCmp<Ref> :: operator > (const RefwCmp<Ref>& o) const	// @2
{
	return (  o.Empty() || (!this->Empty() && (*object() > *o.object())) );
}


//------------------------------------------------------------------------------
//	Short notaton: implicit RefCount Counter (instead of default template args)
//------------------------------------------------------------------------------
template< class T >
class	 ConstRef1	:	public	ConstRef< T, RefCount< T > > {
public:
	typedef		ConstRef< T, RefCount< T > >	Base;
	ConstRef1() : Base()						{}
	ConstRef1( T* ptr ) : Base( ptr )			{}
	ConstRef1( const ConstRef1<T>& o ) : Base( o )	{}
	ConstRef1( const Base& o ) : Base( o )		{}
};


//------------------------------------------------------------------------------
//      Locked Reference Count - Each reference ensures it has a private copy
//
//	Beware !!! This class isn't suitable for polymorphic base class pointers,
//	because it uses the base class copy c'tor for the copy. If you want to
//	store polymorphic base class pointers, use PolyLockedRef instead.
//------------------------------------------------------------------------------
template< class T, class RefCounter >
class LockedRef : public ConstRef< T, RefCounter > {
public:
	typedef	ConstRef< T, RefCounter >		Base;
	typedef	LockedRef< T, RefCounter >		LockedObj;

	LockedRef() : Base()						{}
	LockedRef( T* ptr ) : Base( ptr )			{}
	LockedRef(const Base& o) : Base(o)			{}
	LockedRef(const LockedObj& o) : Base(o)		{}

    // Provide R&W access to object.
	T&					operator * ()			{	return GetObject();		}
	T*					operator -> ()			{	return &GetObject();	}

private:
	T&					GetObject ()			{	Cow();	return Base::GetObject();	}
	virtual 	void	Cow();
};


//------------------------------------------------------------------------------
//	Short notaton: implicit RefCount Counter (instead of default template args)
//------------------------------------------------------------------------------
template< class T >
class	 LockedRef1	:	public	LockedRef< T, RefCount< T > > {
public:
	typedef	LockedRef< T, RefCount< T > >	Base;
	typedef	ConstRef< T, RefCount< T > >	ConstBase;

	LockedRef1() : Base()							{}
	LockedRef1( T* ptr ) : Base( ptr )				{}
	LockedRef1(const ConstBase& o) : Base( o )		{}
	LockedRef1(const LockedRef1<T>& o) : Base( o )	{}
};


//----------------------------------------------------------------------------------------
// Locked Reference for polymorphic classes -  The same as Locked Reference except copy is
// done by the virtual clone method (that sould be defined), instead of copy Ctor. clone
// should be declared virtual at the base class to return a pointer to the base class.
// The derived class will return a new pointer to a copy instance of self.
// ( i.e. B* D::clone() { return new D(*this) ; }  )
//----------------------------------------------------------------------------------------
template< class T, class RefCounter >
class PolyLockedRef : public LockedRef< T, RefCounter > {
public:
	typedef	LockedRef< T, RefCounter >			Base;
	typedef	ConstRef< T, RefCounter >			ConstBase;
	typedef	PolyLockedRef< T, RefCounter >		PolyLockedObj;

	PolyLockedRef() : Base()					{}
	PolyLockedRef( T* ptr ) : Base( ptr )		{}
	PolyLockedRef(const ConstBase& o) : Base( o )	{}
	PolyLockedRef(const PolyLockedObj& o) : Base( o )	{}

private:
	virtual void 	Cow();
};

//------------------------------------------------------------------------------
//	Short notaton: implicit RefCount Counter (instead of default template args)
//------------------------------------------------------------------------------
template< class T >
class	 PolyLockedRef1	:	public	PolyLockedRef< T, RefCount< T > > {
public:
	typedef	PolyLockedRef< T, RefCount< T > >	Base;
	typedef	ConstRef< T, RefCount< T >  >	ConstBase;

	PolyLockedRef1() : Base()					{}
	PolyLockedRef1( T* ptr ) : Base( ptr )		{}
	PolyLockedRef1(const ConstBase& o) : Base( o )	{}
	PolyLockedRef1(const PolyLockedRef1<T>& o) : Base( o )	{}
};


//------------------------------------------------------------------------------
//      Shared Reference - When changed other references to object are affected!
//		When creating a Shared Ref from a Const Ref - clone it so we won't change it
//------------------------------------------------------------------------------
template< class T, class RefCounter >
class SharedRef : public ConstRef< T, RefCounter > {
public:
	typedef	ConstRef< T, RefCounter >		Base;
	typedef	SharedRef< T, RefCounter >		SharedObj;

	SharedRef() : Base()						{}
	SharedRef( T* ptr ) : Base( ptr )			{}
	SharedRef(const SharedObj& o) : Base(o)		{}
	SharedRef(const Base& o) : Base(o)
	{
		if( !Base::Empty() )						// (@1)
			Set( new RefCounter( *Base::_object ));	//	Use copy Ctor to clone Object (@1)
	}

 	SharedObj&	operator = (const SharedObj& o)	{	Base::operator = (o);	return *this;	}
 	SharedObj&	operator = (const Base& o)		{	Base::operator = ( SharedObj( o ));	return *this;	}
 	SharedObj&	operator = (T* p)				{	Base::operator = ( SharedObj( p ));	return *this;	}

	T&					GetObject ()			{	return Base::GetObject ();		}

    // Provide R&W access to object.
	T&					operator * ()			{	return GetObject();		}
	T*					operator -> ()			{	return &GetObject();	}
};


//------------------------------------------------------------------------------
//	Short notaton: implicit RefCount Counter (instead of default template args)
//------------------------------------------------------------------------------
template< class T >
class	 SharedRef1	:	public	SharedRef< T, RefCount< T > > {
public:
	typedef	SharedRef< T, RefCount< T > >	Base;
	typedef	ConstRef< T, RefCount< T >  >	ConstBase;

	SharedRef1() : Base()						{}
	SharedRef1( T* ptr ) : Base( ptr )			{}
	SharedRef1( const SharedRef1<T>& o ) : Base(o)	{}
	SharedRef1( const ConstBase& o ) : Base(o)	{}
};


#ifdef COMPILE_INSTANTIATE
#	include <librefcount/Reference.cpp>
#endif


#endif	//	REFERENCE_H

//////////// File: Reference.h /////////////////////////////////////////////////
