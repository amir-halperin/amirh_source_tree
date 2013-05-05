/* XdpTrace.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpTrace
   <replace this by module description text>


	An buffer entry is composed from one or more buffer records.
	Buffer record is eight UINT32-s.

	The first buffer record format is:
	[0] - Flags and line number
		 3: 0 - Entry color - XDP_TRACE_color
            4 - Valid record bit
            5 - First record of entry
            6 - Last record of entry (if set, the entry has single record)
		11: 7 - Application filter tag
        15:12 - Not Used
		31:16 - Line number

	[1] - Time stamp
	[2] - Format string point. The user format string is concatinated
			with the file name in the macro. They are seperated by "~"
	[3] - Currnet Task
	[4] - User paramter P0
	[5] - User paramter P1
	[6] - User paramter P2
	[7] - User paramter P3

	If needed a second (third) record is used:
 	[0] - Flags
  		 3: 0 - Not used
            4 - Valid record bit
            5 - First record of entry (never set in non first record)
            6 - Last record of entry (if set, the entry has single record)
	    31: 7 - Not used
	
	[1] - User paramrter P4 (P11) - second (third) record
	[2] - User paramrter P5 (P12) - second (third) record
	[3] - User paramrter P6 (P13) - second (third) record
	[4] - User paramrter P7 (P14) - second (third) record
	[5] - User paramrter P8 (P15) - second (third) record
	[6] - User paramrter P9 (P16) - second (third) record
	[7] - User paramrter P10(P17) - second (third) record

AUTHOR
   Gaash, SB

		
*************************************************************************/

#ifndef XdpTrace_h
#define XdpTrace_h

#include "basictypes.h"
#include "RWHdr/cstring.h"
#include "MonApplic.h"


// ============================================================================
// ===
// === Access macros
// ===
// ============================================================================

// --- Helper macros ----------------------------------------------------------
// Verify trace pointer is not null, checked only under RN_DEBUG
//
#if defined(RN_DEBUG) && !defined(XDP_TRACE_DISABLE)
// RN_DEBUG defined, check trace != NULL
// When trace is null don't use RN_ASSERT because:
//   1. Don't include Error.h
//   2. Reduce code size, RN_ASSERT generate big coe
// Instead, we call a static functions to do the job
//
#define _XDP_TRACE_CHECK_NULL_TRACE(trace)                                    \
                if ( (trace) == NULL ) {                                      \
                    XdpTrace::_XdpWriteEntryNullTraceP();                     \
                }                                                             \

#else
    // No RN_DEBUG, do nothing
#define _XDP_TRACE_CHECK_NULL_TRACE(trace)
#endif // RN_DEBUG

#ifdef XDP_TRACE_DISABLE
#define XDP_TRACE0(trace,color,str)
#define XDP_TRACE1(trace,color,str,p0)
#define XDP_TRACE2(trace,color,str,p0,p1)
#define XDP_TRACE3(trace,color,str,p0,p1,p2)
#define XDP_TRACE4(trace,color,str,p0,p1,p2,p3)
#define XDP_TRACE5(trace,color,str,p0,p1,p2,p3,p4)
#define XDP_TRACE6(trace,color,str,p0,p1,p2,p3,p4,p5)
#define XDP_TRACE7(trace,color,str,p0,p1,p2,p3,p4,p5,p6)
#define XDP_TRACE8(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7)
#define XDP_TRACE9(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8)
#define XDP_TRACE10(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9)
#define XDP_TRACE11(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10)
#define XDP_TRACE12(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11)
#define XDP_TRACE13(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12)
#define XDP_TRACE14(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13)
#define XDP_TRACE15(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14)
#define XDP_TRACE16(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15)

#define XDP_TRACE_ENH0(trace,color,appTag,str)
#define XDP_TRACE_ENH1(trace,color,appTag,str,p0)
#define XDP_TRACE_ENH2(trace,color,appTag,str,p0,p1)
#define XDP_TRACE_ENH3(trace,color,appTag,str,p0,p1,p2)
#define XDP_TRACE_ENH4(trace,color,appTag,str,p0,p1,p2,p3)
#define XDP_TRACE_ENH5(trace,color,appTag,str,p0,p1,p2,p3,p4)
#define XDP_TRACE_ENH6(trace,color,appTag,str,p0,p1,p2,p3,p4,p5)
#define XDP_TRACE_ENH7(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6)
#define XDP_TRACE_ENH8(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7)
#define XDP_TRACE_ENH9(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8)
#define XDP_TRACE_ENH10(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9)
#define XDP_TRACE_ENH11(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10)
#define XDP_TRACE_ENH12(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11)
#define XDP_TRACE_ENH13(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12)
#define XDP_TRACE_ENH14(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13)
#define XDP_TRACE_ENH15(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14)
#define XDP_TRACE_ENH16(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15)


#else // XDP_TRACE_DISABLE

// the next defines (both normal and enhanced traces) checks if the color and
// the application tag of the trace are enabled in the xdp trave object,
// if they are it call the right WriteEntry method and if not it icreases
// a counter of the discarded traces.

#define XDP_TRACE0(trace,color,str) {                                         \
          if ( !XdpTrace::StaticGetIsXDPDisabled() ){                         \
          _XDP_TRACE_CHECK_NULL_TRACE(trace);                                 \
            if ((((trace)->_ItsActiveColors[XDP_TRACE_APP_DEFAULT - 1] & (1<<(color)) ) != 0 ) \
                && (((trace)->_ItsActiveAppTags & (1<<(XDP_TRACE_APP_DEFAULT)) ) != 0 )  ) {\
                (trace)->_WriteEntry0((UINT16)(color),(UINT16)(XDP_TRACE_APP_DEFAULT), __LINE__,              \
                    __FILE__ "~" ## str);                                  \
			}                                                                 \
			else {                                                            \
				(trace)->_ItsNonActiveDiscardCounter++;                       \
			}                                                                 \
	      }}                                                                  \

#define XDP_TRACE1(trace,color,str,p0)                                        \
	        XDP_TRACE4(trace,color,str,p0,0,0,0)                              \

#define XDP_TRACE2(trace,color,str,p0,p1)                                     \
	        XDP_TRACE4(trace,color,str,p0,p1,0,0)                             \

#define XDP_TRACE3(trace,color,str,p0,p1,p2)                                  \
	        XDP_TRACE4(trace,color,str,p0,p1,p2,0)                            \

#define XDP_TRACE4(trace,color,str,p0,p1,p2,p3) {                             \
          if ( !XdpTrace::StaticGetIsXDPDisabled() ){                         \
          _XDP_TRACE_CHECK_NULL_TRACE(trace);                                 \
            if ((((trace)->_ItsActiveColors[XDP_TRACE_APP_DEFAULT - 1] & (1<<(color)) ) != 0 ) \
                && (((trace)->_ItsActiveAppTags & (1<<(XDP_TRACE_APP_DEFAULT)) ) != 0 )  ) {   \
                (trace)->_WriteEntry4((UINT16)(color),(UINT16)(XDP_TRACE_APP_DEFAULT), __LINE__,    \
                    __FILE__ "~" ## str,                                   \
					(UINT32)(p0),(UINT32)(p1),(UINT32)(p2),(UINT32)(p3));     \
			}                                                                 \
			else {                                                            \
				(trace)->_ItsNonActiveDiscardCounter++;                       \
			}                                                                 \
		}}                                                                     \

#define XDP_TRACE5(trace,color,str,p0,p1,p2,p3,p4)                            \
	        XDP_TRACE8(trace,color,str,p0,p1,p2,p3,p4,0,0,0)                  \

#define XDP_TRACE6(trace,color,str,p0,p1,p2,p3,p4,p5)                         \
	        XDP_TRACE8(trace,color,str,p0,p1,p2,p3,p4,p5,0,0)                 \

#define XDP_TRACE7(trace,color,str,p0,p1,p2,p3,p4,p5,p6)                      \
	        XDP_TRACE8(trace,color,str,p0,p1,p2,p3,p4,p5,p6,0)                \

#define XDP_TRACE8(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7) {                 \
          if ( !XdpTrace::StaticGetIsXDPDisabled() ){                         \
          _XDP_TRACE_CHECK_NULL_TRACE(trace);                                 \
            if ((((trace)->_ItsActiveColors[XDP_TRACE_APP_DEFAULT - 1] & (1<<(color)) ) != 0 ) \
                && (((trace)->_ItsActiveAppTags & (1<<(XDP_TRACE_APP_DEFAULT)) ) != 0 )  ) {   \
                    (trace)->_WriteEntry8((UINT16)(color),XDP_TRACE_APP_DEFAULT, __LINE__, \
                    __FILE__ "~" ## str,                                       \
                     (UINT32)(p0),(UINT32)(p1),(UINT32)(p2),(UINT32)(p3),      \
                     (UINT32)(p4),(UINT32)(p5),(UINT32)(p6),(UINT32)(p7));     \
			}                                                                 \
			else {                                                            \
				(trace)->_ItsNonActiveDiscardCounter++;                       \
			}                                                                 \
		} }                                                                    \

#define XDP_TRACE9(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8)                \
	        XDP_TRACE11(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,0,0)       \

#define XDP_TRACE10(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9)            \
	        XDP_TRACE11(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,0)      \

#define XDP_TRACE11(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10) {      \
          if ( !XdpTrace::StaticGetIsXDPDisabled() ){                         \
          _XDP_TRACE_CHECK_NULL_TRACE(trace);                                 \
            if ((((trace)->_ItsActiveColors[XDP_TRACE_APP_DEFAULT - 1] & (1<<(color)) ) != 0 ) \
                && (((trace)->_ItsActiveAppTags & (1<<(XDP_TRACE_APP_DEFAULT)) ) != 0 )  ) {  \
            (trace)->_WriteEntry11((UINT16)(color),(UINT16)(XDP_TRACE_APP_DEFAULT), __LINE__,                     \
                    __FILE__ "~" ## str,                                   \
					(UINT32)(p0),(UINT32)(p1),(UINT32)(p2),(UINT32)(p3),      \
					(UINT32)(p4),(UINT32)(p5),(UINT32)(p6),(UINT32)(p7),      \
					(UINT32)(p8),(UINT32)(p9),(UINT32)(p10));                 \
			}                                                                 \
			else {                                                            \
				(trace)->_ItsNonActiveDiscardCounter++;                       \
			}                                                                 \
		} }                                                                    \

#define XDP_TRACE12(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11)    \
          XDP_TRACE16(trace,color,str,                                        \
                      p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,0,0,0,0)          \

#define XDP_TRACE13(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12) \
	      XDP_TRACE16(trace,color,str,                                        \
                      p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,0,0,0)        \

#define XDP_TRACE14(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13) \
	      XDP_TRACE16(trace,color,str,                                        \
                      p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,0,0)      \

#define XDP_TRACE15(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14) \
	      XDP_TRACE16(trace,color,str,                                        \
                      p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,0)    \

#define XDP_TRACE16(trace,color,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15) { \
          if ( !XdpTrace::StaticGetIsXDPDisabled() ){                         \
          _XDP_TRACE_CHECK_NULL_TRACE(trace);                                 \
            if ((((trace)->_ItsActiveColors[XDP_TRACE_APP_DEFAULT - 1] & (1<<(color)) ) != 0 ) \
                && (((trace)->_ItsActiveAppTags & (1<<(XDP_TRACE_APP_DEFAULT)) ) != 0 )  ) {  \
            (trace)->_WriteEntry16((UINT16)(color),(UINT16)(XDP_TRACE_APP_DEFAULT), __LINE__,                     \
                    __FILE__ "~" ## str,                                   \
					(UINT32)(p0),(UINT32)(p1),(UINT32)(p2),(UINT32)(p3),      \
					(UINT32)(p4),(UINT32)(p5),(UINT32)(p6),(UINT32)(p7),      \
					(UINT32)(p8),(UINT32)(p9),(UINT32)(p10),(UINT32)(p11),    \
					(UINT32)(p12),(UINT32)(p13),(UINT32)(p14),(UINT32)(p15)); \
			}                                                                 \
			else {                                                            \
				(trace)->_ItsNonActiveDiscardCounter++;                       \
			}                                                                 \
		}}                                                                     \

// --- XDP TRACE ENHANCED - Second/Application filter -------------------------


#define XDP_TRACE_ENH0(trace,color,appTag,str) {                                         \
          _XDP_TRACE_CHECK_NULL_TRACE(trace);                                 \
            if ((((trace)->_ItsActiveColors[appTag - 1] & (1<<(color)) ) != 0 ) \
                && (((trace)->_ItsActiveAppTags & (1<<(appTag)) ) != 0 )  ) {\
                (trace)->_WriteEntry0((UINT16)(color),(UINT16)(appTag), __LINE__,                      \
                    __FILE__ "~" ## str);                                  \
			}                                                                 \
			else {                                                            \
				(trace)->_ItsNonActiveDiscardCounter++;                       \
			}                                                                 \
		  }                                                                   \

#define XDP_TRACE_ENH1(trace,color,appTag,str,p0)                                        \
	        XDP_TRACE_ENH4(trace,color,appTag,str,p0,0,0,0)                              \

#define XDP_TRACE_ENH2(trace,color,appTag,str,p0,p1)                                     \
	        XDP_TRACE_ENH4(trace,color,appTag,str,p0,p1,0,0)                             \

#define XDP_TRACE_ENH3(trace,color,appTag,str,p0,p1,p2)                                  \
	        XDP_TRACE_ENH4(trace,color,appTag,str,p0,p1,p2,0)                            \

#define XDP_TRACE_ENH4(trace,color,appTag,str,p0,p1,p2,p3) {                             \
          _XDP_TRACE_CHECK_NULL_TRACE(trace);                                 \
            if ((((trace)->_ItsActiveColors[appTag - 1] & (1<<(color)) ) != 0 ) \
                && (((trace)->_ItsActiveAppTags & (1<<(appTag)) ) != 0 )  ) {\
                    (trace)->_WriteEntry4((UINT16)(color),(UINT16)(appTag), __LINE__,                     \
                    __FILE__ "~" ## str,                                   \
					(UINT32)(p0),(UINT32)(p1),(UINT32)(p2),(UINT32)(p3));     \
			}                                                                 \
			else {                                                            \
				(trace)->_ItsNonActiveDiscardCounter++;                       \
			}                                                                 \
		}                                                                     \

#define XDP_TRACE_ENH5(trace,color,appTag,str,p0,p1,p2,p3,p4)                            \
	        XDP_TRACE_ENH8(trace,color,appTag,str,p0,p1,p2,p3,p4,0,0,0)                  \

#define XDP_TRACE_ENH6(trace,color,appTag,str,p0,p1,p2,p3,p4,p5)                         \
	        XDP_TRACE_ENH8(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,0,0)                 \

#define XDP_TRACE_ENH7(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6)                      \
	        XDP_TRACE_ENH8(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,0)                \

#define XDP_TRACE_ENH8(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7) {                 \
          _XDP_TRACE_CHECK_NULL_TRACE(trace);                                 \
            if ((((trace)->_ItsActiveColors[appTag - 1] & (1<<(color)) ) != 0 ) \
                && (((trace)->_ItsActiveAppTags & (1<<(appTag)) ) != 0 )  ) {\
                    (trace)->_WriteEntry8((UINT16)(color),(UINT16)(appTag), __LINE__,                     \
                    __FILE__ "~" ## str,                                   \
					(UINT32)(p0),(UINT32)(p1),(UINT32)(p2),(UINT32)(p3),      \
					(UINT32)(p4),(UINT32)(p5),(UINT32)(p6),(UINT32)(p7));     \
			}                                                                 \
			else {                                                            \
				(trace)->_ItsNonActiveDiscardCounter++;                       \
			}                                                                 \
		}                                                                     \


#define XDP_TRACE_ENH9(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8)                \
	        XDP_TRACE_ENH11(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,0,0)       \

#define XDP_TRACE_ENH10(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9)            \
	        XDP_TRACE_ENH11(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,0)      \

#define XDP_TRACE_ENH11(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10) {      \
          _XDP_TRACE_CHECK_NULL_TRACE(trace);                                 \
            if ((((trace)->_ItsActiveColors[appTag - 1] & (1<<(color)) ) != 0 ) \
                && (((trace)->_ItsActiveAppTags & (1<<(appTag)) ) != 0 )  ) {\
                    (trace)->_WriteEntry11((UINT16)(color),(UINT16)(appTag), __LINE__,                     \
                    __FILE__ "~" ## str,                                   \
					(UINT32)(p0),(UINT32)(p1),(UINT32)(p2),(UINT32)(p3),      \
					(UINT32)(p4),(UINT32)(p5),(UINT32)(p6),(UINT32)(p7),      \
					(UINT32)(p8),(UINT32)(p9),(UINT32)(p10));                 \
    		}                                                                 \
    		else {                                                            \
    			(trace)->_ItsNonActiveDiscardCounter++;                       \
    		}                                                                 \
		}                                                                     \

#define XDP_TRACE_ENH12(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11)    \
          XDP_TRACE_ENH16(trace,color,appTag,str,                                        \
                      p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,0,0,0,0)          \

#define XDP_TRACE_ENH13(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12) \
	      XDP_TRACE_ENH16(trace,color,appTag,str,                                        \
                      p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,0,0,0)        \

#define XDP_TRACE_ENH14(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13) \
	      XDP_TRACE_ENH16(trace,color,appTag,str,                                        \
                      p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,0,0)      \

#define XDP_TRACE_ENH15(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14) \
	      XDP_TRACE_ENH16(trace,color,appTag,str,                                        \
                      p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,0)    \


#define XDP_TRACE_ENH16(trace,color,appTag,str,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15) { \
          _XDP_TRACE_CHECK_NULL_TRACE(trace);                                 \
            if ((((trace)->_ItsActiveColors[appTag - 1] & (1<<(color)) ) != 0 ) \
                && (((trace)->_ItsActiveAppTags & (1<<(appTag)) ) != 0 )  ) {  \
            (trace)->_WriteEntry16((UINT16)(color),(UINT16)(appTag), __LINE__,                     \
                    __FILE__ "~" ## str,                                   \
		    		(UINT32)(p0),(UINT32)(p1),(UINT32)(p2),(UINT32)(p3),      \
					(UINT32)(p4),(UINT32)(p5),(UINT32)(p6),(UINT32)(p7),      \
					(UINT32)(p8),(UINT32)(p9),(UINT32)(p10),(UINT32)(p11),    \
					(UINT32)(p12),(UINT32)(p13),(UINT32)(p14),(UINT32)(p15)); \
			}                                                                 \
			else {                                                            \
				(trace)->_ItsNonActiveDiscardCounter++;                       \
			}                                                                 \
		}                                                                     \


#endif //XDP_TRACE_DISABLE
//-- Xdp name max size
#define XDP_TRACE_NAME_MAX_LEN 50

// --- XdpTraceColor (enum) ---------------------------------------------------

enum XdpTraceColor
     { XDP_TRACE_COLOR_NONE = 0x0000,  // Not used
       XDP_TRACE_COLOR_FIRST= 0x0001,  // Color loop helper
       XDP_TRACE_WHITE      = 0x0001,  // Very minor events (e.g. register access)
       XDP_TRACE_GREEN      = 0x0002,  // Free running events (e.g. tick, "periodics")
	   XDP_TRACE_YELLOW     = 0x0003,  // Externaly (HW) initiated events (e.g. port down)
	   XDP_TRACE_ORANGE     = 0x0004,  // User initiated events (e.g. configuration)
	   XDP_TRACE_RED        = 0x0005,  // Execpetions (e.g. BIT failure)
	   XDP_TRACE_BLACK      = 0x0006,  // Exterm execptions (e.g fatal)

       XDP_TRACE_COLOR_LAST = 0x0006,  // Some helpers
       XDP_TRACE_COLOR_MASK = 0x000f};
	
// --- XdpTraceAppTag (enum) ---------------------------------------------------

enum XdpTraceAppTag
    { XDP_TRACE_APP_NONE    = 0x0000,
      XDP_TRACE_APP_FIRST   = 0x0001,  //Helpers tags
      XDP_TRACE_APP_DEFAULT = 0x0001,  //The default tag
      XDP_TRACE_APP1        = 0x0002,
      XDP_TRACE_APP2        = 0x0003,
      XDP_TRACE_APP3        = 0x0004,
      XDP_TRACE_APP4        = 0x0005,
      XDP_TRACE_APP5        = 0x0006,
      XDP_TRACE_APP6        = 0x0007,
      XDP_TRACE_APP7        = 0x0008,
      XDP_TRACE_APP8        = 0x0009,
      XDP_TRACE_APP9        = 0x000a,
      XDP_TRACE_APP10       = 0x000b,
      XDP_TRACE_APP11       = 0x000c,
      XDP_TRACE_APP12       = 0x000d,
      XDP_TRACE_APP13       = 0x000e,
      XDP_TRACE_APP14       = 0x000f,

      XDP_TRACE_APP_LAST    = 0x000f,
      XDP_TRACE_APP_MASK    = 0x000f,
      XDP_TRACE_APP_ALL     = 0x0010};  //Helpers tags


// ****************************************************************************
// ***
// *** XdpTrace class declaration
// ***
// ****************************************************************************

class CmdMngr;
class XdpTraceUI;
class XdpServices;

class XdpTrace : public MonApplic

{

    friend class XdpTraceUI;
    friend class XdpTraceCmdMngr;

// ****************************************************************************
// ****************************************************************************
// ***
// *** public
// ***
// ****************************************************************************
// ****************************************************************************
public:

// ============================================================================
// ===
// === Enums (public)
// ===
// ============================================================================

// --- XdpTraceState ----------------------------------------------------------
//
// Enum for buffer state
//
    enum XdpTraceState {
          XDP_TRACE_STATE_OFF,
          XDP_TRACE_STATE_ON,
          XDP_TRACE_STATE_RECOVER,
          XDP_TRACE_STATE_LAST
    };

// --- XdpTraceFormat ---------------------------------------------------------
//
// Enum for formating fields
//
    enum XdpTraceFormat  {
          XDP_TRACE_FORMAT_OFF,
          XDP_TRACE_FORMAT_ON,
          XDP_TRACE_FORMAT_LONG,
          XDP_TRACE_FORMAT_HEX,
          XDP_TRACE_FORMAT_DEC,
          XDP_TRACE_FORMAT_HEX8,
          XDP_TRACE_FORMAT_INT8,
          XDP_TRACE_FORMAT_UINT8,
          XDP_TRACE_FORMAT_HEX16,
          XDP_TRACE_FORMAT_INT16,
          XDP_TRACE_FORMAT_UINT16,
          XDP_TRACE_FORMAT_HEX32,
          XDP_TRACE_FORMAT_INT32,
          XDP_TRACE_FORMAT_UINT32,
          XDP_TRACE_FORMAT_APP1,
          XDP_TRACE_FORMAT_APP2,
          XDP_TRACE_FORMAT_APP3,
          XDP_TRACE_FORMAT_NEXT   // Must be the last one
    };

// --- XdpTraceFormatField ----------------------------------------------------
//
// Enum of the formatted fields
//

    enum XdpTraceFormatField {
        XDP_TRACE_FORMAT_FIELD_SEQ,
        XDP_TRACE_FORMAT_FIELD_NAME,
        XDP_TRACE_FORMAT_FIELD_ENTRY_NUM,
        XDP_TRACE_FORMAT_FIELD_COLOR,
        XDP_TRACE_FORMAT_FIELD_APP_TAG, // Application tag
        XDP_TRACE_FORMAT_FIELD_TASK,
        XDP_TRACE_FORMAT_FIELD_TIME,
        XDP_TRACE_FORMAT_FIELD_FILE,
        XDP_TRACE_FORMAT_FIELD_FMT_STRING,
        XDP_TRACE_FORMAT_FIELD_USER_PARAMS,
        XDP_TRACE_FORMAT_FIELD_USER_BYTES,
        XDP_TRACE_FORMAT_FIELD_NEXT           // Must be the last one
   };


// --- XdpTraceOutput ---------------------------------------------------------
//
// Enum output channel
//
    enum XdpTraceOutput {
          XDP_TRACE_OUTPUT_CMDMNGR,
          XDP_TRACE_OUTPUT_UDP,
          XDP_TRACE_OUTPUT_SERIAL,
          XDP_TRACE_OUTPUT_FILE,
          XDP_TRACE_OUTPUT_NEXT
    };

// --- XdpTraceBuffer ---------------------------------------------------------
//
// Enum buffer type
//
    enum XdpTraceBuffer {
          XDP_TRACE_BUFFER_HEAP,
          XDP_TRACE_BUFFER_EXT_SRAM,
          XDP_TRACE_BUFFER_CPU_SRAM,
          XDP_TRACE_BUFFER_NEXT
    };

// ============================================================================
// ===
// === Constructor & Destructor (public)
// ===
// ============================================================================

    XdpTrace(const RWCString &theLongName, const RWCString &theShortName) ;
// ============================================================================
// ===
// === Write entries (public)
// ===
// ============================================================================

// All the write functions should not be used directly, use the XDP_TRACE macros !!

	void _WriteEntry0(UINT16 theColor,UINT16 theAppTag, UINT16 theLineNumber, const char *theFormatString);

	void _WriteEntry4(UINT16 theColor,UINT16 theAppTag, UINT16 theLineNumber, const char *theFormatString,
		              UINT32 theP0, UINT32 theP1=0, UINT32 theP2=0, UINT32 theP3 = 0);

	void _WriteEntry8(UINT16 theColor,UINT16 theAppTag, UINT16 theLineNumber, const char *theFormatString,
		              UINT32 theP0, UINT32 theP1,   UINT32 theP2,   UINT32 theP3,
  		              UINT32 theP4, UINT32 theP5=0, UINT32 theP6=0, UINT32 theP7 = 0);

	void _WriteEntry11(UINT16 theColor,UINT16 theAppTag, UINT16 theLineNumber, const char *theFormatString,
		               UINT32 theP0, UINT32 theP1,   UINT32 theP2,   UINT32 theP3,
		               UINT32 theP4, UINT32 theP5,   UINT32 theP6,   UINT32 theP7,
		               UINT32 theP8, UINT32 theP9=0, UINT32 theP10=0);

	void _WriteEntry16(UINT16 theColor,UINT16 theAppTag, UINT16 theLineNumber, const char *theFormatString,
		               UINT32 theP0, UINT32 theP1,   UINT32 theP2,   UINT32 theP3,
		               UINT32 theP4, UINT32 theP5,   UINT32 theP6,   UINT32 theP7,
		               UINT32 theP8, UINT32 theP9,   UINT32 theP10,  UINT32 theP11,
		               UINT32 theP12,UINT32 theP13=0,UINT32 theP14=0, UINT32 theP15 = 0);

    static void _XdpWriteEntryNullTraceP(); // Handle write entry that got NULL trace

// ============================================================================
// ===
// === Display & output (public)
// ===
// ============================================================================


        bool SetOutputChannel(XdpTrace::XdpTraceOutput theOutput);
        bool SetOutputReliefNumber(UINT32 theReliefNumber);


// ============================================================================
// ===
// === Buffer state (public)
// ===
// ============================================================================

    bool            On();
    bool            Off();
    bool            Start();
    bool            Stop();
    bool            Resume();

// ============================================================================
// ===
// === Buffer control (public)
// ===
// ============================================================================

    // Buffer configutation

    bool           SetBufferSizeAndType( UINT32 theBufferSizeInK, XdpTraceBuffer theBufferType);

    // Cyclic Flag

    bool           SetCyclicFlag(bool theCyclicFlag);

// ============================================================================
// ===
// === Task & Time (public)
// ===
// ============================================================================

    bool   SetTimeStampResolution(double theRequiredResolution);

// ============================================================================
// ===
// === Color functions (public)
// ===
// ============================================================================

    bool           SetColor      (bool theEnableFlag, UINT32 theColor, UINT32 theAppTag = XDP_TRACE_APP_ALL);

    bool           SetAboveColors(bool theEnableFlag, UINT32 theColor, UINT32 theAppTag = XDP_TRACE_APP_ALL);

    bool           SetBelowColors(bool theEnableFlag, UINT32 theColor, UINT32 theAppTag = XDP_TRACE_APP_ALL);

    bool           IsAllColorsDisabled    (UINT32 theColorMask[]);
// ============================================================================
// ===
// === Format functions (public)
// ===
// ============================================================================

    bool           SetFormat(XdpTraceFormatField theFormatField, XdpTraceFormat theFormat);


// ============================================================================
// ===
// === Print functions (public)
// ===
// ============================================================================

    bool           SetPrintColor      (bool theEnableFlag, UINT32 theColor);
    bool           SetPrintColor      (bool theEnableFlag, UINT32 theColor, UINT32 theAppTag);

    bool           SetPrintAboveColors(bool theEnableFlag, UINT32 theColor);
    bool           SetPrintAboveColors(bool theEnableFlag, UINT32 theColor, UINT32 theAppTag);

    bool           SetPrintBelowColors(bool theEnableFlag, UINT32 theColor);
    bool           SetPrintBelowColors(bool theEnableFlag, UINT32 theColor, UINT32 theAppTag);

    bool           SetPrintOutputChannel(XdpTrace::XdpTraceOutput theOutput);

    static void    StaticDisableXDP()
    {
        TheirIsXDPDisabled = true;
    }

    static BOOL32  StaticGetIsXDPDisabled()
    {
        return TheirIsXDPDisabled;
    }

// ============================================================================
// ===
// === Configuration save and restore functions (public)
// ===
// ============================================================================

   bool           RestoreConfig(const UINT32  theEnabledAppTags,
                                const UINT32* theEnabledColorsP,
                                const UINT32  thePrintActiveAppTags,
                                const UINT32* thePrintActiveColorsP,
                                const XdpTrace::XdpTraceOutput thePrintOutputChannel);

   bool         SaveConfig();
   bool         ShowTraceConfig();



// ============================================================================
// ===
// === User interface functions (public)
// ===
// ============================================================================

    // Entry function for applications commmand managers
    void ProcessUI(CmdMngr *theCmdMngr);

    // For MonApplic
    virtual RWCString GetYourId();

// ============================================================================
// ===
// === Data members (public)
// ===
// ============================================================================


	// Those data members are public to enable using the in the XDP_TRACE access macros

	UINT32 _ItsNonActiveDiscardCounter; // Discard due to inactive color counter
	
    UINT32 _ItsActiveColors[XDP_TRACE_APP_LAST];        // Map of the active color (both buffer and print) for each application

    UINT32 _ItsActiveAppTags;           // Bit map of the active application tags (both buffer and print)

// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ***
// *** protected
// ***
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
protected:

// ============================================================================
// ===
// === enums (protected)
// ===
// ============================================================================

// --- Recored & entry structure ----------------------------------------------

    // Application tag in first record header

    enum {XDP_TRACE_APP_SHIFT        = 7};

    // Record flags
    enum {XDP_TRACE_RECORD_VALID     = 0x0010,  // Valid/invalid record
          XDP_TRACE_RECORD_FIRST     = 0x0020,  // First record (but not last) of entry
          XDP_TRACE_RECORD_LAST      = 0x0040   // Last record of multiple records entry
	};

    // Line number pos in first record header
    enum {XDP_TRACE_LINE_NUM_SHIFT   = 16,
          XDP_TRACE_LINE_NUM_MASK = 0xffff} ;

    // Number of UINT32-s in single record
  	enum {XDP_TRACE_UINT32_IN_RECORD = 8};

    // Up to 3 record of paramters in entry
    enum {XDP_TRACE_MAX_PARAM_RECORDS_IN_ENTERY = 3};

    // The maximum number of paramters in an entry
    enum {XDP_TRACE_MAX_PARAMS_IN_ENTERY =
        XDP_TRACE_MAX_PARAM_RECORDS_IN_ENTERY * (XDP_TRACE_UINT32_IN_RECORD - 1) };

    //  Location of specific fields in the parms array
    enum {XDP_TRACE_PARAMS_FMT_STRING  = 0,
          XDP_TRACE_PARAMS_TIME        = 1,
          XDP_TRACE_PARAMS_TASK        = 2,
          XDP_TRACE_PARAMS_USER        = 3}; // The first user param

// ============================================================================
// ===
// === Display and output functions (protected)
// ===
// ============================================================================

    bool Display(INT32 theCount, XdpTrace::XdpTraceOutput theOutput);
    bool Output(XdpTrace::XdpTraceOutput theOutput, const char *theString);

    INT32           GetDefaultDisplayCount() { return ItsDefaultDisplayCount; };

    XdpTraceOutput  GetOutputChannel()       { return ItsOutputChannel; };


    bool            SetDefaultDisplayCount(INT32 theVal){ ItsDefaultDisplayCount = theVal;
                                                          return true; };

    UINT32          GetOutputReliefNumber() { return ItsOutputReliefNumber; } ;

    bool            VerifyOutputChannel(XdpTrace::XdpTraceOutput theOutput);

// ============================================================================
// ===
// === Cursor functions (protected)
// ===
// ============================================================================


    bool   SeekCursor(INT32 theEntryCount, bool *theEndOfBufferFlagP);
    bool   AbsoluteSeekCursor(UINT32 theEntryNumber, bool *theEndOfBufferFlagP);
    bool   SeekCursorToEnd();

    bool   IsCursorValid();
    bool   VerifyCursorValid();

    bool   GetCursorEntryNumber(UINT32 *theEntryNumberP);

// ============================================================================
// ===
// === Color functions (protected)
// ===
// ============================================================================

    UINT32*  GetEnableColors()        { return ItsEnabledColors; };
    bool     UpdateEnabledColors    (bool theEnableFlag, UINT32 theColorMask, UINT32 theAppTag);
	void     SetBufferActiveColors  (UINT32 theNewColors[]);
    void     SetBufferActiveColors  (UINT32 theNewColors);
    bool     VerifyColor            (UINT32 theColor);

// ============================================================================
// ===
// === Application tags functions (protected)
// ===
// ============================================================================

    UINT32   GetEnableAppTags()       { return ItsEnabledAppTags; };
    UINT32   GetPrintAppTags()        { return ItsPrintActiveAppTags; };
    bool     SetAppTag              (bool theEnableFlag, UINT32 theAppTag);
    bool     SetPrintAppTag         (bool theEnableFlag, UINT32 theAppTag);
    bool     UpdateEnabledAppTags   (bool theEnableFlag, UINT32 theAppTagMask);
    bool     UpdatePrintAppTags     (bool theEnableFlag, UINT32 theAppTagMask);
    void     SetPrintActiveAppTags  (UINT32 theNewAppTags);
    void     SetBufferActiveAppTags (UINT32 theNewAppTags);
    bool     VerifyAppTag           (UINT32 theAppTag);

// ============================================================================
// ===
// === Buffer control (protected)
// ===
// ============================================================================

    // Buffer configutation

    XdpTraceBuffer GetBufferType()    { return ItsBufferType; };
    UINT32         GetBufferSizeInK() { return ItsBufferSizeInK; };

    // Buffer state

    XdpTraceState   GetState()               { return ItsState; };

    // Verify state functions

    bool VerifyStateOff();
    bool VerifyStateOn();
    bool VerifyStateRecover();
    bool VerifyStateOffOrOn();
    bool VerifyStateOffOrRecover();
    bool VerifyStateOnOrRecover();

    // Cyclic Flag

    bool GetCyclicFlag()     { return ItsCyclicFlag; };

// ============================================================================
// ===
// === Print functions (protected)
// ===
// ============================================================================

    void PrintEntry(UINT32 theEntryHeader, // Includes color & line
                    UINT32 *theParamsP, UINT32 theParmasCount,
                    UINT8  *theBytesP,  UINT32 theBytesCount);

	void PrintOutput(const char *theStringP);

    UINT32*          GetPrintColors()        { return ItsPrintActiveColors; };

	void SetPrintActiveColors(UINT32 theNewColors[]);
    bool UpdatePrintColors   (bool theEnableFlag, UINT32 theColorMask, UINT32 theAppTag);

    XdpTraceOutput  GetPrintOutputChannel() { return ItsPrintOutputChannel; };


// ============================================================================
// ===
// === Record & Entry core function (protected)
// ===
// ============================================================================

	UINT32 *AllocateRecord();
    UINT32 *GetNextRecord(UINT32 *theRecordP, INT32 *theWrapCountP);
    UINT32 *GetPrevRecord(UINT32 *theRecordP, INT32 *theWrapCountP);

    bool   GetNextEntry(  UINT32 **theEntryP, INT32 *theWrapCountP);
    bool   GetPrevEntry(  UINT32 **theEntryP, INT32 *theWrapCountP);

    bool   ReadEntry(UINT32 *theEntryHeader, UINT32 *theParamsP, UINT32 *theParamCountP);
    UINT32 GetEntryColor(UINT32 theEntryHeader);
    UINT32 GetEntryAppTag(UINT32 theEntryHeader);
    UINT32 GetEntryLineNumber(UINT32 theEntryHeader);

    bool WasRecordEntryHeader(UINT32 *theRecordP);

    void   ZeroAllRecords();

// ============================================================================
// ===
// === Format functions (protected)
// ===
// ============================================================================

    // Format entry
    RWCString FormatEntry(UINT32 theSeqNum,   UINT32 theEntryNum,
                          UINT32 theEntryHeader, // Includes color & line
                          UINT32 *theParamsP, UINT32 theParmasCount,
                          UINT8  *theBytesP,  UINT32 theBytesCount);

    // Format field
    RWCString FormatName       ();
    RWCString FormatSeq        (UINT32 theSeqNum);
    RWCString FormatEntryNum   (UINT32 theEntryNum);
    RWCString FormatColor      (UINT32 theEntryHeader);
    RWCString FormatAppTag     (UINT32 theEntryHeader);
    RWCString FormatTask       (UINT32 theTaskStamp);
    RWCString FormatTime       (UINT32 theTimeStamp);
    RWCString FormatFile       (UINT32 theFmtString, UINT32 theEntryHeader);
    RWCString FormatFmtString  (UINT32 theFmtString);
    RWCString FormatUserParams (UINT32 theFmtString, UINT32 *theParamsP, UINT32 theCount);
    RWCString FormatUserBytes  (UINT8  *theBytesP, UINT32 theCount);

    // Application specific format functions
    virtual   RWCString FormatUserParamsApp1(UINT32 theFmtString, UINT32 *theParamsP, UINT32 theCount);
    virtual   RWCString FormatUserParamsApp2(UINT32 theFmtString, UINT32 *theParamsP, UINT32 theCount);
    virtual   RWCString FormatUserParamsApp3(UINT32 theFmtString, UINT32 *theParamsP, UINT32 theCount);
    //virtual   RWCString FormatUserBytesApp1 (UINT8  *theBytesP,  UINT32 theCount);
    //virtual   RWCString FormatUserBytesApp2 (UINT8  *theBytesP,  UINT32 theCount);
    //virtual   RWCString FormatUserBytesApp3 (UINT8  *theBytesP,  UINT32 theCount);


    // Get format
    XdpTraceFormat  GetFormatSeq()           { return ItsFormatSeq; };
    XdpTraceFormat  GetFormatName()          { return ItsFormatName; };
    XdpTraceFormat  GetFormatEntryNum()      { return ItsFormatEntryNum; };
    XdpTraceFormat  GetFormatColor()         { return ItsFormatColor; };
    XdpTraceFormat  GetFormatAppTag()        { return ItsFormatAppTag; };
    XdpTraceFormat  GetFormatTask()          { return ItsFormatTask; };
    XdpTraceFormat  GetFormatTime()          { return ItsFormatTime; };
    XdpTraceFormat  GetFormatFile()          { return ItsFormatFile; };
    XdpTraceFormat  GetFormatFmtString()     { return ItsFormatFmtString; };
    XdpTraceFormat  GetFormatUserParams()    { return ItsFormatUserParams; };
    XdpTraceFormat  GetFormatUserBytes()     { return ItsFormatUserBytes; };
    UINT32          GetFormatFileWidth()     { return ItsFormatFileWidth; };

    // Set format options. Parameter validation should be made by the caller
    bool SetFormatSeq       (XdpTraceFormat theVal) { ItsFormatSeq       =theVal; return true; };
    bool SetFormatName      (XdpTraceFormat theVal) { ItsFormatName      =theVal; return true; };
    bool SetFormatEntryNum  (XdpTraceFormat theVal) { ItsFormatEntryNum  =theVal; return true; };
    bool SetFormatColor     (XdpTraceFormat theVal) { ItsFormatColor     =theVal; return true; };
    bool SetFormatAppTag    (XdpTraceFormat theVal) { ItsFormatAppTag    =theVal; return true; };
    bool SetFormatTask      (XdpTraceFormat theVal) { ItsFormatTask      =theVal; return true; };
    bool SetFormatTime      (XdpTraceFormat theVal) { ItsFormatTime      =theVal; return true; };
    bool SetFormatFile      (XdpTraceFormat theVal) { ItsFormatFile      =theVal; return true; };
    bool SetFormatFmtString (XdpTraceFormat theVal) { ItsFormatFmtString =theVal; return true; };
    bool SetFormatUserParams(XdpTraceFormat theVal) { ItsFormatUserParams=theVal; return true; };
    bool SetFormatUserBytes (XdpTraceFormat theVal) { ItsFormatUserBytes =theVal; return true; };
    bool SetFormatFileWidth (UINT32         theVal) { ItsFormatFileWidth =theVal; return true; };

    // helpers
    UINT32 GetFmtStringFileNameLength(const char *theFmtString);
    const char *GetFmtStringOnly(const char *theFmtString);

    bool   VerifyFormat(XdpTraceFormat theFormat);
    bool   VerifyFormatField(XdpTraceFormatField theFormatField);

// ============================================================================
// ===
// === Task & Time (protected)
// ===
// ============================================================================

    double GetTimeStampRequiredResolution()    { return ItsTimeStampRequiredResolution; } ;
    double GetTimeStampRealResolution()        { return ItsTimeStampRealResolution; } ;

    double GetTimeOffset()                     { return ItsTimeOffset; };
    bool   SetTimeOffset(double theTimeOffset) { ItsTimeOffset  = theTimeOffset; return true; };

    double GetStartTime();

    double GetEntryAbsoluteTime(UINT32 theTimeStamp);
    double GetEntryTime(UINT32 theTimeStamp);

// ============================================================================
// ===
// === Relief functions (protected)
// ===
// ============================================================================

    void   SeekCursorTaskRelief();

// ============================================================================
// ===
// === To string functions (protected)
// ===
// ============================================================================

    // To string functions

    RWCString ToStr(XdpTraceState   theState,      INT32 theWidth=0);
    RWCString ToStr(XdpTraceColor   theColor,      INT32 theWidth=0);
    RWCString ToStr(XdpTraceAppTag  theAppTag,     INT32 theWidth=0);
    RWCString ToStr(XdpTraceOutput  theOutput,     INT32 theWidth=0);
    RWCString ToStr(XdpTraceFormat  theFormat,     INT32 theWidth=0);
    RWCString ToStr(XdpTraceFormatField theField,  INT32 theWidth=0);
    RWCString ToStr(XdpTraceBuffer  theBufferType, INT32 theWidth=0);

    RWCString ColorMaskToStr(UINT32 theColorMask[]);
    RWCString AppTagMaskToStr(UINT32 theAppTagMask);

// ============================================================================
// ===
// === Error & info functions (support functions) (protected)
// ===
// ============================================================================

    void      ClearStatusStrings();        // Clears the error & info strings
    RWCString GetStatusStrings();          // Returns the error & info strings

    void      SetErrorString(RWCString theError);     // Set the error string
    void      SetInfoString(RWCString theInfo);       // Set the info string



// ============================================================================
// ===
// === Data members (protected)
// ===
// ============================================================================

// --- Display & Output -------------------------------------------------------

    UINT32         ItsDefaultDisplayCount;

    XdpTraceOutput ItsOutputChannel;

    UINT32         ItsDisplaySeqNumber; // Display output sequence number

    // Display buffers
    // The display functions read entry into those buffers. Defined here to save stack
    UINT32         ItsDisplayParamsBuffer[XDP_TRACE_MAX_PARAMS_IN_ENTERY] ;
    //UINT8         ItsDisplayBytesBuffer[ ?? ] ;

    // Display relief

    UINT32 ItsOutputReliefNumber;   // Number of outputed entries between reliefs
    UINT32 ItsOutputReliefCounter;  // Count of outputed entries since last relief


// --- Colors -----------------------------------------------------------------

    UINT32 ItsPrintActiveColors[XDP_TRACE_APP_LAST];  // Map of the active color to print for each app tag

    UINT32 ItsBufferActiveColors[XDP_TRACE_APP_LAST];  // Map of the active color to write to buffer for each app tag

// --- Cursor -----------------------------------------------------------------

    UINT32 *ItsCursorEntryP;    // Pointer to the buffer entry of the cursor or NULL
    UINT32 ItsCursorEntryNumber;// Cursor entry number
    UINT32 ItsCursorWrapCounter;// Buffer wrap counter value when curson pointer has been set
                                // Used to validate cursor from buffer wrapping corraption


// --- Format -----------------------------------------------------------------

    XdpTraceFormat ItsFormatSeq;
    XdpTraceFormat ItsFormatName;
    XdpTraceFormat ItsFormatEntryNum;
    XdpTraceFormat ItsFormatColor;
    XdpTraceFormat ItsFormatAppTag; // application tag format
    XdpTraceFormat ItsFormatTask;
    XdpTraceFormat ItsFormatTime;
    XdpTraceFormat ItsFormatFile;
    XdpTraceFormat ItsFormatFmtString;
    XdpTraceFormat ItsFormatUserParams;
    XdpTraceFormat ItsFormatUserBytes;
    UINT32         ItsFormatFileWidth;

// --- Print ------------------------------------------------------------------

    // Print application tags
    UINT32 ItsPrintActiveAppTags;  // Bit map of the active app tags to print

    // Print output
    XdpTraceOutput ItsPrintOutputChannel;    // Print output channel

	// Print counters
	UINT32  ItsPrintSeqNumber;               // Print sequence number
	UINT32  ItsPrintInterruptDiscardCounter; // Number of print discarded due to interrupt
	UINT32  ItsPrintOutputErrorCounter;      // Number of errors occured while printing


// ---	Core buffer members --------------------------------------------------

    // Buffer pointers

   UINT32 *ItsBufferBaseP;   // Buffer first address
	UINT32 *ItsBufferEndP;    // Buffer end address. A record might use this address
	UINT32 *ItsBufferWrapP;   // Buffer wrap point, when the write pointer
							  // this point it wraps to the base. From the wrap
							  // point to the end of the buffer a room for at
	                          // least one entry plus and record must be resereved.
                              // The reason is that in non-cyclic buffer,
                              // the buffer is stopped when the write pointer crosess
                              // this point, but the current entry must have room in the
                              // buffer. Furthermore, the record after the last entry is
                              // marked as invalid. it must have room in the buffer also.
                              // The last record might use this address
	UINT32 *ItsBufferWriteP;  // Next entry write pointer

    // Buffer size & type

    UINT32 ItsBufferSizeInRecords; // Number of records in the buffer
    UINT32 ItsBufferSizeInK;       // Buffer size in K. 0 = no buffer
    XdpTraceBuffer ItsBufferType;  // Buffer memory type: Heap, CPU_SRAM, EXT_SRAM
    UINT32 ItsMaxRecordsInEntry;// Max number of records in entry (more that this buffer error)


    // Buffer application tags

    UINT32 ItsBufferActiveAppTags; // Bit map of the active app tags to write to buffer

    // Buffer colors

    UINT32 ItsEnabledColors[XDP_TRACE_APP_LAST];  // Map of the color enalbed by the user for each app tag

    // Application tags

    UINT32 ItsEnabledAppTags;      // Bit map of the app tags enabled by the user

	bool   ItsCyclicFlag;          // Cyclic buffer or non-cyclic buffer flag
	bool   ItsBufferActiveFlag;    // True when buffer is on and not stopped

    // Bufer state & status
    XdpTraceState   ItsState;
    INT32           ItsEntryCounter;    // The sequence number of the last entry in the buffer
	UINT32          ItsWrapCounter;     // The number of time the buffer has been wrapped (cyclic only)
	bool            ItsFullStopFlag;    // The buffer has been stopped due to non-cyclic buffer stop
	

// --- Information (Task & Time) ----------------------------------------------

    double ItsTimeStampRequiredResolution; // Time stamp required resultion as the user choose
    double ItsTimeStampRealResolution;     // Time stamp real resultion as used by the XDP service
    UINT32 ItsTimeStampParameter;          // Time stamp parameter, used by XDP time stamp service

    UINT32 ItsStartTimeStamp;              // Time stamp when the trace has been started
    double ItsTimeOffset;                  // Display time offset from the absolute time

// --- Relief -----------------------------------------------------------------

    UINT32 ItsSeekInterReliefNumber; // Number of seek iteration during cursor seek with int disable
    UINT32 ItsSeekTaskReliefNumber;  // Number of seek iteration during cursor seek without task relief

    UINT32 ItsSeekInterReliefCounter; // Interrupt reliefs in cursor seek counter
    UINT32 ItsSeekTaskReliefCounter;  // Calls to SeekCursorTaskRelief() in cursor seek counter

// --- User interface & XDP Services ------------------------------------------

    XdpTraceUI      *ItsUserInterfaceP;
    XdpServices     *ItsXdpServicesP;

// --- Status strings ---------------------------------------------------------

    RWCString ItsErrorString;    // Hold the last error string
    RWCString ItsInfoString;     // Hold the last info string

// --- Trace name -------------------------------------------------------------

    RWCString       ItsLongName;    // Trace full name for CmdMngr objects
    RWCString       ItsShortName;   // Trace name 2-3 letters for output

// ****************************************************************************
// ****************************************************************************
// ***
// *** private
// ***
// ****************************************************************************
// ****************************************************************************

private:

// ============================================================================
// ===
// === dtor, copy ctor, operator = (private)
// ===
// ============================================================================
//
// dtor, copy ctor & operator = are declared private and has no implemenation
// this procted from calling them

    // dtor
    ~XdpTrace();

    // Copy ctor
    XdpTrace(const XdpTrace &);

    // Operator =
    XdpTrace & operator=( const XdpTrace) ;

    static BOOL32 TheirIsXDPDisabled;

};


#endif // XdpTrace_h


