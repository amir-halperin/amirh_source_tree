/* yvals.h values header for conforming compilers on various systems */
#if (defined(__cplusplus) && defined(__GNUC__))
/* GCC C++ has it's own yvals.h */
#include_next <yvals.h>
#else /* __cplusplus && __GNUC__ */
#ifndef _YVALS
#define _YVALS
#ifdef _NO_WINDRIVER_MODIFICATIONS
#include <stdarg.h>
#endif

#define _CPPLIB_VER	400

/* You can predefine (on the compile command line, for example):

_ALT_NS=1 -- to use namespace _Dinkum_std for C++
_ALT_NS=2 -- to use namespace _Dinkum_std for C++ and C
_C_AS_CPP -- to compile C library as C++
_C_IN_NS -- to define C names in std/_Dinkum_std instead of global namespace
_C99 -- to turn ON C99 library support
_ECPP -- to turn ON Embedded C++ dialect
_NO_EX -- to turn OFF use of try/throw
_NO_MT -- to turn OFF thread synchronization
_NO_NS -- to turn OFF use of namespace declarations
_STL_DB (or _STLP_DEBUG) -- to turn ON iterator/range debugging

You can change (in this header):

_COMPILER_TLS -- from 0 to 1 if _TLS_QUAL is not nil
_EXFAIL -- from 1 to any nonzero value for EXIT_FAILURE
_FILE_OP_LOCKS -- from 0 to 1 for file atomic locks
_GLOBAL_LOCALE -- from 0 to 1 for shared locales instead of per-thread
_HAS_IMMUTABLE_SETS -- from 1 to 0 to permit alterable set elements
_HAS_TRADITIONAL_IOSTREAMS -- from 1 to 0 to omit old iostreams functions
_HAS_TRADITIONAL_ITERATORS -- from 0 to 1 for vector/string pointer iterators
_HAS_TRADITIONAL_POS_TYPE -- from 1 to 0 for streampos same as streamoff
_HAS_TRADITIONAL_STL -- from 1 to 0 to omit old STL functions
_TLS_QUAL -- from nil to compiler TLS qualifier, such as __declspec(thread)

Include directories needed to compile with Dinkum C:

C -- include/c
C99 -- include/c (define _C99)
Embedded C++ -- include/c include/embedded (define _ECPP)
Abridged C++ -- include/c include/embedded include (define _ECPP)
Standard C++ -- include/c include
Standard C++ with export -- include/c include/export include
	(--export --template_dir=lib/export)

Include directories needed to compile with native C:

C -- none
C99 -- N/A
Embedded C++ -- include/embedded (define _ECPP)
Abridged C++ -- include/embedded include (define _ECPP)
Standard C++ -- include
Standard C++ with export -- include/export include
	(--export --template_dir=lib/export)
 */

		/* DETERMINE MACHINE TYPE */

 #if defined(i386) || defined(__i386) || defined(__386) \
	|| defined(__i386__) || defined(_M_IX86)	/* Pentium */
  #define _D0		3	/* 0: big endian, 3: little endian floating-point */

  #if defined(__RTP__)
   #define _DLONG       1       /* 0: 64, 1: 80, 2: 128 long double bits */
   #define _LBIAS       0x3ffe  /* 80/128 long double bits */
   #define _LOFF        15      /* 64 long double bits */

  #elif defined(__BORLANDC__) && !__EDG__
   #pragma warn -inl
   #define _DLONG	1	/* 0: 64, 1: 80, 2: 128 long double bits */
   #define _LBIAS	0x3ffe	/* 80/128 long double bits */
   #define _LOFF	15	/* 64 long double bits */

  #elif defined(__MINGW32__)
   #define _DLONG	1	/* 0: 64, 1: 80, 2: 128 long double bits */
   #define _LBIAS	0x3ffe	/* 80/128 long double bits */
   #define _LOFF	15	/* 64 long double bits */

  #elif defined(_M_IX86)
   #define _DLONG	0	/* 0: 64, 1: 80, 2: 128 long double bits */
   #define _LBIAS	0x3fe	/* 64 long double bits */
   #define _LOFF	4	/* 64 long double bits */

  #else /* defined(_M_IX86) */
   #define _DLONG	1	/* 0: 64, 1: 80, 2: 128 long double bits */
   #define _LBIAS	0x3ffe	/* 80/128 long double bits */
   #define _LOFF	15	/* 80/128 long double bits */
  #endif /* defined(_M_IX86) */

  #define _FPP_TYPE	_FPP_X86	/* Pentium FPP */

 #elif defined(sparc) || defined(__sparc)	/* SPARC */
  #define _D0		0	/* 0: big endian, 3: little endian floating-point */
  #if defined(_NO_WINDRIVER_MODIFICATIONS)
  #define _DLONG	2	/* 0: 64, 1: 80, 2: 128 long double bits */
  #else /* _NO_WINDRIVER_MODIFICATIONS */
  #define _DLONG	0	/* 0: 64, 1: 80, 2: 128 long double bits */
  #endif /* _NO_WINDRIVER_MODIFICATIONS */
  #define _LBIAS	0x3ffe	/* 80/128 long double bits */
  #if defined(_NO_WINDRIVER_MODIFICATIONS)
  #define _LOFF		15	/* 80/128 long double bits */
  #else /* _NO_WINDRIVER_MODIFICATIONS */
  #define _LOFF		4	/* 80/128 long double bits */
  #endif /* _NO_WINDRIVER_MODIFICATIONS */
  #define _FPP_TYPE	_FPP_SPARC	/* SPARC FPP */

 #elif defined(_MIPS) || defined(_M_MRX000) || defined(__mips) /* MIPS */
  #if defined(__LITTLE_ENDIAN__) || defined(__MIPSEL)
  #define _D0           3       /* 0: big endian, 3: little endian floating-point */
  #else /* __LITTLE_ENDIAN__ */
  #define _D0           0       /* 0: big endian, 3: little endian floating-point */
  #endif /* __LITTLE_ENDIAN__ */
  #define _DLONG	0	/* 0: 64, 1: 80, 2: 128 long double bits */
  #define _LBIAS	0x3fe	/* 64 long double bits */
  #define _LOFF		4	/* 64 long double bits */
  #define _FPP_TYPE	_FPP_MIPS	/* MIPS FPP */

  #if defined(_NO_WINDRIVER_MODIFICATIONS)
  #define _MACH_PDT	long
  #define _MACH_SZT	unsigned long
  #endif /* _NO_WINDRIVER_MODIFICATIONS */

 #elif defined(__s390__)	/* IBM S/390 */
  #define _D0		0	/* 0: big endian, 3: little endian floating-point */
  #define _DLONG	0	/* 0: 64, 1: 80, 2: 128 long double bits */
  #define _LBIAS	0x3fe	/* 64 long double bits */
  #define _LOFF		4	/* 64 long double bits */
  #define _FPP_TYPE	_FPP_S390	/* S/390 FPP */

  #define _MACH_PDT	long
  #define _MACH_SZT	unsigned long

 #elif defined(__ppc) || defined(__ppc__) || defined(_POWER) || defined(_M_PPC)	/* PowerPC */
  #define _D0		0	/* 0: big endian, 3: little endian floating-point */
  #define _DLONG	0	/* 0: 64, 1: 80, 2: 128 long double bits */
  #define _LBIAS	0x3fe	/* 64 long double bits */
  #define _LOFF		4	/* 64 long double bits */
  #define _FPP_TYPE	_FPP_PPC	/* PowerPC FPP */

  #if defined(__APPLE__)
   #define _MACH_I32	int
   #define _MACH_PDT	int
   #define _MACH_SZT	unsigned long
  #endif /* defined(__APPLE__) */

 #elif defined(__hppa)	/* HP PA-RISC */
  #define _D0		0	/* 0: big endian, 3: little endian floating-point */
  #define _DLONG	2	/* 0: 64, 1: 80, 2: 128 long double bits */
  #define _LBIAS	0x3ffe	/* 80/128 long double bits */
  #define _LOFF		15	/* 80/128 long double bits */
  #define _FPP_TYPE	_FPP_HPPA	/* HP PA-RISC FPP */

 #elif defined(_M_ALPHA)	/* Alpha */
  #define _D0		3	/* 0: big endian, 3: little endian floating-point */
  #define _DLONG	0	/* 0: 64, 1: 80, 2: 128 long double bits */
  #define _LBIAS	0x3fe	/* 80/128 long double bits */
  #define _LOFF		4	/* 80/128 long double bits */
  #define _FPP_TYPE	_FPP_ALPHA	/* HP PA-RISC FPP */

 #elif defined(_ARM_) || defined(__arm)  /* ARM */
  #if defined(__BIG_ENDIAN__) || defined(__ARMEB__)
  #define _D0           0       /* 0: big endian, 3: little endian floating-point */
  #else /* __BIG_ENDIAN__ */
  #define _D0           3       /* 0: big endian, 3: little endian floating-point */
  #endif /* __BIG_ENDIAN__ */
  #define _DLONG        0       /* 0: 64, 1: 80, 2: 128 long double bits */
  #define _LBIAS        0x3fe   /* 64 long double bits */
  #define _LOFF         4       /* 64 long double bits */
  #define _FPP_TYPE     _FPP_ARM        /* ARM ARM FPP */

 #elif defined(_M_IA64)	/* IA64 */
  #define _D0		3	/* 0: big endian, 3: little endian floating-point */
  #define _DLONG	0	/* 0: 64, 1: 80, 2: 128 long double bits */
  #define _LBIAS	0x3fe	/* 80/128 long double bits */
  #define _LOFF		4	/* 80/128 long double bits */
  #define _FPP_TYPE	_FPP_IA64	/* HP PA-RISC FPP */

 #elif defined(__sh) || defined(__sh__)
  #if defined(__BIG_ENDIAN__)
  #define _D0           0       /* 0: big endian, 3: little endian floating-point */
  #else /* __BIG_ENDIAN__ */
  #define _D0           3       /* 0: big endian, 3: little endian floating-point */
  #endif /* __BIG_ENDIAN__ */
  #define _DLONG        0       /* 0: 64, 1: 80, 2: 128 long double bits */
  #define _LBIAS        0x3fe   /* 64 long double bits */
  #define _LOFF         4       /* 64 long double bits */
  #define _FPP_TYPE     _FPP_SH4       /* SH4 FPP */

 #else /* system detector */
/* #error unknown compilation environment, guess big-endian */
  #define _D0		0	/* 0: big endian, 3: little endian floating-point */
  #define _DLONG	0	/* 0: 64, 1: 80, 2: 128 long double bits */
  #define _LBIAS	0x3fe	/* 64 long double bits */
  #define _LOFF		4	/* 64 long double bits */
 #endif /* system detector */

		/* DETERMINE _Ptrdifft AND _Sizet FROM MACHINE TYPE */

 #if defined(_MACH_I32)
typedef _MACH_I32 _Int32t;
typedef unsigned _MACH_I32 _Uint32t;

 #else /* defined(_MACH_I32) */
typedef long _Int32t;
typedef unsigned long _Uint32t;
 #endif /* defined(_MACH_I32) */

 #if defined(_MACH_PDT)
typedef _MACH_PDT _Ptrdifft;

 #else /* defined(_MACH_PDT) */
typedef int _Ptrdifft;
 #endif /* defined(_MACH_PDT) */

 #if defined(_MACH_SZT)
typedef _MACH_SZT _Sizet;

 #else /* defined(_MACH_SZT) */
typedef unsigned int _Sizet;
 #endif /* defined(_MACH_SZT) */

 #if defined(__linux)
  #define _LINUX_C_LIB	1	/* Linux-specific conventions */
 #endif /* defined(__linux) */

 #define _HAS_TRADITIONAL_IOSTREAMS	1	/* add old iostreams stuff */

 #if defined(_M_IX86) || defined(_M_MRX000) || defined(_M_PPC) \
	|| defined(_M_ALPHA) || defined(_M_IA64)
  #define _WIN32_C_LIB	1	/* use Windows32 conventions */

 #else /* defined(_M_*) */
  #define _HAS_POSIX_C_LIB	1	/* use common Unix/Linux conventions */
 #endif /* defined(_M_*) */

 #define _ADDED_C_LIB	1	/* add declarations for C extensions */

 #if !defined(_HAS_C9X) && defined(_C99)
  #define _HAS_C9X	1
 #endif /* !defined(_HAS_C9X) etc. */

 #define _HAS_C9X_IMAGINARY_TYPE	(_HAS_C9X && __EDG__ \
	&& !defined(__cplusplus))

 #if defined(_NO_WINDRIVER_MODIFICATIONS)
 #if !defined(_IS_EMBEDDED) && defined(_ECPP)
  #define _IS_EMBEDDED	1	/* 1 for Embedded C++ */
 #endif /* _IS_EMBEDDED */
 #else /* _NO_WINDRIVER_MODIFICATIONS */
  #define _IS_EMBEDDED		__CONFIGURE_EMBEDDED
 #endif

		/* EXCEPTION CONTROL */
 #if defined(_NO_WINDRIVER_MODIFICATIONS)
 #ifndef _HAS_EXCEPTIONS
  #define _HAS_EXCEPTIONS	!defined(_NO_EX)	/* 1 for try/throw logic */
 #endif /* _HAS_EXCEPTIONS */
 #else /* _NO_WINDRIVER_MODIFICATIONS */
  #define _HAS_EXCEPTIONS	__CONFIGURE_EXCEPTIONS	/* 1 for try/throw logic */
 #endif /* _NO_WINDRIVER_MODIFICATIONS */

		/* NAMING PROPERTIES */
/* #define _STD_LINKAGE	defines C names as extern "C++" */
/* #define _STD_USING	defines C names in namespace std or _Dinkum_std */

 #ifndef _HAS_NAMESPACE
  #define _HAS_NAMESPACE	!defined(_NO_NS)	/* 1 for C++ names in std */
 #endif /* _HAS_NAMESPACE */

 #if !defined(_STD_USING) && defined(__cplusplus) \
	&& (defined(_C_IN_NS) || 0 < _ALT_NS)
  #define _STD_USING	exports C names to global, else reversed
 #endif /* !defined(_STD_USING) */

 #if defined(_NO_WINDRIVER_MODIFICATIONS)
 #if !defined(_HAS_STRICT_LINKAGE) \
	&& __EDG__ && !defined(_WIN32_C_LIB)
  #define _HAS_STRICT_LINKAGE	1	/* extern "C" in function type */
 #endif /* !defined(_HAS_STRICT_LINKAGE) */
 #else /* _NO_WINDRIVER_MODIFICATIONS */
  #define _HAS_STRICT_LINKAGE 	0
 #endif /* _NO_WINDRIVER_MODIFICATIONS */

#define _USE_EXISTING_SYSTEM_NAMES	1 /* _Open => open etc. */

		/* THREAD AND LOCALE CONTROL */
 #ifndef _MULTI_THREAD

  #ifdef __CYGWIN__
   #define _MULTI_THREAD	0	/* Cygwin has dummy thread library */

  #else /* __CYGWIN__ */
   #if defined(_NO_WINDRIVER_MODIFICATIONS)
   #define _MULTI_THREAD	!defined(_NO_MT)	/* 0 for no thread locks */
   #else /* _NO_WINDRIVER_MODIFICATIONS */
   #define _MULTI_THREAD	1	/* 0 for no thread locks */
   #endif /* _NO_WINDRIVER_MODIFICATIONS */
  #endif /* __CYGWIN__ */

 #endif /* _MULTI_THREAD */

#if defined(_NO_WINDRIVER_MODIFICATIONS)
#define _GLOBAL_LOCALE	0	/* 0 for per-thread locales, 1 for shared */
#define _FILE_OP_LOCKS	0	/* 0 for no file atomic locks, 1 for atomic */
#else /* _NO_WINDRIVER_MODIFICATIONS */
#define _GLOBAL_LOCALE	1	/* 0 for per-thread locales, 1 for shared */
#define _FILE_OP_LOCKS	1	/* 0 for no file atomic locks, 1 for atomic */
#endif /* _NO_WINDRIVER_MODIFICATIONS */

		/* THREAD-LOCAL STORAGE */
#define _COMPILER_TLS	0	/* 1 if compiler supports TLS directly */
#define _TLS_QUAL	/* TLS qualifier, such as __declspec(thread), if any */

 #define _HAS_IMMUTABLE_SETS	1
 #define _HAS_TRADITIONAL_IOSTREAMS	1
 #define _HAS_TRADITIONAL_ITERATORS	0
 #define _HAS_TRADITIONAL_POS_TYPE	0
 #define _HAS_TRADITIONAL_STL	1

 #if !defined(_HAS_ITERATOR_DEBUGGING) \
	&& (defined(_STL_DB) || defined(_STLP_DEBUG))
  #define _HAS_ITERATOR_DEBUGGING	1	/* for range checks, etc. */
 #endif /* !defined(_HAS_ITERATOR_DEBUGGING) && defined(_STLP_DEBUG) */

		/* NAMESPACE CONTROL */

 #if defined(__cplusplus)

 #if _HAS_NAMESPACE
namespace std {}

 #if defined(_C_AS_CPP)
  #define _NO_CPP_INLINES	/* just for compiling C library as C++ */
 #endif /* _C_AS_CPP */

 #if 0 < _ALT_NS

  #if defined(_C_AS_CPP)	/* define library in _Dinkum_std */
   #define _STD_BEGIN	namespace _Dinkum_std {_C_LIB_DECL
   #define _STD_END		_END_C_LIB_DECL }

  #else /* _C_AS_CPP */
   #define _STD_BEGIN	namespace _Dinkum_std {
   #define _STD_END		}
  #endif /* _C_AS_CPP */

  #if _ALT_NS == 1	/* define C library in global namespace */
    #define _C_STD_BEGIN
    #define _C_STD_END
    #define _CSTD		::
    #define _STD			::_Dinkum_std::

  #else	/* define both C and C++ in namespace _Dinkum_std */
    #define _C_STD_BEGIN	namespace _Dinkum_std {
    #define _C_STD_END	}
    #define _CSTD		::_Dinkum_std::
    #define _STD			::_Dinkum_std::
  #endif /* _ALT_NS */

namespace _Dinkum_std {}
namespace std {
	using namespace _Dinkum_std;
		}

 #elif defined(_STD_USING)

  #if defined(_C_AS_CPP)	/* define library in std */
   #define _STD_BEGIN	namespace std {_C_LIB_DECL
   #define _STD_END		_END_C_LIB_DECL }

  #else /* _C_AS_CPP */
   #define _STD_BEGIN	namespace std {
   #define _STD_END		}
  #endif /* _C_AS_CPP */

   #define _C_STD_BEGIN	namespace std {
   #define _C_STD_END	}
   #define _CSTD		::std::
   #define _STD			::std::

 #else /* _ALT_NS == 0 && !defined(_STD_USING) */

  #if defined(_C_AS_CPP)	/* define C++ library in std, C in global */
   #define _STD_BEGIN	_C_LIB_DECL
   #define _STD_END		_END_C_LIB_DECL

  #else /* _C_AS_CPP */
   #define _STD_BEGIN	namespace std {
   #define _STD_END		}
  #endif /* _C_AS_CPP */

   #define _C_STD_BEGIN	
   #define _C_STD_END	
   #define _CSTD		::
   #if defined(_NO_WINDRIVER_MODIFICATIONS)
   #define _STD			::std::
   #else /* _NO_WINDRIVER_MODIFICATIONS */
   #define _STD			std::
   #endif /* _NO_WINDRIVER_MODIFICATIONS */
 #endif /* _ALT_NS etc */

  #define _X_STD_BEGIN	namespace std {
  #define _X_STD_END	}
  #define _XSTD			::std::

  #if defined(_STD_USING)
   #undef _GLOBAL_USING		/* c* in std namespace, *.h imports to global */

  #else /* defined(_STD_USING) */
   #define _GLOBAL_USING	/* *.h in global namespace, c* imports to std */
  #endif /* defined(_STD_USING) */

  #if defined(_STD_LINKAGE)
   #define _C_LIB_DECL		extern "C++" {	/* C has extern "C++" linkage */

  #else /* defined(_STD_LINKAGE) */
   #define _C_LIB_DECL		extern "C" {	/* C has extern "C" linkage */
  #endif /* defined(_STD_LINKAGE) */

  #define _END_C_LIB_DECL	}
  #define _EXTERN_C			extern "C" {
  #define _END_EXTERN_C		}

 #else /* _HAS_NAMESPACE */
  #define _STD_BEGIN
  #define _STD_END
  #define _STD	::

  #define _X_STD_BEGIN
  #define _X_STD_END
  #define _XSTD	::

  #define _C_STD_BEGIN
  #define _C_STD_END
  #define _CSTD	::

  #define _C_LIB_DECL		extern "C" {
  #define _END_C_LIB_DECL	}
  #define _EXTERN_C			extern "C" {
  #define _END_EXTERN_C		}
 #endif /* _HAS_NAMESPACE */

 #else /* __cplusplus */
  #define _STD_BEGIN
  #define _STD_END
  #define _STD

  #define _X_STD_BEGIN
  #define _X_STD_END
  #define _XSTD

  #define _C_STD_BEGIN
  #define _C_STD_END
  #define _CSTD

  #define _C_LIB_DECL
  #define _END_C_LIB_DECL
  #define _EXTERN_C
  #define _END_EXTERN_C
 #endif /* __cplusplus */

 #if 199901L <= __STDC_VERSION__

 #if defined(__GNUC__) || defined(__cplusplus)
  #define _Restrict

 #else /* defined(__GNUC__) || defined(__cplusplus) */
  #define _Restrict restrict
 #endif /* defined(__GNUC__) || defined(__cplusplus) */

 #if defined(__GNUC__) && defined(__cplusplus)
  #define _C99_float_complex	float __complex__
  #define _C99_double_complex	double __complex__
  #define _C99_ldouble_complex	long double __complex__
 #endif /* defined(__GNUC__) && defined(__cplusplus) */

 #else /* 199901L <= __STDC_VERSION__ */
 #define _Restrict
 #endif /* 199901L <= __STDC_VERSION__ */

 #ifdef __cplusplus
_STD_BEGIN
typedef bool _Bool;
_STD_END
 #endif /* __cplusplus */

		/* VC++ COMPILER PARAMETERS */
#define _CRTIMP
#define _CDECL

 #if defined(_WIN32_C_LIB)
  #ifndef _VA_LIST_DEFINED

   #if defined(__BORLANDC__)

   #elif defined(_M_ALPHA)
typedef struct
	{	/* define va_list a la EDG */
	char *a0;
	int offset;
	} va_list;

   #else /* defined(_M_ALPHA) */
typedef char *va_list;
   #endif /* defined(_M_ALPHA) */

  #define _VA_LIST_DEFINED
 #endif /* _VA_LIST_DEFINED */

 #if _HAS_C9X

 #if defined(__cplusplus) || __EDG__ || 0 < __GNUC__
  #define _LONGLONG	long long
  #define _ULONGLONG	unsigned long long

 #else /* __cplusplus */
  #define _LONGLONG	__int64
  #define _ULONGLONG	unsigned __int64
 #endif /* __cplusplus */

 #else /* _IS_C9X */

 #ifdef __cplusplus
  #define _LONGLONG	long long
  #define _ULONGLONG	unsigned long long

 #else /* __cplusplus */
  #if defined(__RTP__)
  #define _LONGLONG	long long
  #define _ULONGLONG	unsigned long long
  #else /* __RTP__ */
  #define _LONGLONG	__int64
  #define _ULONGLONG	unsigned __int64
  #endif /* __RTP__ */
 #endif /* __cplusplus */

 #endif /* _IS_C9X */

  #define _LLONG_MAX	0x7fffffffffffffff
  #define _ULLONG_MAX	0xffffffffffffffff

 #else /* defined(_WIN32_C_LIB) */
#define _LONGLONG	long long
#define _ULONGLONG	unsigned long long
#define _LLONG_MAX	0x7fffffffffffffffLL
#define _ULLONG_MAX	0xffffffffffffffffULL
 #endif /* defined(_WIN32_C_LIB) */

		/* MAKE MINGW LOOK LIKE WIN32 HEREAFTER */

 #if defined(__MINGW32__)
  #define _WIN32_C_LIB	1
 #endif /* defined(__MINGW32__) */

 #if _WIN32_C_LIB && !defined(__BORLANDC__)
  #undef _HAS_POSIX_C_LIB

  #if !defined(_SIZE_T) && !defined(_SIZET) \
	&& !defined(_BSD_SIZE_T_DEFINED_) \
	&& !defined(_SIZE_T_DEFINED)
   #define _SIZE_T
   #define _SIZET
   #define _BSD_SIZE_T_DEFINED_
   #define _STD_USING_SIZE_T
   #define _SIZE_T_DEFINED

typedef _Sizet size_t;
  #endif /* !defined(_SIZE_T) etc. */

  #if !defined(_WCHAR_T_DEFINED)
   #define _WCHAR_T_DEFINED
   #define _WCHAR_T_
   #undef __need_wchar_t

   #ifndef __cplusplus
typedef unsigned short wchar_t;
   #endif /* __cplusplus */

  #endif /* !defined(_WCHAR_T) etc. */
 #endif /* _WIN32_C_LIB */

_C_STD_BEGIN

		/* FLOATING-POINT PROPERTIES */
#define _DBIAS	0x3fe	/* IEEE format double and float */
#define _DOFF	4
#define _FBIAS	0x7e
#define _FOFF	7
#define _FRND	1

		/* INTEGER PROPERTIES */
#define _BITS_BYTE	8
#define _C2			1	/* 0 if not 2's complement */
#define _MBMAX		8	/* MB_LEN_MAX */
#define _ILONG		1	/* 0 if 16-bit int */

#if defined(_NO_WINDRIVER_MODIFICATIONS)
 #if defined(__s390__) || defined(__CHAR_UNSIGNED__)  \
	|| defined(_CHAR_UNSIGNED)
  #define _CSIGN	0	/* 0 if char is not signed */

 #else /* defined(__s390__) || defined(__CHAR_UNSIGNED__) */
  #define _CSIGN	1
 #endif /* defined(__s390__) || defined(__CHAR_UNSIGNED__) */
#else /* _NO_WINDRIVER_MODIFICATIONS */
#if defined(__ppc) || defined(__rce) || defined(__mips) || defined(__arm) || defined(__s390__) || defined(__CHAR_UNSIGNED__) || defined(_CHAR_UNSIGNED)
#define _CSIGN		0	/* 0 if char is not signed */
#else
#define _CSIGN		1	/* 0 if char is not signed */
#endif
#endif /* _NO_WINDRIVER_MODIFICATIONS */

#define _MAX_EXP_DIG	8	/* for parsing numerics */
#define _MAX_INT_DIG	32
#define _MAX_SIG_DIG	36

 #if defined(_LONGLONG)
typedef _LONGLONG _Longlong;
typedef _ULONGLONG _ULonglong;

 #else /* defined(_LONGLONG) */
typedef long _Longlong;
typedef unsigned long _ULonglong;
 #define _LLONG_MAX		0x7fffffffL
 #define _ULLONG_MAX	0xffffffffUL
 #endif /* defined(_LONGLONG) */

		/* wchar_t AND wint_t PROPERTIES */

 #if defined(_WCHAR_T) || defined(_WCHAR_T_DEFINED) \
	|| defined (_MSL_WCHAR_T_TYPE)
  #define _WCHART
 #endif /* defined(_WCHAR_T) || defined(_WCHAR_T_DEFINED) */

 #if defined(_WINT_T)
  #define _WINTT
 #endif /* _WINT_T */

 #ifdef __cplusplus
  #define _WCHART
typedef wchar_t _Wchart;
typedef wchar_t _Wintt;
 #endif /* __cplusplus */

 #if defined(_MSL_WCHAR_T_TYPE)
  #define _WCMIN	0
  #define _WCMAX	0xffff

  #ifndef __cplusplus
typedef wchar_t _Wchart;
typedef wint_t _Wintt;
  #endif /* __cplusplus */

  #define mbstate_t	_DNK_mbstate_t
  #define wctype_t	_DNK_wctype_t
  #define wint_t	_DNK_wint_t
  #define _MSC_VER	1

 #elif defined(_WIN32_C_LIB) || defined(__RTP__)
  #define _WCMIN	0
  #define _WCMAX	0xffff

  #ifndef __cplusplus
typedef unsigned short _Wchart;
typedef unsigned short _Wintt;
  #endif /* __cplusplus */

 #elif defined(__CYGWIN__)
  #define _WCMIN	(-_WCMAX - _C2)
  #define _WCMAX	0x7fff

  #ifndef __cplusplus
typedef short _Wchart;
typedef short _Wintt;
  #endif /* __cplusplus */

 #elif defined(__WCHAR_TYPE__)
  #define _WCMIN	(-_WCMAX - _C2)
  #define _WCMAX	0x7fffffff	/* assume signed 32-bit wchar_t */

  #ifndef __cplusplus
typedef __WCHAR_TYPE__ _Wchart;
typedef __WCHAR_TYPE__ _Wintt;
  #endif /* __cplusplus */

 #else /* default wchar_t/wint_t */
  #define _WCMIN	(-_WCMAX - _C2)
  #define _WCMAX	0x7fffffff

  #ifndef __cplusplus
typedef long _Wchart;
typedef long _Wintt;
  #endif /* __cplusplus */

 #endif /* compiler/library type */

		/* POINTER PROPERTIES */
#define _NULL		0	/* 0L if pointer same as long */

		/* signal PROPERTIES */

 #if defined(_WIN32_C_LIB)
#define _SIGABRT	22
#define _SIGMAX		32

 #else /* defined(_WIN32_C_LIB) */
#define _SIGABRT	6
#define _SIGMAX		44
 #endif /* defined(_WIN32_C_LIB) */

#ifdef _NO_WINDRIVER_MODIFICATIONS
		/* stdarg PROPERTIES */
typedef va_list _Va_list;

 #if _HAS_C9X

 #if __EDG__
  #undef va_copy
 #endif /* __EDG__ */

 #ifndef va_copy
_EXTERN_C
void _Vacopy(va_list *, va_list);
_END_EXTERN_C
  #define va_copy(apd, aps)	_Vacopy(&(apd), aps)
 #endif /* va_copy */

 #endif /* _IS_C9X */
#endif

		/* stdlib PROPERTIES */
#define _EXFAIL	1	/* EXIT_FAILURE */

_EXTERN_C
void _Atexit(void (*)(void));
_END_EXTERN_C

		/* stdio PROPERTIES */
#define _FNAMAX	260
#define _FOPMAX	20
#define _TNAMAX	16

#define _FD_VALID(fd)	(0 <= (fd))	/* fd is signed integer */
#define _FD_INVALID		(-1)

		/* STORAGE ALIGNMENT PROPERTIES */
#define _MEMBND	3U /* eight-byte boundaries (2^^3) */

		/* time PROPERTIES */
#define _CPS	1
#define _TBIAS	((70 * 365LU + 17) * 86400)
_C_STD_END

		/* MULTITHREAD PROPERTIES */

 #if _MULTI_THREAD
_EXTERN_C
  #if defined(__RTP__)
void _Locksyslock(unsigned int);
void _Unlocksyslock(unsigned int);

typedef struct __diab_semaphore_struct *__diab_semaphore;
extern __diab_semaphore __diab_alloc_semaphore(void);
extern void __diab_free_semaphore(__diab_semaphore);
extern void __diab_lock_semaphore(__diab_semaphore);
extern int __diab_trylock_semaphore(__diab_semaphore);
extern void __diab_unlock_semaphore(__diab_semaphore);
  #else /* __RTP__ */
void _Locksyslock(int);
void _Unlocksyslock(int);
  #endif /* __RTP__ */
_END_EXTERN_C

 #else /* _MULTI_THREAD */
  #define _Locksyslock(x)	(void)0
  #define _Unlocksyslock(x)	(void)0
 #endif /* _MULTI_THREAD */

		/* LOCK MACROS */
 #define _LOCK_LOCALE	0
 #define _LOCK_MALLOC	1
 #define _LOCK_STREAM	2
 #define _LOCK_DEBUG	3
 #define _MAX_LOCK		4	/* one more than highest lock number */

 #ifdef __cplusplus
_STD_BEGIN
extern "C++" {	// in case of _C_AS_CPP
		// CLASS _Lockit
class _Lockit
	{	// lock while object in existence -- MUST NEST
public:

  #if _MULTI_THREAD
   #define _LOCKIT(x)	lockit x
	explicit _Lockit()
		: _Locktype(0)
		{	// set default lock
		_Locksyslock(_Locktype);
		}

	explicit _Lockit(int _Type)
		: _Locktype(_Type)
		{	// set the lock
		_Locksyslock(_Locktype);
		}

	~_Lockit()
		{	// clear the lock
		_Unlocksyslock(_Locktype);
		}

private:
	_Lockit(const _Lockit&);				// not defined
	_Lockit& operator=(const _Lockit&);	// not defined

	int _Locktype;

  #else /* _MULTI_THREAD */
   #define _LOCKIT(x)
	explicit _Lockit()
		{	// do nothing
		}

	explicit _Lockit(int)
		{	// do nothing
		}

	~_Lockit()
		{	// do nothing
		}
  #endif /* _MULTI_THREAD */

	};

class _Mutex
	{	// lock under program control
public:

  #if _MULTI_THREAD
   #if defined(__RTP__)
	_Mutex() {
		_Semp = __diab_alloc_semaphore();
	}
	~_Mutex() {
		__diab_free_semaphore(_Semp);
	}
	void _Lock() {
		__diab_lock_semaphore(_Semp);
	}
	void _Unlock() {
		__diab_unlock_semaphore(_Semp);
	}

   #else /* __RTP__ */
	_Mutex();
	~_Mutex();
	void _Lock();
	void _Unlock();
   #endif /* __RTP__ */

private:
	_Mutex(const _Mutex&);				// not defined
	_Mutex& operator=(const _Mutex&);	// not defined
   #if defined(__RTP__)
	__diab_semaphore _Semp;
   #else /* __RTP__ */
	void *_Mtx;
   #endif /* __RTP__ */

  #else /* _MULTI_THREAD */
    void _Lock()
		{	// do nothing
		}

	void _Unlock()
		{	// do nothing
	}
  #endif /* _MULTI_THREAD */

	};
}	// extern "C++"
_STD_END
 #endif /* __cplusplus */

		/* MISCELLANEOUS MACROS */
#define _ATEXIT_T	void

#define _MAX	(max)
#define _MIN	(min)

#define _TEMPLATE_STAT

 #if 0 < __GNUC__
  #define _NO_RETURN(fun)	void fun __attribute__((__noreturn__))

 #elif 1200 <= _MSC_VER
  #define _NO_RETURN(fun)	__declspec(noreturn) void fun

 #else /* compiler selector */
  #define _NO_RETURN(fun)	void fun
 #endif /* compiler selector */

#ifdef _NO_WINDRIVER_MODIFICATIONS
 #if _HAS_NAMESPACE

 #ifdef __cplusplus

  #if !defined(_C_AS_CPP) && defined(_GLOBAL_USING)
_STD_BEGIN
using ::va_list;
_STD_END
  #endif /* !defined(_C_AS_CPP) && defined(_GLOBAL_USING) */

 #endif /* __cplusplus */
 #endif /* _HAS_NAMESPACE */
#endif /* _NO_WINDRIVER_MODIFICATIONS */

#endif /* _YVALS */
#endif /* __cplusplus ** __GNUC__ */

/*
 * Copyright (c) 1992-2002 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V4.00:1278 */
