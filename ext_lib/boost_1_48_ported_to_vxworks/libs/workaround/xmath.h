/* xmath.h internal header */
#ifndef _XMATH
#define _XMATH
#include <errno.h>
#include <math.h>
#include <stddef.h>
#include "ymath.h"

_C_STD_BEGIN

		/* FLOAT PROPERTIES */
#ifndef _D0
 #define _D0	3	/* little-endian, small long doubles */
 #define _D1	2
 #define _D2	1
 #define _D3	0

 #define _DBIAS	0x3fe
 #define _DOFF	4

 #define _FBIAS	0x7e
 #define _FOFF	7
 #define _FRND	1

 #define _DLONG	0
 #define _LBIAS	0x3fe
 #define _LOFF	4

#elif _D0 == 0		/* other params defined in <yvals.h> */
 #define _D1	1	/* big-endian */
 #define _D2	2
 #define _D3	3

#else /* _D0 */
 #define _D1	2	/* little-endian */
 #define _D2	1
 #define _D3	0
#endif /* _D0 */

		/* IEEE 754 double properties */
#define _DFRAC	((unsigned short)((1 << _DOFF) - 1))
#define _DMASK	((unsigned short)(0x7fff & ~_DFRAC))
#define _DMAX	((unsigned short)((1 << (15 - _DOFF)) - 1))
#define _DSIGN	((unsigned short)0x8000)
#define DSIGN(x)	(((unsigned short *)(char *)&(x))[_D0] & _DSIGN)
#define HUGE_EXP	(int)(_DMAX * 900L / 1000)
#define HUGE_RAD	2.73e9	/* ~ 2^33 / pi */
#define SAFE_EXP	((short)(_DMAX >> 1))

		/* IEEE 754 float properties */
#define _FFRAC	((unsigned short)((1 << _FOFF) - 1))
#define _FMASK	((unsigned short)(0x7fff & ~_FFRAC))
#define _FMAX	((unsigned short)((1 << (15 - _FOFF)) - 1))
#define _FSIGN	((unsigned short)0x8000)
#define FSIGN(x)	(((unsigned short *)(char *)&(x))[_F0] & _FSIGN)
#define FHUGE_EXP	(int)(_FMAX * 900L / 1000)
#define FHUGE_RAD	40.7	/* ~ 2^7 / pi */
#define FSAFE_EXP	((short)(_FMAX >> 1))

 #if _D0 == 0
  #define _F0	0	/* big-endian */
  #define _F1	1

 #else /* _D0 == 0 */
  #define _F0	1	/* little-endian */
  #define _F1	0
 #endif /* _D0 == 0 */

		/* IEEE 754 long double properties */
#define _LFRAC	((unsigned short)(-1))
#define _LMASK	((unsigned short)0x7fff)
#define _LMAX	((unsigned short)0x7fff)
#define _LSIGN	((unsigned short)0x8000)
#define LSIGN(x)	(((unsigned short *)(char *)&(x))[_L0] & _LSIGN)
#define LHUGE_EXP	(int)(_LMAX * 900L / 1000)
#define LHUGE_RAD	2.73e9	/* ~ 2^33 / pi */
#define LSAFE_EXP	((short)(_LMAX >> 1))

 #if _D0 == 0
  #define _L0	0	/* big-endian */
  #define _L1	1
  #define _L2	2
  #define _L3	3
  #define _L4	4
  #define _L5	5	/* 128-bit only */
  #define _L6	6
  #define _L7	7

 #elif _DLONG == 0
  #define _L0	3	/* little-endian, 64-bit long doubles */
  #define _L1	2
  #define _L2	1
  #define _L3	0
  #define _L4	xxx	/* should never be used */
  #define _L5	xxx
  #define _L6	xxx
  #define _L7	xxx

 #elif _DLONG == 1
  #define _L0	4	/* little-endian, 80-bit long doubles */
  #define _L1	3
  #define _L2	2
  #define _L3	1
  #define _L4	0
  #define _L5	xxx	/* should never be used */
  #define _L6	xxx
  #define _L7	xxx

 #else /* _DLONG */
  #define _L0	7	/* little-endian, 128-bit long doubles */
  #define _L1	6
  #define _L2	5
  #define _L3	4
  #define _L4	3
  #define _L5	2
  #define _L6	1
  #define _L7	0
 #endif /* _DLONG */

		/* return values for _Stopfx/_Stoflt */
#define FL_ERR	0
#define FL_DEC	1
#define FL_HEX	2
#define FL_INF	3
#define FL_NAN	4
#define FL_NEG	8

_C_LIB_DECL
int _Stopfx(const char **, char **);
int _Stoflt(const char *, const char *, char **,
	long[], int);
int _Stoxflt(const char *, const char *, char **,
	long[], int);
int _WStopfx(const wchar_t **, wchar_t **);
int _WStoflt(const wchar_t *, const wchar_t *, wchar_t **,
	long[], int);
int _WStoxflt(const wchar_t *, const wchar_t *, wchar_t **,
	long[], int);

		/* double declarations */
double _Atan(double, int);
short _Dint(double *, short);
short _Dnorm(unsigned short *);
short _Dscale(double *, long);
double _Dtento(double, long);
short _Dunscale(short *, double *);
double _Hypot(double, double, int *);
double _Poly(double, const double *, int);
extern /* const */ _Dconst _Eps, _Rteps;
extern /* const */ double _Xbig, _Zero;

		/* float declarations */
float _FAtan(float, int);
short _FDint(float *, short);
short _FDnorm(unsigned short *);
short _FDscale(float *, long);
float _FDtento(float, long);
short _FDunscale(short *, float *);
float _FHypot(float, float, int *);
float _FPoly(float, const float *, int);
extern /* const */ _Dconst _FEps, _FRteps;
extern /* const */ float _FXbig, _FZero;

		/* long double functions */
long double _LAtan(long double, int);
short _LDint(long double *, short);
short _LDnorm(unsigned short *);
short _LDscale(long double *, long);
long double _LDtento(long double, long);
short _LDunscale(short *, long double *);
long double _LHypot(long double, long double, int *);
long double _LPoly(long double, const long double *, int);
extern /* const */ _Dconst _LEps, _LRteps;
extern /* const */ long double _LXbig, _LZero;
_END_C_LIB_DECL
_C_STD_END
#endif /* _XMATH */

/*
 * Copyright (c) 1992-2002 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V4.00:1278 */
