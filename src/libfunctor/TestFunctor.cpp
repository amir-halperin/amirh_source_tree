//////////////////////////////////////////////////////////////////////////////////
// NAME: 
//   Functor -
//
// SYNOPSIS:	Callback objects library test program
// DESCRIPTION:	Test functor objects creation & functionality
//				A diffrent 'Button' class is provided for each number of arguments
//				with or without a return value.
//				Client function/method also receive a typeless client data.
//
//				Callback objects are created using:
// 					MakeFunctor##(object, method, client data)
// 				where ## stands for argument & return value (e.g. MakeFunctor1wRet)
// EXAMPLES:	
// FILES:		TestFunctor.cxx
// BUGS:
// SEE ALSO:
// TYPE:		PROGRAM
//////////////////////////////////////////////////////////////////////////////////

#include <bntypes/bntypes.h>

#ifdef _NT
#define main __declspec(dllexport) nt_main
#endif

#include <ctype.h>
#include <stdlib.h>
#include <iostream.h>

#include <libfunctor/Functor.h>
#include <librefcount/Reference.h>


bool wRet = False;
const char*	wRetStr = "with return value";

//--------------------------------------------------------------------------------
//	Callback functions & member methods
//--------------------------------------------------------------------------------
int		counter = 0;
char	cdp[] = "CDPlayer ", Def[] = "the blues ", wow[] = "Wow!! ", empty[] = "";
int		pr(void* ClientData, const char* cd, const char* music,
		   int no=1, const char* trk=empty)
{
	cout << cd << (const char*)ClientData << music << trk << no << endl;
	return ++counter;
}

class CDPlayer {
public:
	void	Func0(void* ClientData)
		{	pr(ClientData, cdp, Def);	}
	int		Func0R(void* ClientData)
		{	return pr(ClientData, cdp, Def);	}
	void	Func0C(void* ClientData) const
		{	pr(ClientData, cdp, Def);	}
	int		Func0RC(void* ClientData) const
		{	return pr(ClientData, cdp, Def);	}

	void	Func1(const char* msg, void* ClientData)
		{	pr(ClientData, cdp, msg);	}
	int		Func1R(const char* msg, void* ClientData)
		{	return pr(ClientData, cdp, msg);	}
	void	Func1C(const char* msg, void* ClientData) const
		{	pr(ClientData, cdp, msg);	}
	int		Func1RC(const char* msg, void* ClientData) const
		{	return pr(ClientData, cdp, msg);	}

	void	Func2(const char* msg, int no, void* ClientData)
		{	pr(ClientData, cdp, msg, no);	}
	int		Func2R(const char* msg, int no, void* ClientData)
		{	return pr(ClientData, cdp, msg, no);	}
	void	Func2C(const char* msg, int no, void* ClientData) const
		{	pr(ClientData, cdp, msg, no);	}
	int		Func2RC(const char* msg, int no, void* ClientData) const
		{	return pr(ClientData, cdp, msg, no);	}

	void	Func3(const char* msg, int no, const char* trk, void* ClientData)
		{	pr(ClientData, cdp, msg, no, trk);	}
	int		Func3R(const char* msg, int no, const char* trk, void* ClientData)
		{	return pr(ClientData, cdp, msg, no, trk);	}
	void	Func3C(const char* msg, int no, const char* trk, void* ClientData) const
		{	pr(ClientData, cdp, msg, no, trk);	}
	int		Func3RC(const char* msg, int no, const char* trk, void* ClientData) const
		{	return pr(ClientData, cdp, msg, no, trk);	}

	void	Func4(const char* msg, int no, const char* trk, int p4, void* ClientData) {
		pr(ClientData, cdp, msg, no, trk);
	}
	int		Func4R(const char* msg, int no, const char* trk, int p4, void* ClientData) {
		pr(ClientData, cdp, msg, no, trk);
		return p4;
	}
	void	Func4C(const char* msg, int no, const char* trk, int p4, void* ClientData) const {
		pr(ClientData, cdp, msg, no, trk);
	}
	int		Func4RC(const char* msg, int no, const char* trk, int p4, void* ClientData) const {
		pr(ClientData, cdp, msg, no, trk);
		return p4;
	}

	void	Func5(const char* msg, int no, const char* trk,
				  int p4, int p5, void* ClientData) {
		pr(ClientData, cdp, msg, no, trk);
		cout << "values: [" << p4 << "," << p5 << "]" << endl;
	}
	int		Func5R(const char* msg, int no, const char* trk,
				  int p4, int p5, void* ClientData) {
		pr(ClientData, cdp, msg, no, trk);
		cout << "values: [" << p4 << "," << p5 << "]" << endl;
		return p5;
	}
	void	Func5C(const char* msg, int no, const char* trk,
				  int p4, int p5, void* ClientData) const	{
		pr(ClientData, cdp, msg, no, trk);
		cout << "values: [" << p4 << "," << p5 << "]" << endl;
	}
	int		Func5RC(const char* msg, int no, const char* trk,
				  int p4, int p5, void* ClientData) const	{
		pr(ClientData, cdp, msg, no, trk);
		cout << "values: [" << p4 << "," << p5 << "]" << endl;
		return p5;
	}

	void	Func6(const char* msg, int no, const char* trk,
				  int p4, int p5, int p6, void* ClientData) {
		pr(ClientData, cdp, msg, no, trk);
		cout << "values: [" << p4 << "," << p5 << "," << p6 << "]" << endl;
	}
	int		Func6R(const char* msg, int no, const char* trk,
				  int p4, int p5, int p6, void* ClientData) {
		pr(ClientData, cdp, msg, no, trk);
		cout << "values: [" << p4 << "," << p5 << "," << p6 << "]" << endl;
		return p6;
	}
	void	Func6C(const char* msg, int no, const char* trk,
				   int p4, int p5, int p6, void* ClientData) const	{
		pr(ClientData, cdp, msg, no, trk);
		cout << "values: [" << p4 << "," << p5 << "," << p6 << "]" << endl;
	}
	int		Func6RC(const char* msg, int no, const char* trk,
					int p4, int p5, int p6, void* ClientData) const	{
		pr(ClientData, cdp, msg, no, trk);
		cout << "values: [" << p4 << "," << p5 << "," << p6 << "]" << endl;
		return p6;
	}

	void	Func7(const char* msg, int no, const char* trk,
				  int p4, int p5, int p6, int p7, void* ClientData) {
		pr(ClientData, cdp, msg, no, trk);
		cout << "values: [" << p4 << "," << p5 << "," << p6 << "]" << endl;
	}
	int		Func7R(const char* msg, int no, const char* trk,
				   int p4, int p5, int p6, int p7, void* ClientData) {
		pr(ClientData, cdp, msg, no, trk);
		cout << "values: [" << p4 << "," << p5 << "," << p6 << "]" << endl;
		return p7;
	}
	void	Func7C(const char* msg, int no, const char* trk,
				   int p4, int p5, int p6, int p7, void* ClientData) const	{
		pr(ClientData, cdp, msg, no, trk);
		cout << "values: [" << p4 << "," << p5 << "," << p6 << "]" << endl;
	}
	int		Func7RC(const char* msg, int no, const char* trk,
				   int p4, int p5, int p6, int p7, void* ClientData) const	{
		pr(ClientData, cdp, msg, no, trk);
		cout << "values: [" << p4 << "," << p5 << "," << p6 << "]" << endl;
		return p7;
	}
};	//	class CDPlayer

CDPlayer	cd;


void	Wow0(void* ClientData)							{	pr(ClientData, wow, Def);	}
int		Wow0R(void* ClientData)							{	return pr(ClientData, wow, Def);	}
void	Wow1(const char* msg, void* ClientData)			{	pr(ClientData, wow, msg); }
int		Wow1R(const char* msg, void* ClientData)		{	return pr(ClientData, wow, msg); }
void	Wow2(const char* msg, int no, void* ClientData)	{	pr(ClientData, wow, msg, no); }
int		Wow2R(const char* msg, int no, void* ClientData){	return pr(ClientData, wow, msg, no); }
void	Wow3(const char* msg, int no, const char* trk, void* ClientData)
		{	pr(ClientData, wow, msg, no, trk); }
int		Wow3R(const char* msg, int no, const char* trk, void* ClientData)
		{	return pr(ClientData, wow, msg, no, trk); }
void	Wow4(const char* msg, int no, const char* trk,
			 int p4, void* ClientData)
		{	pr(ClientData, wow, msg, no, trk);	}
int		Wow4R(const char* msg, int no, const char* trk,
			 int p4, void* ClientData) {
	pr(ClientData, wow, msg, no, trk);
	return p4;
}
void 	Wow5(const char* msg, int no, const char* trk,
		  int p4, int p5, void* ClientData) {
	pr(ClientData, wow, msg, no, trk);
	cout << "values: [" << p4 << "," << p5 << "]" << endl;
}
int 	Wow5R(const char* msg, int no, const char* trk,
		  int p4, int p5, void* ClientData) {
	pr(ClientData, wow, msg, no, trk);
	cout << "values: [" << p4 << "," << p5 << "]" << endl;
	return p5;
}
void 	Wow6(const char* msg, int no, const char* trk,
		  int p4, int p5, int p6, void* ClientData) {
	pr(ClientData, wow, msg, no, trk);
	cout << "values: [" << p4 << "," << p5 << "," << p6 << "]" << endl;
}
int		 Wow6R(const char* msg, int no, const char* trk,
			   int p4, int p5, int p6, void* ClientData) {
	pr(ClientData, wow, msg, no, trk);
	cout << "values: [" << p4 << "," << p5 << "," << p6 << "]" << endl;
	return p6;
}
void	Wow7(const char* msg, int no, const char* trk,
			 int p4, int p5, int p6, int p7, void* ClientData) {
	pr(ClientData, wow, msg, no, trk);
	cout << "values: [" << p4 << "," << p5 << "," << p6 << "]" << endl;
}
int		Wow7R(const char* msg, int no, const char* trk,
			  int p4, int p5, int p6, int p7, void* ClientData) {
	pr(ClientData, wow, msg, no, trk);
	cout << "values: [" << p4 << "," << p5 << "," << p6 << "]" << endl;
	return p7;
}

//--------------------------------------------------------------------------------
//	Base class for all buttons
//--------------------------------------------------------------------------------
class Butt {
public:
	Butt()	:	Name(), No(0)		{}
	Butt( const char* name, int no = 0 )
		:	Name(name), No(no)		{}
protected:
	const char*		Name;
	int				No;
};

//--------------------------------------------------------------------------------
//	Test for Functor without parameter with return value
//--------------------------------------------------------------------------------
class Button0 : public Butt {
public:
	typedef	Functor0				BtnFunctor;
	Button0( BtnFunctor* action )  :  Action(action)		{}
	~Button0()					{	Click(); }
	void			Click()		{	(*Action)();	}
private:
	typedef			ConstRef<BtnFunctor,BtnFunctor>		BtnFunctorRef;
	BtnFunctorRef	Action;
};
class Button0R : public Butt {
public:
	typedef	Functor0wRet<int>		BtnFunctor;
	Button0R( BtnFunctor* action )  :  Action(action)		{}
	~Button0R()					{	Click(); }
	void			Click()		{	cout << (*Action)() << endl;	}
private:
	typedef			ConstRef<BtnFunctor,BtnFunctor>		BtnFunctorRef;
	BtnFunctorRef	Action;
};

void	test0()
{
	cout << endl << "Testing Functors without arguments " << (wRet?wRetStr:"") << endl;
	if( wRet ) {
		Button0R	play(MakeFunctor0wRet( cd, &CDPlayer::Func0R, (void*)"playing " ));
		Button0R	stop(MakeFunctor0wRet( cd, &CDPlayer::Func0RC, (void*)"stopped " ));
		Button0R	wow (MakeFunctor0wRet( &Wow0R, (void*)"now " ));
	} else {
		Button0		play(MakeFunctor0( cd, &CDPlayer::Func0, (void*)"playing " ));
		Button0		stop(MakeFunctor0( cd, &CDPlayer::Func0C, (void*)"stopped " ));
		Button0		wow (MakeFunctor0( &Wow0, (void*)"now " ));
	}
}

//--------------------------------------------------------------------------------
//	Test for Functor with 1 parameter with return value
//--------------------------------------------------------------------------------
class Button1 : public Butt {
public:
	typedef	Functor1<const char*>				BtnFunctor;
	Button1( const char* name, BtnFunctor* action )
		:	Action(action), Butt(name)		{}
	~Button1()					{	Click(); }
	void			Click()		{	(*Action)( Name );	}
private:
	typedef			ConstRef<BtnFunctor,BtnFunctor>		BtnFunctorRef;
	BtnFunctorRef	Action;
};
class Button1R : public Butt {
public:
	typedef	Functor1wRet<int, const char*>		BtnFunctor;
	Button1R( const char* name, BtnFunctor* action )
		:	Action(action), Butt(name)		{}
	~Button1R()					{	Click(); }
	void			Click()		{	cout << (*Action)( Name ) << endl;	}
private:
	typedef			ConstRef<BtnFunctor,BtnFunctor>		BtnFunctorRef;
	BtnFunctorRef	Action;
};

void	test1()
{
	cout << endl << "Testing Functors with 1 arguments " << (wRet?wRetStr:"") << endl;

	if( wRet ) {
		Button1R	play("Jazz ", MakeFunctor1wRet( cd, &CDPlayer::Func1R, (void*)"playing " ));
		Button1R	stop("Rock ", MakeFunctor1wRet( cd, &CDPlayer::Func1RC, (void*)"stopped " ));
		Button1R	wow ("Next ", MakeFunctor1wRet( &Wow1R, (void*)"What " ));
	} else {
		Button1		play("Jazz ", MakeFunctor1( cd, &CDPlayer::Func1, (void*)"playing " ));
		Button1		stop("Rock ", MakeFunctor1( cd, &CDPlayer::Func1C, (void*)"stopped " ));
		Button1		wow ("Next ", MakeFunctor1( &Wow1, (void*)"What " ));
	}
}

//--------------------------------------------------------------------------------
//	Test for Functor with 2 parameter with return value
//--------------------------------------------------------------------------------
class Button2 : public Butt {
public:
	typedef	Functor2<const char*, int>		BtnFunctor;
	Button2( const char* name, int no, BtnFunctor* action )
		:	Action(action), Butt(name,no)		{}
	~Button2()					{	Click(); }
	void			Click()		{	(*Action)( Name, No );	}
private:
	typedef			ConstRef<BtnFunctor,BtnFunctor>		BtnFunctorRef;
	BtnFunctorRef	Action;
};
class Button2R : public Butt {
public:
	typedef	Functor2wRet<int, const char*, int>		BtnFunctor;
	Button2R( const char* name, int no, BtnFunctor* action )
		:	Action(action), Butt(name,no)		{}
	~Button2R()					{	Click(); }
	void			Click()		{	cout << (*Action)( Name, No ) << endl;	}
private:
	typedef			ConstRef<BtnFunctor,BtnFunctor>		BtnFunctorRef;
	BtnFunctorRef	Action;
};

void	test2()
{
	cout << endl << "Testing Functors with 2 arguments "  << (wRet?wRetStr:"")<< endl;
	if( wRet ) {
		Button2R	play("Jazz ", 2, MakeFunctor2wRet( cd, &CDPlayer::Func2R, (void*)"playing " ));
		Button2R	stop("Rock ", 4, MakeFunctor2wRet( cd, &CDPlayer::Func2RC, (void*)"stopped " ));
		Button2R	wow ("Next ", 8, MakeFunctor2wRet( &Wow2R, (void*)"What " ));
	} else {
		Button2		play("Jazz ", 2, MakeFunctor2( cd, &CDPlayer::Func2, (void*)"playing " ));
		Button2		stop("Rock ", 4, MakeFunctor2( cd, &CDPlayer::Func2C, (void*)"stopped " ));
		Button2		wow ("Next ", 8, MakeFunctor2( &Wow2, (void*)"What " ));
	}
}

//--------------------------------------------------------------------------------
//	Test for Functor with 3 parameter with return value
//--------------------------------------------------------------------------------
class Button3 : public Butt {
public:
	typedef	Functor3<const char*, int, const char*>	BtnFunctor;
	Button3( const char* name, int no, BtnFunctor* action )
		:	Action(action), Butt(name,no)		{}
	~Button3()					{	Click(); }
	void			Click()		{	(*Action)( Name, No, "track: " );	}
private:
	typedef			ConstRef<BtnFunctor,BtnFunctor>		BtnFunctorRef;
	BtnFunctorRef	Action;
};
class Button3R : public Butt {
public:
	typedef	Functor3wRet<int, const char*, int, const char*>	BtnFunctor;
	Button3R( const char* name, int no, BtnFunctor* action )
		:	Action(action), Butt(name,no)		{}
	~Button3R()					{	Click(); }
	void			Click()		{	cout << (*Action)( Name, No, "track: " ) << endl;	}
private:
	typedef			ConstRef<BtnFunctor,BtnFunctor>		BtnFunctorRef;
	BtnFunctorRef	Action;
};

void test3()
{
	cout << endl << "Testing Functors with 3 arguments " << (wRet?wRetStr:"") << endl;
	if( wRet ) {
		Button3R	play("Jazz ", 2, MakeFunctor3wRet( cd, &CDPlayer::Func3R, (void*)"playing " ));
		Button3R	stop("Rock ", 4, MakeFunctor3wRet( cd, &CDPlayer::Func3RC, (void*)"stopped " ));
		Button3R	wow ("Next ", 8, MakeFunctor3wRet( &Wow3R, (void*)"What " ));
	} else {
		Button3		play("Jazz ", 2, MakeFunctor3( cd, &CDPlayer::Func3, (void*)"playing " ));
		Button3		stop("Rock ", 4, MakeFunctor3( cd, &CDPlayer::Func3C, (void*)"stopped " ));
		Button3		wow ("Next ", 8, MakeFunctor3( &Wow3, (void*)"What " ));
	}
}

//--------------------------------------------------------------------------------
//	Test for Functor with 4 parameter without return value
//--------------------------------------------------------------------------------
class Button4 : public Butt {
public:
	typedef	Functor4<const char*, int, const char*, int>	BtnFunctor;
	Button4( const char* name, int no, BtnFunctor* action )
		:	Action(action), Butt(name,no)		{}
	~Button4()					{	Click(); }
	void			Click()		{	(*Action)(Name,No,"track: ",16 );	}
private:
	typedef			ConstRef<BtnFunctor,BtnFunctor>		BtnFunctorRef;
	BtnFunctorRef	Action;
};
class Button4R : public Butt {
public:
	typedef	Functor4wRet<int, const char*, int, const char*, int>	BtnFunctor;
	Button4R( const char* name, int no, BtnFunctor* action )
		:	Action(action), Butt(name,no)		{}
	~Button4R()					{	Click(); }
	void			Click()		{	cout << (*Action)(Name,No,"track: ",16 ) << endl;	}
private:
	typedef			ConstRef<BtnFunctor,BtnFunctor>		BtnFunctorRef;
	BtnFunctorRef	Action;
};

void test4()
{
	cout << endl << "Testing Functors with 4 arguments "  << (wRet?wRetStr:"") << endl;
	if( wRet ) {
		Button4R	play("Jazz ", 2, MakeFunctor4wRet( cd, &CDPlayer::Func4R, (void*)"playing " ));
		Button4R	stop("Rock ", 4, MakeFunctor4wRet( cd, &CDPlayer::Func4RC, (void*)"stopped " ));
		Button4R	wow ("Next ", 8, MakeFunctor4wRet( &Wow4R, (void*)"What " ));
	} else {
		Button4		play("Jazz ", 2, MakeFunctor4( cd, &CDPlayer::Func4, (void*)"playing " ));
		Button4		stop("Rock ", 4, MakeFunctor4( cd, &CDPlayer::Func4C, (void*)"stopped " ));
		Button4		wow ("Next ", 8, MakeFunctor4( &Wow4, (void*)"What " ));
	}
}

//--------------------------------------------------------------------------------
//	Test for Functor with 5 parameter without return value
//--------------------------------------------------------------------------------
class Button5 : public Butt {
public:
	typedef	Functor5<const char*, int, const char*, int, int>	BtnFunctor;
	Button5( const char* name, int no, BtnFunctor* action )
		:	Action(action), Butt(name,no)		{}
	~Button5()					{	Click(); }
	void			Click()		{	(*Action)( Name, No, "track: ", 16, 32 );	}
private:
	typedef			ConstRef<BtnFunctor,BtnFunctor>		BtnFunctorRef;
	BtnFunctorRef	Action;
};
class Button5R : public Butt {
public:
	typedef	Functor5wRet<int, const char*, int, const char*, int, int>	BtnFunctor;
	Button5R( const char* name, int no, BtnFunctor* action )
		:	Action(action), Butt(name,no)		{}
	~Button5R()					{	Click(); }
	void			Click()		{	cout << (*Action)( Name, No, "track: ", 16, 32 ) << endl;	}
private:
	typedef			ConstRef<BtnFunctor,BtnFunctor>		BtnFunctorRef;
	BtnFunctorRef	Action;
};

void test5()
{
	cout << endl << "Testing Functors with 5 arguments "  << (wRet?wRetStr:"") << endl;
	if( wRet ) {
		Button5R	play("Jazz ", 2, MakeFunctor5wRet( cd, &CDPlayer::Func5R, (void*)"playing " ));
		Button5R	stop("Rock ", 4, MakeFunctor5wRet( cd, &CDPlayer::Func5RC, (void*)"stopped " ));
		Button5R	wow ("Next ", 8, MakeFunctor5wRet( &Wow5R, (void*)"What " ));
	} else {
		Button5		play("Jazz ", 2, MakeFunctor5( cd, &CDPlayer::Func5, (void*)"playing " ));
		Button5		stop("Rock ", 4, MakeFunctor5( cd, &CDPlayer::Func5C, (void*)"stopped " ));
		Button5		wow ("Next ", 8, MakeFunctor5( &Wow5, (void*)"What " ));
	}
}


//--------------------------------------------------------------------------------
//	Test for Functor with 6 parameter without return value
//--------------------------------------------------------------------------------
class Button6 : public Butt {
public:
	typedef	Functor6<const char*, int, const char*, int, int, int>	BtnFunctor;
	Button6( const char* name, int no, BtnFunctor* action )
		:	Action(action), Butt(name,no)		{}
	~Button6()					{	Click(); }
	void			Click()		{	(*Action)( Name, No, "track: ", 16, 32, 64 );	}
private:
	typedef			ConstRef<BtnFunctor,BtnFunctor>		BtnFunctorRef;
	BtnFunctorRef	Action;
};
class Button6R : public Butt {
public:
	typedef	Functor6wRet<int, const char*, int, const char*, int, int, int>	BtnFunctor;
	Button6R( const char* name, int no, BtnFunctor* action )
		:	Action(action), Butt(name,no)		{}
	~Button6R()					{	Click(); }
	void			Click()		{	cout << (*Action)( Name, No, "track: ", 16, 32, 64 ) << endl;	}
private:
	typedef			ConstRef<BtnFunctor,BtnFunctor>		BtnFunctorRef;
	BtnFunctorRef	Action;
};

void test6()
{
	cout << endl << "Testing Functors with 6 arguments "  << (wRet?wRetStr:"") << endl;
	if( wRet ) {
		Button6R	play("Jazz ", 2, MakeFunctor6wRet( cd, &CDPlayer::Func6R, (void*)"playing " ));
		Button6R	stop("Rock ", 4, MakeFunctor6wRet( cd, &CDPlayer::Func6RC, (void*)"stopped " ));
		Button6R	wow ("Next ", 8, MakeFunctor6wRet( &Wow6R, (void*)"What " ));
	} else {
		Button6		play("Jazz ", 2, MakeFunctor6( cd, &CDPlayer::Func6, (void*)"playing " ));
		Button6		stop("Rock ", 4, MakeFunctor6( cd, &CDPlayer::Func6C, (void*)"stopped " ));
		Button6		wow ("Next ", 8, MakeFunctor6( &Wow6, (void*)"What " ));
	}
}

//--------------------------------------------------------------------------------
//	Test for Functor with 7 parameter with return value
//--------------------------------------------------------------------------------
class Button7 : public Butt {
public:
	typedef	Functor7<const char*, int, const char*, int, int, int, int>	BtnFunctor;
	Button7( const char* name, int no, BtnFunctor* action )
		:	Action(action), Butt(name,no)		{}
	~Button7()			{	Click(); }
	void	Click()		{	(*Action)( Name, No, "track: ", 16, 32, 64, 42 );	}
private:
	typedef			ConstRef<BtnFunctor,BtnFunctor>		BtnFunctorRef;
	BtnFunctorRef	Action;
};
class Button7R : public Butt {
public:
	typedef	Functor7wRet<int, const char*, int, const char*, int, int, int, int>	BtnFunctor;
	Button7R( const char* name, int no, BtnFunctor* action )
		:	Action(action), Butt(name,no)		{}
	~Button7R()			{	Click(); }
	void	Click()		{	cout << (*Action)( Name, No, "track: ", 16, 32, 64, 42 ) << endl;	}
private:
	typedef			ConstRef<BtnFunctor,BtnFunctor>		BtnFunctorRef;
	BtnFunctorRef	Action;
};

void test7()
{
	cout << endl << "Testing Functors with 7 arguments "  << (wRet?wRetStr:"") << endl;
	if( wRet ) {
		Button7R	play("Jazz ", 2, MakeFunctor7wRet( cd, &CDPlayer::Func7R, (void*)"playing " ));
		Button7R	stop("Rock ", 4, MakeFunctor7wRet( cd, &CDPlayer::Func7RC, (void*)"stopped " ));
		Button7R	wow ("Next ", 8, MakeFunctor7wRet( &Wow7R, (void*)"What " ));
	} else {
		Button7		play("Jazz ", 2, MakeFunctor7( cd, &CDPlayer::Func7, (void*)"playing " ));
		Button7		stop("Rock ", 4, MakeFunctor7( cd, &CDPlayer::Func7C, (void*)"stopped " ));
		Button7		wow ("Next ", 8, MakeFunctor7( &Wow7, (void*)"What " ));
	}
}

//--------------------------------------------------------------------------------
//	Main: let user select the test
//--------------------------------------------------------------------------------
void Test(int params)
{
	switch( params ) {
	case 0:
		test0();
		break;
	case 1:
		test1();
		break;
	case 2:
		test2();
		break;
	case 3:
		test3();
		break;
	case 4:
		test4();
		break;
	case 5:
		test5();
		break;
	case 6:
		test6();
		break;
	case 7:
		test7();
		break;
	default:
		cout << "Illegal no. of argument: " << params << endl;
		break;
	}
}

int main(int argc, char **argv)
{
	int		i;
	char	c;
	while ((c = getopt(argc, argv, "rRn:N:aAhH")) != EndOfOpt)
	{
		switch( toupper(c) )
		{
		case 'R':
			wRet = True;
			break;
		case 'A':
			for(i = 0; i <= 7; ++i)
				Test( i );
			break;
		case 'N':
			Test( atoi(optarg) );
			break;
		default:
			cout << "Usage: TestFunctor -R[eturn value] -A[ll] -N parameters -H[elp]" << endl;
		}
	}
	return 0;
}


//////////// File: TestFunctor.cpp /////////////////////////////////////////////
