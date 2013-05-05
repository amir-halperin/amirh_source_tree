/* XdpTrace.cc - The core module of XDP trace  */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpTrace
   The core module of XDP Trace

AUTHOR
   Gaash, SB


**************************************************************************/

#include "XdpServices.h"  // should be first, other wise problems with WIN32
#include "prepc.h" // sprintf
#include "Error.h"
#include "XdpConv.h"
#include "XdpTrace.h"
#include "XdpTraceUI.h"
#include "XdpTraceCmdMngr.h"
#include "XdpConfigMngr.h"

#include "OSTimeUtil.h" // For Sleep

extern const MaskFlagId MASK_XDP;

BOOL32 XdpTrace::TheirIsXDPDisabled = false;

// --- macros -----------------------------------------------------------------

// Use XDP services macros to deal with interrups

#define XDP_TRACE_DISABLE_INTERRUPTS(intStatus) { XDP_SERVICES_INTERRUPTS_DISABLE(intStatus) };
#define XDP_TRACE_RESTORE_INTERRUPTS(intStatus) { XDP_SERVICES_INTERRUPTS_RESTORE(intStatus) };


// Use XDP services to retrive time stamp & task for trace

#define XDP_TRACE_WRITE_TIME_STAMP(p)  { (*p) = XdpServices::TimeStampGet(ItsTimeStampParameter); };
#define XDP_TRACE_WRITE_TASK(p)        { (*p) = XDP_SERVICES_TASK_STAMP_GET(); } ;

// ****************************************************************************
// ****************************************************************************
// ***
// *** public
// ***
// ****************************************************************************
// ****************************************************************************

// ============================================================================
// ===
// === Constructor & Destructor (public)
// ===
// ============================================================================

XdpTrace::XdpTrace(const RWCString &theLongName,const RWCString &theShortName) :
    ItsLongName(theLongName),
    ItsShortName(theShortName)
{
    RN_ASSERT( (theLongName.length() < XDP_TRACE_NAME_MAX_LEN) ,
              "The trace name should be less then 50 chars long !!");
    RN_TRACE(MASK_XDP, "ctor begin(" + XdpConv::ToHexStr(this) +
                        ") longName=" + theLongName +
                        " shortName=" + theShortName);

	UINT32 rec_num = 64 * 1024 / 32;  // 64 K buffer by default

    ItsMaxRecordsInEntry = 3; // upto 16 parameters

    ItsBufferSizeInRecords = rec_num;

    ItsBufferActiveFlag = false;

    UINT32 appTag;

  //  Initializing the colors for each application tag

 //   memset(ItsEnabledColors, '\0', 4 * (XDP_TRACE_APP_LAST + 1));

 //   memset(ItsBufferActiveColors, '\0', 4 * (XDP_TRACE_APP_LAST + 1));

 //   memset(ItsPrintActiveColors, '\0', 4 * (XDP_TRACE_APP_LAST + 1));

    for (appTag = XDP_TRACE_APP_FIRST - 1; appTag <= XDP_TRACE_APP_LAST - 1; appTag++)

    {

       ItsEnabledColors[appTag] = 0;

       ItsBufferActiveColors[appTag] = 0;

////////////////// Lioz
        ItsPrintActiveColors[appTag] = 0;

       _ItsActiveColors[appTag] = ItsPrintActiveColors[appTag] | ItsBufferActiveColors[appTag];

    }

    ItsEnabledAppTags = 1 << XDP_TRACE_APP_DEFAULT; // To enable the default tag

    ItsBufferActiveAppTags = 1 << XDP_TRACE_APP_DEFAULT; // To enable the default tag

    ItsPrintActiveAppTags = 1 << XDP_TRACE_APP_DEFAULT; // To enable the default tag

    _ItsActiveAppTags = ItsBufferActiveAppTags | ItsPrintActiveAppTags;

    ItsCyclicFlag = true;

    ItsBufferBaseP  = new UINT32[XDP_TRACE_UINT32_IN_RECORD * rec_num];
    ItsBufferSizeInK = 4 * XDP_TRACE_UINT32_IN_RECORD * rec_num / 1024;
    ItsBufferType    = XDP_TRACE_BUFFER_HEAP;


    // Cursor
    ItsCursorEntryP = NULL;

    // Error handling
    ItsErrorString = "";
    ItsInfoString = "";


    // Relief
    ItsSeekInterReliefNumber = 1000;
    ItsSeekTaskReliefNumber  = 10000;

    ItsSeekInterReliefCounter = 0;
    ItsSeekTaskReliefCounter  = 0;

    // Format

    ItsFormatName         = XDP_TRACE_FORMAT_OFF;
    ItsFormatSeq          = XDP_TRACE_FORMAT_OFF;
    ItsFormatEntryNum     = XDP_TRACE_FORMAT_ON;
    ItsFormatAppTag       = XDP_TRACE_FORMAT_LONG; // Application tag format
    ItsFormatColor        = XDP_TRACE_FORMAT_LONG;
    ItsFormatTask         = XDP_TRACE_FORMAT_ON;
    ItsFormatTime         = XDP_TRACE_FORMAT_ON;
    ItsFormatFile         = XDP_TRACE_FORMAT_ON;
    ItsFormatFmtString    = XDP_TRACE_FORMAT_OFF;
    ItsFormatUserParams   = XDP_TRACE_FORMAT_ON;
    ItsFormatUserBytes    = XDP_TRACE_FORMAT_OFF;


    ItsFormatFileWidth = 0;

    // Disp and output

    ItsDefaultDisplayCount = 5;

    ItsOutputChannel = XDP_TRACE_OUTPUT_CMDMNGR;

    ItsDisplaySeqNumber = 0;

    ItsOutputReliefNumber  = 100;
    ItsOutputReliefCounter = 0;

    // State

    ItsState = XDP_TRACE_STATE_OFF;

    // Time stamp - real init later

    ItsTimeStampRealResolution     = 0;
    ItsTimeStampRequiredResolution = 0;
    ItsTimeStampParameter          = 0;
    ItsStartTimeStamp              = 0;
    ItsTimeOffset                  = 0;

    // Build user interface

    ItsUserInterfaceP   = new XdpTraceUI;
    if (ItsUserInterfaceP == NULL) {
        REC_FATAL(17501);
    }

	// Print

	ItsPrintSeqNumber = 0;
    ItsPrintOutputChannel = XDP_TRACE_OUTPUT_UDP;

    ItsPrintInterruptDiscardCounter = 0;
    ItsPrintOutputErrorCounter      = 0;

    // Get XdpServices;

    ItsXdpServicesP = XdpServices::GetIt();
    if (ItsUserInterfaceP == NULL) {
        REC_FATAL(17501);
    }

    // Register to MonApplic
    CmdMngr *cmdMngrP = XdpTraceCmdMngr::Instance();
    if (cmdMngrP == NULL) {
        REC_FATAL(17501);
    }

    MonApplic::Register(cmdMngrP);

    XdpConfigMngr::GetIt()->RegisterTrace(this);
    // Setup time stamp

    SetTimeStampResolution(50e-6); // 50 uSec

    // Setup colors

//    SetAboveColors(true,XDP_TRACE_GREEN);
//    SetPrintAboveColors(true,XDP_TRACE_RED);

    // Turn the buffer on

    On();



    RN_TRACE(MASK_XDP, ItsLongName + " ctor (" + XdpConv::ToHexStr(this) +
                       ") Enabled colors per app tag:\n " + ColorMaskToStr(ItsEnabledColors));

    RN_TRACE(MASK_XDP, ItsLongName + " ctor (" + XdpConv::ToHexStr(this) +
                       ") Enabled print colors per app tag:\n " + ColorMaskToStr(ItsPrintActiveColors));

    RN_TRACE(MASK_XDP, ItsLongName + " ctor end (" + XdpConv::ToHexStr(this) + ")" );

}




bool XdpTrace::WasRecordEntryHeader(UINT32 *theRecordP){
    return (*theRecordP & XDP_TRACE_RECORD_FIRST);
}




// ============================================================================
// ===
// === Write entry functions (public)
// ===
// ============================================================================

// --- _WriteEntry0 -----------------------------------------------------------
void XdpTrace::_WriteEntry0(UINT16 theColor, UINT16 theAppTag, UINT16 theLineNumber, const char *theFormatString) {

      // Write entry to buffer if the application tag and the color is active
      if ( (ItsBufferActiveAppTags & (1<<theAppTag)) &&
           (ItsBufferActiveColors[theAppTag - 1] & (1<<theColor)) ) {

		UINT32 *p;

		// disable interrputs
        UINT32 interruptsStatus;
        XDP_TRACE_DISABLE_INTERRUPTS(interruptsStatus);

        // Allocate record for entry header (paramters space is not used)
        p = AllocateRecord();
        bool entryHeader = WasRecordEntryHeader(p);
        if (entryHeader)
        {
            ItsEntryCounter--;
        }

		// Save entry pointer and value. They will be the last entry write.
        // Before the write, the entry is zero according to buffer operation rules
        UINT32 *entryFlagsAndLineNumP = p;
        UINT32  entryFlagsAndLineNum  = theColor |
                                        XDP_TRACE_RECORD_VALID | // Record is valid
                                        XDP_TRACE_RECORD_FIRST | // Signle record entry
                                        XDP_TRACE_RECORD_LAST  | // first & last record
                                        theAppTag << XDP_TRACE_APP_SHIFT | // Application tag
                                        theLineNumber << XDP_TRACE_LINE_NUM_SHIFT; // Line #
		p++;

		// Write format String
		*p = (UINT32) theFormatString;
		p++;

		// Write time stamp
		XDP_TRACE_WRITE_TIME_STAMP(p);
		p++;

		// Write task
		XDP_TRACE_WRITE_TASK(p);
		p++;

		// Increase last entry number
		ItsEntryCounter++;


        // Mark the next entry as invalid
        *ItsBufferWriteP = 0;

        // Write this entry flags (and line number). This marks them as valid

        *entryFlagsAndLineNumP = entryFlagsAndLineNum;

		// restore interrputs
        XDP_TRACE_RESTORE_INTERRUPTS(interruptsStatus);


	}

      // Is application tag and color enabled to print
      if ( (ItsPrintActiveAppTags & (1<<theAppTag)) &&
           (ItsPrintActiveColors[theAppTag - 1] & (1<<theColor)) ) {
		// Yes, Prepare to print

		// Build array from parameters
		// Array is local becuase the function is reenetrant
		UINT32 paramArray[XDP_TRACE_PARAMS_USER + 0];
		
		// Foramt string
		paramArray[XDP_TRACE_PARAMS_FMT_STRING] = (UINT32) theFormatString;

		// Time stamp
		XDP_TRACE_WRITE_TIME_STAMP(&paramArray[XDP_TRACE_PARAMS_TIME]);

		// Task
		XDP_TRACE_WRITE_TASK      (&paramArray[XDP_TRACE_PARAMS_TASK]);

		// No user parameters

		// Build entry header for print, put just color ,application tag and line number since only those are used
        UINT32  printEntryFlagsAndLineNum  = theColor | (theAppTag << XDP_TRACE_APP_SHIFT) | theLineNumber << XDP_TRACE_LINE_NUM_SHIFT;

		// Print the entry

		PrintEntry(printEntryFlagsAndLineNum,  paramArray, XDP_TRACE_PARAMS_USER + 0, NULL, 0);

	} // if print
}


// --- _WriteEntry4 -----------------------------------------------------------
void XdpTrace::_WriteEntry4(UINT16 theColor, UINT16 theAppTag, UINT16 theLineNumber, const char *theFormatString,
				 UINT32 theP0, UINT32 theP1, UINT32 theP2, UINT32 theP3) {

    // Write entry to buffer if the application tag and color is active
    if ( (ItsBufferActiveAppTags & (1<<theAppTag)) &&
         (ItsBufferActiveColors[theAppTag - 1] & (1<<theColor)) ) {

		UINT32 *p;

		// disable interrputs
        UINT32 interruptsStatus;
        XDP_TRACE_DISABLE_INTERRUPTS(interruptsStatus);

        // Allocate record for entry header and 4 paramters
        p = AllocateRecord();
        bool entryHeader = WasRecordEntryHeader(p);
        if (entryHeader)
        {
            ItsEntryCounter--;
        }


		// Save entry pointer and value. They will be the last entry write.
        // Before the write, the entry is zero according to buffer operation rules
        UINT32 *entryFlagsAndLineNumP = p;
        UINT32  entryFlagsAndLineNum  = theColor |
                                        XDP_TRACE_RECORD_VALID | // Record is valid
                                        XDP_TRACE_RECORD_FIRST | // Signle record entry
                                        XDP_TRACE_RECORD_LAST  | // first & last record
                                        theAppTag << XDP_TRACE_APP_SHIFT | // Application tag
                                        theLineNumber << XDP_TRACE_LINE_NUM_SHIFT; // Line #
		p++;

		// Write format String
		*p = (UINT32) theFormatString;
		p++;

		// Write time stamp
		XDP_TRACE_WRITE_TIME_STAMP(p);
		p++;

		// Write task
		XDP_TRACE_WRITE_TASK(p);
		p++;

		// Write paramters
		*p = theP0; p++;
		*p = theP1; p++;
		*p = theP2; p++;
		*p = theP3; p++;

		// Increase last entry number
		ItsEntryCounter++;


        // Mark the next entry as invalid
        *ItsBufferWriteP = 0;

        // Write this entry flags (and line number). This marks them as valid

        *entryFlagsAndLineNumP = entryFlagsAndLineNum;

		// restore interrputs
        XDP_TRACE_RESTORE_INTERRUPTS(interruptsStatus);


	}

    // Is application tag and color enabled to print
    if ( (ItsPrintActiveAppTags & (1<<theAppTag)) &&
         (ItsPrintActiveColors[theAppTag - 1] & (1<<theColor)) ) {
		// Yes, Prepare to print

		// Build array from parameters
		// Array is local becuase the function is reenetrant
		UINT32 paramArray[XDP_TRACE_PARAMS_USER + 4];
		
		// Foramt string
		paramArray[XDP_TRACE_PARAMS_FMT_STRING] = (UINT32) theFormatString;

		// Time stamp
		XDP_TRACE_WRITE_TIME_STAMP(&paramArray[XDP_TRACE_PARAMS_TIME]);

		// Task
		XDP_TRACE_WRITE_TASK      (&paramArray[XDP_TRACE_PARAMS_TASK]);

		// User parameters
		paramArray[XDP_TRACE_PARAMS_USER + 0]  = theP0;
		paramArray[XDP_TRACE_PARAMS_USER + 1]  = theP1;
		paramArray[XDP_TRACE_PARAMS_USER + 2]  = theP2;
		paramArray[XDP_TRACE_PARAMS_USER + 3]  = theP3;
		
		// Build entry header for print, put just color, application tag and line number since only those are used
        UINT32  printEntryFlagsAndLineNum  = theColor | (theAppTag << XDP_TRACE_APP_SHIFT) | theLineNumber << XDP_TRACE_LINE_NUM_SHIFT;

		// Print the entry

		PrintEntry(printEntryFlagsAndLineNum, paramArray, XDP_TRACE_PARAMS_USER + 4, NULL, 0);

	} // if print
}

// --- _WriteEntry8 -----------------------------------------------------------
void XdpTrace::_WriteEntry8(UINT16 theColor, UINT16 theAppTag, UINT16 theLineNumber, const char *theFormatString,
				 UINT32 theP0, UINT32 theP1, UINT32 theP2, UINT32 theP3,
                 UINT32 theP4, UINT32 theP5, UINT32 theP6, UINT32 theP7) {

    // Write entry to buffer if the application tag and color is active
    if ( (ItsBufferActiveAppTags & (1<<theAppTag)) &&
         (ItsBufferActiveColors[theAppTag - 1] & (1<<theColor)) ) {

		UINT32 *p;

		// disable interrputs
        UINT32 interruptsStatus;
        XDP_TRACE_DISABLE_INTERRUPTS(interruptsStatus);

        // Allocate record for entry header and 4 paramters
		p = AllocateRecord();
        bool entryHeader = WasRecordEntryHeader(p);
        if (entryHeader)
        {
            ItsEntryCounter--;
        }


		// Save entry pointer and value. They will be the last entry write.
        // Before the write, the entry is zero according to buffer operation rules
        UINT32 *entryFlagsAndLineNumP = p;
        UINT32  entryFlagsAndLineNum  = theColor |
                                        XDP_TRACE_RECORD_VALID | // Record is valid
                                        XDP_TRACE_RECORD_FIRST | // first record
                                        theAppTag << XDP_TRACE_APP_SHIFT | // Application tag
                                        theLineNumber << XDP_TRACE_LINE_NUM_SHIFT;
		p++;

		// Write format String
		*p = (UINT32) theFormatString;
		p++;

		// Write time stamp
		XDP_TRACE_WRITE_TIME_STAMP(p);
		p++;

		// Write task
		XDP_TRACE_WRITE_TASK(p);
		p++;

		// Write paramters
		*p = theP0; p++;
		*p = theP1; p++;
		*p = theP2; p++;
		*p = theP3; p++;

        // Allocate second record for next 8 paramters (only 4 are used)
		p = AllocateRecord();
        entryHeader = WasRecordEntryHeader(p);
        if (entryHeader)
        {
            ItsEntryCounter--;
        }


        // Write second record header
        *p = XDP_TRACE_RECORD_VALID |  // Record is valid
             XDP_TRACE_RECORD_LAST;    // Last record in entry
		p++;

        // Write paramters
		*p = theP4; p++;
		*p = theP5; p++;
		*p = theP6; p++;
		*p = theP7; p++;

		// Increase last entry number
		ItsEntryCounter++;


        // Mark the next entry as invalid
        *ItsBufferWriteP = 0;

        // Write this entry flags (and line number). This marks them as valid

        *entryFlagsAndLineNumP = entryFlagsAndLineNum;

		// restore interrputs
        XDP_TRACE_RESTORE_INTERRUPTS(interruptsStatus);

	}

    // Is application tag enabled to print
    if ( (ItsPrintActiveAppTags & (1<<theAppTag)) &&
         (ItsPrintActiveColors[theAppTag - 1] & (1<<theColor)) ) {
		// Yes, Prepare to print

		// Build array from parameters
		// Array is local becuase the function is reenetrant
		UINT32 paramArray[XDP_TRACE_PARAMS_USER + 8];
		
		// Foramt string
		paramArray[XDP_TRACE_PARAMS_FMT_STRING] = (UINT32) theFormatString;

		// Time stamp
		XDP_TRACE_WRITE_TIME_STAMP(&paramArray[XDP_TRACE_PARAMS_TIME]);

		// Task
		XDP_TRACE_WRITE_TASK      (&paramArray[XDP_TRACE_PARAMS_TASK]);

		// User parameters
		paramArray[XDP_TRACE_PARAMS_USER + 0]  = theP0;
		paramArray[XDP_TRACE_PARAMS_USER + 1]  = theP1;
		paramArray[XDP_TRACE_PARAMS_USER + 2]  = theP2;
		paramArray[XDP_TRACE_PARAMS_USER + 3]  = theP3;
		paramArray[XDP_TRACE_PARAMS_USER + 4]  = theP4;
		paramArray[XDP_TRACE_PARAMS_USER + 5]  = theP5;
		paramArray[XDP_TRACE_PARAMS_USER + 6]  = theP6;
		paramArray[XDP_TRACE_PARAMS_USER + 7]  = theP7;

		// Build entry header for print, put just color, application tag and line number since only those are used
        UINT32  printEntryFlagsAndLineNum  = theColor | (theAppTag << XDP_TRACE_APP_SHIFT) | theLineNumber << XDP_TRACE_LINE_NUM_SHIFT;

		// Print the entry

		PrintEntry(printEntryFlagsAndLineNum, paramArray, XDP_TRACE_PARAMS_USER + 8, NULL, 0);

	} // if print
}

// --- _WriteEntry11 ----------------------------------------------------------
void XdpTrace::_WriteEntry11(UINT16 theColor, UINT16 theAppTag, UINT16 theLineNumber, const char *theFormatString,
				 UINT32 theP0, UINT32 theP1, UINT32 theP2, UINT32 theP3,
                 UINT32 theP4, UINT32 theP5, UINT32 theP6, UINT32 theP7,
                 UINT32 theP8, UINT32 theP9, UINT32 theP10) {

    // Write entry to buffer if the application tag and color is active
    if ( (ItsBufferActiveAppTags & (1<<theAppTag)) &&
         (ItsBufferActiveColors[theAppTag - 1] & (1<<theColor)) ) {

		UINT32 *p;

		// disable interrputs
        UINT32 interruptsStatus;
        XDP_TRACE_DISABLE_INTERRUPTS(interruptsStatus);

        // Allocate record for entry header and 4 paramters
		p = AllocateRecord();
        bool entryHeader = WasRecordEntryHeader(p);
        if (entryHeader)
        {
            ItsEntryCounter--;
        }


		// Save entry pointer and value. They will be the last entry write.
        // Before the write, the entry is zero according to buffer operation rules
        UINT32 *entryFlagsAndLineNumP = p;
        UINT32  entryFlagsAndLineNum  = theColor |
                                        XDP_TRACE_RECORD_VALID | // Record is valid
                                        XDP_TRACE_RECORD_FIRST | // first record
                                        theAppTag << XDP_TRACE_APP_SHIFT | // Application tag
                                        theLineNumber << XDP_TRACE_LINE_NUM_SHIFT;
		p++;

		// Write format String
		*p = (UINT32) theFormatString;
		p++;

		// Write time stamp
		XDP_TRACE_WRITE_TIME_STAMP(p);
		p++;

		// Write task
		XDP_TRACE_WRITE_TASK(p);
		p++;

		// Write paramters
		*p = theP0; p++;
		*p = theP1; p++;
		*p = theP2; p++;
		*p = theP3; p++;

        // Allocate second record for next 8 paramters (P4-P10)
		p = AllocateRecord();
        entryHeader = WasRecordEntryHeader(p);
        if (entryHeader)
        {
            ItsEntryCounter--;
        }


        // Write second record header
        *p = XDP_TRACE_RECORD_VALID |  // Record is valid
             XDP_TRACE_RECORD_LAST;    // Last record in entry
		p++;

        // Write paramters
		*p = theP4; p++;
		*p = theP5; p++;
		*p = theP6; p++;
		*p = theP7; p++;
		*p = theP8; p++;
		*p = theP9; p++;
		*p = theP10; p++;

		// Increase last entry number
		ItsEntryCounter++;


        // Mark the next entry as invalid
        *ItsBufferWriteP = 0;

        // Write this entry flags (and line number). This marks them as valid

        *entryFlagsAndLineNumP = entryFlagsAndLineNum;

		// restore interrputs
        XDP_TRACE_RESTORE_INTERRUPTS(interruptsStatus);

	}

		
	// Is application tag and color enabled to print
    if ( (ItsPrintActiveAppTags & (1<<theAppTag)) &&
         (ItsPrintActiveColors[theAppTag - 1] & (1<<theColor)) ) {
		// Yes, Prepare to print

		// Build array from parameters
		// Array is local becuase the function is reenetrant
		UINT32 paramArray[XDP_TRACE_PARAMS_USER + 11];
		
		// Foramt string
		paramArray[XDP_TRACE_PARAMS_FMT_STRING] = (UINT32) theFormatString;

		// Time stamp
		XDP_TRACE_WRITE_TIME_STAMP(&paramArray[XDP_TRACE_PARAMS_TIME]);

		// Task
		XDP_TRACE_WRITE_TASK      (&paramArray[XDP_TRACE_PARAMS_TASK]);

		// User parameters
		paramArray[XDP_TRACE_PARAMS_USER + 0]  = theP0;
		paramArray[XDP_TRACE_PARAMS_USER + 1]  = theP1;
		paramArray[XDP_TRACE_PARAMS_USER + 2]  = theP2;
		paramArray[XDP_TRACE_PARAMS_USER + 3]  = theP3;
		paramArray[XDP_TRACE_PARAMS_USER + 4]  = theP4;
		paramArray[XDP_TRACE_PARAMS_USER + 5]  = theP5;
		paramArray[XDP_TRACE_PARAMS_USER + 6]  = theP6;
		paramArray[XDP_TRACE_PARAMS_USER + 7]  = theP7;
		paramArray[XDP_TRACE_PARAMS_USER + 8]  = theP8;
		paramArray[XDP_TRACE_PARAMS_USER + 9]  = theP9;
		paramArray[XDP_TRACE_PARAMS_USER + 10] = theP10;

		// Build entry header for print, put just color, application tag and line number since only those are used
        UINT32  printEntryFlagsAndLineNum  = theColor | (theAppTag << XDP_TRACE_APP_SHIFT) | theLineNumber << XDP_TRACE_LINE_NUM_SHIFT;

		// Print the entry

		PrintEntry(printEntryFlagsAndLineNum, paramArray, XDP_TRACE_PARAMS_USER + 11, NULL, 0);

	} // if print
}

// --- _WriteEntry16 ----------------------------------------------------------
void XdpTrace::_WriteEntry16(UINT16 theColor, UINT16 theAppTag, UINT16 theLineNumber, const char *theFormatString,
				 UINT32 theP0,  UINT32 theP1,  UINT32 theP2,  UINT32 theP3,
                 UINT32 theP4,  UINT32 theP5,  UINT32 theP6,  UINT32 theP7,
                 UINT32 theP8,  UINT32 theP9,  UINT32 theP10, UINT32 theP11,
                 UINT32 theP12, UINT32 theP13, UINT32 theP14, UINT32 theP15) {

    // Write entry to buffer if the application tag and color is active
    if ( (ItsBufferActiveAppTags & (1<<theAppTag)) &&
         (ItsBufferActiveColors[theAppTag - 1] & (1<<theColor)) ) {

		UINT32 *p;

		// disable interrputs
        UINT32 interruptsStatus;
        XDP_TRACE_DISABLE_INTERRUPTS(interruptsStatus);

        // Allocate record for entry header and 4 paramters
		p = AllocateRecord();
        bool entryHeader = WasRecordEntryHeader(p);
        if (entryHeader)
        {
            ItsEntryCounter--;
        }


		// Save entry pointer and value. They will be the last entry write.
        // Before the write, the entry is zero according to buffer operation rules
        UINT32 *entryFlagsAndLineNumP = p;
        UINT32  entryFlagsAndLineNum  = theColor |
                                        XDP_TRACE_RECORD_VALID | // Record is valid
                                        XDP_TRACE_RECORD_FIRST | // first record
                                        theAppTag << XDP_TRACE_APP_SHIFT | // Application tag
                                        theLineNumber << XDP_TRACE_LINE_NUM_SHIFT;
		p++;

		// Write format String
		*p = (UINT32) theFormatString;
		p++;

		// Write time stamp
		XDP_TRACE_WRITE_TIME_STAMP(p);
		p++;

		// Write task
		XDP_TRACE_WRITE_TASK(p);
		p++;

		// Write paramters
		*p = theP0; p++;
		*p = theP1; p++;
		*p = theP2; p++;
		*p = theP3; p++;

        // Allocate second record for next 8 paramters (P4-P11)
		p = AllocateRecord();
        entryHeader = WasRecordEntryHeader(p);
        if (entryHeader)
        {
            ItsEntryCounter--;
        }


        // Write second record header. It is not the last record in the entry
        *p = XDP_TRACE_RECORD_VALID;  // Record is valid
		p++;

        // Write paramters
		*p = theP4; p++;
		*p = theP5; p++;
		*p = theP6; p++;
		*p = theP7; p++;
		*p = theP8; p++;
		*p = theP9; p++;
		*p = theP10; p++;

          // Allocate third record for last 6 paramters (P11-P15)
		p = AllocateRecord();
        entryHeader = WasRecordEntryHeader(p);
        if (entryHeader)
        {
            ItsEntryCounter--;
        }


        // Write third record header
        *p = XDP_TRACE_RECORD_VALID |  // Record is valid
             XDP_TRACE_RECORD_LAST;    // Last record in entry
		p++;

        // Write paramters
		*p = theP11; p++;
		*p = theP12; p++;
		*p = theP13; p++;
		*p = theP14; p++;
		*p = theP15; p++;

		// Increase last entry number
		ItsEntryCounter++;


        // Mark the next entry as invalid
        *ItsBufferWriteP = 0;

        // Write this entry flags (and line number). This marks them as valid

        *entryFlagsAndLineNumP = entryFlagsAndLineNum;

		// restore interrputs
        XDP_TRACE_RESTORE_INTERRUPTS(interruptsStatus);

	}

    // Is application tag and color enabled to print
    if ( (ItsPrintActiveAppTags & (1<<theAppTag)) &&
         (ItsPrintActiveColors[theAppTag - 1] & (1<<theColor)) ) {
		// Yes, Prepare to print

		// Build array from parameters
		// Array is local becuase the function is reenetrant
		UINT32 paramArray[XDP_TRACE_PARAMS_USER + 16];
		
		// Foramt string
		paramArray[XDP_TRACE_PARAMS_FMT_STRING] = (UINT32) theFormatString;

		// Time stamp
		XDP_TRACE_WRITE_TIME_STAMP(&paramArray[XDP_TRACE_PARAMS_TIME]);

		// Task
		XDP_TRACE_WRITE_TASK      (&paramArray[XDP_TRACE_PARAMS_TASK]);

		// User parameters
		paramArray[XDP_TRACE_PARAMS_USER + 0]  = theP0;
		paramArray[XDP_TRACE_PARAMS_USER + 1]  = theP1;
		paramArray[XDP_TRACE_PARAMS_USER + 2]  = theP2;
		paramArray[XDP_TRACE_PARAMS_USER + 3]  = theP3;
		paramArray[XDP_TRACE_PARAMS_USER + 4]  = theP4;
		paramArray[XDP_TRACE_PARAMS_USER + 5]  = theP5;
		paramArray[XDP_TRACE_PARAMS_USER + 6]  = theP6;
		paramArray[XDP_TRACE_PARAMS_USER + 7]  = theP7;
		paramArray[XDP_TRACE_PARAMS_USER + 8]  = theP8;
		paramArray[XDP_TRACE_PARAMS_USER + 9]  = theP9;
		paramArray[XDP_TRACE_PARAMS_USER + 10] = theP10;
		paramArray[XDP_TRACE_PARAMS_USER + 11] = theP11;
		paramArray[XDP_TRACE_PARAMS_USER + 12] = theP12;
		paramArray[XDP_TRACE_PARAMS_USER + 13] = theP13;
		paramArray[XDP_TRACE_PARAMS_USER + 14] = theP14;
		paramArray[XDP_TRACE_PARAMS_USER + 15] = theP15;

		// Build entry header for print, put just color, application tag and line number since only those are used
        UINT32  printEntryFlagsAndLineNum  = theColor | (theAppTag << XDP_TRACE_APP_SHIFT) | theLineNumber << XDP_TRACE_LINE_NUM_SHIFT;

		// Print the entry

		PrintEntry( printEntryFlagsAndLineNum, paramArray, XDP_TRACE_PARAMS_USER + 16, NULL, 0);

	} // if print
}

// --- _XdpWriteEntryNullTraceP -----------------------------------------------
//
// This is a static helpper function, it is called from XDP_WRITExx macros
// when RN_DEBUG is defined and the trace poiner in NULL
//
//
void XdpTrace::_XdpWriteEntryNullTraceP() {

    // Let's hope we are no inside interrupt (There is nothing better to do)

    REC_FATAL(17502);

}


// ============================================================================
// ===
// === Buffer state (public)
// ===
// ============================================================================
// --- On ---------------------------------------------------------------------
//
// Switch to off state
//  1. Verify Off state
//  2. Disable buffer
//  3. Verify buffer exists
//  4. Verify other parameteres
//  5. Setup buffer pointers & variable
//  6. Set on state
//  7. Start the buffer
//
bool XdpTrace::On() {

    RN_TRACE(MASK_XDP, ItsLongName + " ::On (" + XdpConv::ToHexStr(this) + ")" );

    bool rc;

    //  1. Verify off state

    rc = VerifyStateOff();
    if ( rc == false ) {
        return false;
    }

    //  2. Disable buffer - just in case

    // Disable all colors
    SetBufferActiveColors((UINT32)0);

    // Mark the buffer as inactive
    ItsBufferActiveFlag = false;


    //  3. Verify buffer exists

    if (ItsBufferBaseP == NULL) {
        // not exist, error

        SetErrorString("Buffer does not exists, can't switch to On state.");
        return false;
    }

    //  4. Verify buffer parameters

    // Check we have at least 3 records in the buffer
    if ( ItsBufferSizeInRecords < 3 * ItsMaxRecordsInEntry) {
        // We don't, error
        SetErrorString("Buffer should have room of at lease 3 entries.");
        return false;
    }

    //  5. Set up buffer pointers and variables

    // Setup buffer end pointer, substract one becuase we can write at the buffer end pointer
    ItsBufferEndP   = ItsBufferBaseP + XDP_TRACE_UINT32_IN_RECORD * (ItsBufferSizeInRecords - 1);

    // Setup buffer wrap pointer, depending on the cyclic flag
    if (ItsCyclicFlag) {
        // Cyclic, wrap at the end
        ItsBufferWrapP  = ItsBufferEndP;
    }
    else {
        // Non cyclic, wrap at the end, but leave room for one entry plus on record past the wrap pointer
        ItsBufferWrapP  = ItsBufferEndP - XDP_TRACE_UINT32_IN_RECORD * (ItsMaxRecordsInEntry + 1);
    }

    //  6. Set On state

    ItsState = XDP_TRACE_STATE_ON;

    //  7. Start the buffer
    rc = Start();

    return true;
}

// --- Off --------------------------------------------------------------------
//
// Switch to off state
//  1. Verify On state
//  2. Disable buffer
//  3. Set buffer state to inactive
//  4. Set off state
//
bool XdpTrace::Off() {

    RN_TRACE(MASK_XDP, ItsLongName + " ::Off (" + XdpConv::ToHexStr(this) + ")" );


    bool rc;

    //  1. Verify on state

    rc = VerifyStateOn();
    if ( rc == false ) {
        return false;
    }

    //  2. Disable buffer

    // Disable all colors
    SetBufferActiveColors((UINT32)0);

    //  3. Set buffer to inactive

    ItsBufferActiveFlag = false;

    //  4. Set off state

    ItsState = XDP_TRACE_STATE_OFF;

    // All done

    return true;
}

// --- Start ------------------------------------------------------------------
//
// Start the buffer
//  1. Verify on state
//  2. Verify buffer is not active
//  3. Invalidated the cursor and zero buffer's counter
//  4. Clear the buffer
//  5. Reset buffer pointer and and zero the entries counter
//  6. Save start time stamp and set time offset
//  7. Enable buffer
//
bool XdpTrace::Start() {

    RN_TRACE(MASK_XDP, ItsLongName + " ::Start (" + XdpConv::ToHexStr(this) + ")" );


    bool rc;

    //  1. Verify on state

    rc = VerifyStateOn();
    if ( rc == false ) {
        return false;
    }

    //  2. Verify buffer is not active

    if ( ItsBufferActiveFlag == true) {
        // Running, error

        SetErrorString("Buffer is already active");
        return false;

    }

    //  3. Invalidated the cursor and zero the entries counter

    ItsCursorEntryP      = NULL;
    ItsCursorEntryNumber = 0;
    ItsCursorWrapCounter = 0;

    //  4. Clear the buffer
    ZeroAllRecords();

    //  5. Reset buffer pointer and zero buffer's counters

    ItsBufferWriteP            = ItsBufferBaseP;
    ItsEntryCounter            = 0;
    ItsWrapCounter             = 0;
    _ItsNonActiveDiscardCounter = 0;
    ItsFullStopFlag            = false;

    //  6. Save start time stamp and set time offset

    // Save start time stamp

    XDP_TRACE_WRITE_TIME_STAMP(&ItsStartTimeStamp);

    // Set the time offset to the start time

    ItsTimeOffset = ItsStartTimeStamp * ItsTimeStampRealResolution;

    //  7. Enable buffer

    // Get enabled colors

    UINT32 *enabledColors = GetEnableColors();

    // Info if all colors are disabled
    if (IsAllColorsDisabled(enabledColors) == true) {
        // All disabled
        SetInfoString("All Colors are disabled");
    }

    // Mark the buffer as active
    ItsBufferActiveFlag = true;

    // Active the enabled colors
    SetBufferActiveColors(enabledColors);

    // All done

    return true;
}

// --- Stop -------------------------------------------------------------------
//
// Stop the buffer
//  1. Verify on state
//  2. Verify buffer is active
//  3. Disable buffer
//
bool XdpTrace::Stop() {

    RN_TRACE(MASK_XDP, ItsLongName + " ::Stop (" + XdpConv::ToHexStr(this) + ")" );

    bool rc;

    //  1. Verify on state

    rc = VerifyStateOn();
    if ( rc == false ) {
        return false;
    }

    //  2. Verify buffer is active

    if ( ItsBufferActiveFlag == false) {
        // Running, error

        SetErrorString("Buffer is already inactive");
        return false;

    }

    //  3. Disable buffer

    // Disable all colors
    SetBufferActiveColors((UINT32)0);

    // Mark the buffer as inactive
    ItsBufferActiveFlag = false;


    // All done

    return true;
}

// --- Resume -----------------------------------------------------------------
//
// Resume the buffer
//  1. Verify on state
//  2. Verify buffer is not active
//  3. Verify buffer is not stop due to buffer full (non-cyclic)
//  4. Enable buffer
//
bool XdpTrace::Resume() {

    RN_TRACE(MASK_XDP, ItsLongName + " ::Resume (" + XdpConv::ToHexStr(this) + ")" );

    bool rc;

    //  1. Verify on state

    rc = VerifyStateOn();
    if ( rc == false ) {
        return false;
    }

    //  2. Verify buffer is not active

    if ( ItsBufferActiveFlag == true) {

        // Running, error

        SetErrorString("Buffer is already active");
        return false;

    }

    //  3. Verify buffer is not stop due to buffer full (non-cyclic)

    if ( (ItsFullStopFlag == true) && (ItsCyclicFlag == false) ) {

        // non-cyclic full stop, error

        SetErrorString("Buffer has been stopped because it is full (non-cyclic mode). It can't be resumed (use start).");
        return false;

    }

    //  4. Enable buffer

    // Get enabled colors

    UINT32 *enabledColors = GetEnableColors();

    // Info if all colors are disabled
    if (IsAllColorsDisabled(enabledColors) == true) {
        // All disabled
        SetInfoString("All Colors are disabled");
    }

    // Mark the buffer as active
    ItsBufferActiveFlag = true;

    // Active the enabled colors
    SetBufferActiveColors(enabledColors);

    // All done

    return true;
}



// ============================================================================
// ===
// === Configuration save and restore functions (public)
// ===
// ============================================================================


// --- RestoreConfig --------------------------------------------------------------
//
// Update trace configuration (app tags, colors, output channel) with restored values
//
bool  XdpTrace::RestoreConfig(const UINT32  theEnabledAppTags,
                              const UINT32* theEnabledColorsP,
                              const UINT32  thePrintActiveAppTags,
                              const UINT32* thePrintActiveColorsP,
                              const XdpTrace::XdpTraceOutput thePrintOutputChannel)
{

    // Verify in off or on state
    if (VerifyStateOffOrOn() == false) {
        return false;
    }


    //***********  update app tags with it restored values **********

    //update the enabled app tags
    ItsEnabledAppTags = theEnabledAppTags;

    //if the buffer is active update the active app tags
    if (ItsBufferActiveFlag == true)
    {
        ItsBufferActiveAppTags = ItsEnabledAppTags;
    }

    //update the print active app tags
    ItsPrintActiveAppTags = thePrintActiveAppTags;

    // Update the common active app tags
    _ItsActiveAppTags = ItsBufferActiveAppTags | ItsPrintActiveAppTags;

    // Set info string
    SetInfoString("Enabled app tags: " + AppTagMaskToStr(ItsEnabledAppTags));
    SetInfoString("Enabled print app tags: " + AppTagMaskToStr(ItsPrintActiveAppTags));


    //***********  update colors with it restored values **********
    for (UINT32 appTag = XDP_TRACE_APP_FIRST - 1; appTag <= XDP_TRACE_APP_LAST - 1; appTag++)
    {
       //update the enabled colors
       ItsEnabledColors[appTag] = theEnabledColorsP[appTag];

       // if the buffer is active, update the active colors
       if (ItsBufferActiveFlag == true) {
            ItsBufferActiveColors[appTag] = theEnabledColorsP[appTag];
       }

       // update the print colors
       ItsPrintActiveColors[appTag] = thePrintActiveColorsP[appTag];

       // update the common active colors
       _ItsActiveColors[appTag] = ItsBufferActiveColors[appTag] | ItsPrintActiveColors[appTag];

    } //for

    // Set info string
    SetInfoString("Enabled colors:\n" + ColorMaskToStr(ItsEnabledColors));
    SetInfoString("Enabled print colors:\n" + ColorMaskToStr(ItsPrintActiveColors));

    //***********  update the print output channel with it restored values **********
    if (SetPrintOutputChannel(thePrintOutputChannel) == false) {
        return false;
    }

    // All done OK
    return true;
}


// --- SaveConfig --------------------------------------------------------------
//
// Save trace configuration (app tags, colors, output channel) to file
//

bool  XdpTrace::SaveConfig()
{
        XdpConfigMngr::GetIt()->SaveTraceConfig(ItsLongName,
                                              ItsEnabledAppTags,
                                              ItsEnabledColors,
                                              ItsPrintActiveAppTags,
                                              ItsPrintActiveColors,
                                              ItsPrintOutputChannel);

      return true;
}


// --- ShowTraceConfig --------------------------------------------------------------
//
// Print to screen all XdpTrace paramrters which are saved to configuration file
// Used for tests
//

bool XdpTrace::ShowTraceConfig()
{
    printf("-----  XdpTrace::ShowTraceConfig  -----\n");
    printf("trace name = %s\n", ItsLongName.operator const char*());
    printf(" ItsEnabledAppTags = %d\n", ItsEnabledAppTags);
    UINT32 appTag;
    for (appTag =0; appTag <= 14; appTag++)
    {
        printf(" ItsEnabledColors[%d] = %d\n",appTag, ItsEnabledColors[appTag]);
    }
    for (appTag =0; appTag <= 14; appTag++)
    {
        printf(" ItsPrintActiveColors[%d] = %d\n",appTag, ItsPrintActiveColors[appTag]);
    }
    printf(" ItsPrintActiveAppTags = %d\n", ItsPrintActiveAppTags);
    printf(" ItsPrintOutputChannel = %d\n", ItsPrintOutputChannel);
    return true;

}











// ============================================================================
// ===
// === User interface functions (public)
// ===
// ============================================================================

// --- ProcessUI --------------------------------------------------------------
//
// Process user interface requests from command manager. Forward the request
// to the user interface obj - ItsUI
//

void XdpTrace::ProcessUI(CmdMngr *theCmdMngrP) {

    // Forward the request to the UI object
    ItsUserInterfaceP->ProcessCommand(this, theCmdMngrP);

}

// --- GetYourId --------------------------------------------------------------
//
// Return the trace name, for CmdMngr/MonApplic
//

RWCString XdpTrace::GetYourId() {

    return ItsLongName;

}

// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// ***
// *** protected
// ***
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************

// ============================================================================
// ===
// === Display and output functions
// ===
// ============================================================================

// --- Display ----------------------------------------------------------------
//
// Displays on the output channel the following theCount entries from the current
// cursor position. If theCount is negative, then the previous entries are displayed.
//
// The entries are displayed on theOutput channel.
//
// The cursor is moved by this function.
//
// This function uses the object display buffer to read the record
//
// Every ItsDisplayReliefNumber of entries display, a 10ms sleep is made
//
// If a failure occurs, the functions returns false and set the error string
//

bool XdpTrace::Display(INT32 theCount, XdpTrace::XdpTraceOutput theOutput) {

    bool rc;

    UINT32 count; // Number of entries to display

    // Check buffer state is on or recover

    rc = VerifyStateOnOrRecover();
    if (rc == false) {
        return false;
    }

    // Check theOutput is valid

    rc = VerifyOutputChannel(theOutput);
    if (rc == false) {
        return false;
    }

    // Check cursor is valid

    rc = VerifyCursorValid();
    if (rc == false) {
        return false;
    }

    // If the output is not CmdMngr then info the user
    if (theOutput != XDP_TRACE_OUTPUT_CMDMNGR) {
        // outpuy not on CmdMngr, set info string
        SetInfoString("Display output is on " + ToStr(theOutput) );
    }

    // Do we display backward entries
    if (theCount < 0) {
        // Backward, set count and seek back;

        // Seek back
        bool dummyEob;
        rc = SeekCursor(theCount, &dummyEob);
        if (rc == false) {
            return false;
        }

        // Set the count
        count = -theCount;
    }
    else {
        // Forward, just set the count
        count = theCount;
    }


    // Define tmp vars for loop
    UINT32 entryHeader;
    UINT32 paramCount;
    UINT32 entryNumber;
    RWCString s;
    bool   endOfBuffer;

    // Display the entires

    UINT32 ent;
    for(ent = 0 ; ent < count ; ent++) {

        // Read the entry, use display buffers
        rc = ReadEntry(&entryHeader, ItsDisplayParamsBuffer,&paramCount);
        if (rc == false) {
            return false;
        }

        // Get cursor location
        rc = GetCursorEntryNumber(&entryNumber);
        if (rc == false) {
            return false;
        }

        // Format the entry into a string

        s = FormatEntry(ItsDisplaySeqNumber, entryNumber, entryHeader,
                        ItsDisplayParamsBuffer, paramCount,
                        NULL, 0); // Add User byte support here !

        // Output the entry string

        rc = Output(theOutput, s.data());
        if (rc == false) {
            return false;
        }

        // Advance the display output sequence number

        ItsDisplaySeqNumber++;

        // Seek to the next entry

        rc = SeekCursor( 1 , &endOfBuffer);
        if (rc == false) {
            return false;
        }

        // If we got to the end of buffer, break the loop
        if (endOfBuffer == true) {
            // At end of buffer
            break;
        }

    }

    // All done

    return true;

}

// --- Output -----------------------------------------------------------------
//
// theOutput - Output channel
// theString - String to be output on the output channel
//
// Sets the error string
//

bool XdpTrace::Output(XdpTrace::XdpTraceOutput theOutput, const char *theString) {

    bool rc = true;

    // Select the output channel
    switch (theOutput) {

        case XDP_TRACE_OUTPUT_CMDMNGR:
            // Print to command manager via the user interface
            ItsUserInterfaceP->PrintString(theString);
            rc = true;
            break;

        case XDP_TRACE_OUTPUT_UDP:
            // Print to UDP via the XDP services
            rc = ItsXdpServicesP->OutputToUdp(theString);
            if (rc == false) {
                SetErrorString("Failed to output to UDP");
            }
            break;

        case XDP_TRACE_OUTPUT_SERIAL:

            // Print to serial via the XDP services
            rc = ItsXdpServicesP->OutputToSerial(theString);
            if (rc == false) {
                SetErrorString("Failed to output to serial");
            }
            break;

            //case  XDP_TRACE_OUTPUT_FILE:

        default:
            // unsuported channel
            SetErrorString("Unsuppotted output channel " + XdpConv::ToStr((UINT32)theOutput) );
            rc = false;
            break;

    } // switch

    // Advance the relief counter
    ItsOutputReliefCounter++;

    // Check need to relief
    if (ItsOutputReliefCounter >= ItsOutputReliefNumber) {

        // It time to relief

        // Clear the relief counter
        ItsOutputReliefCounter = 0;

        // Relief - sleep for 10m Sec
        OSTimeUtil::Sleep(10);
    }


    return rc;

}

// --- SetOutputChannel -------------------------------------------------------
//
// Set the output channel.
//
// If a failure occurs, the functions returns false and set the error string
//

bool XdpTrace::SetOutputChannel(XdpTrace::XdpTraceOutput theOutput) {

    RN_TRACE(MASK_XDP, ItsLongName + " ::SetOutputChannel (" + XdpConv::ToHexStr(this) +
                       ")  output = " + ToStr(theOutput) );

    bool rc;

    // Check theOutput is valid

    rc = VerifyOutputChannel(theOutput);
    if (rc == false) {
        return false;
    }

    // Valid, Set it

    ItsOutputChannel = theOutput;

    // All done
    return true;
}

// --- SetOutputReliefNumber --------------------------------------------------
//
// Set the number of entries displayed between reliefs (10 mSec sleeps)
//
// If a failure occurs, the functions returns false and set the error string
//

bool XdpTrace::SetOutputReliefNumber(UINT32 theReliefNumber) {

    RN_TRACE(MASK_XDP, ItsLongName + " ::SetOutputReliefNumber (" + XdpConv::ToHexStr(this) +
                        ")  output = " + XdpConv::ToStr(theReliefNumber) );

    // Set it

    ItsOutputReliefNumber = theReliefNumber;

    // All done
    return true;
}


// --- VerifyOutputChannel -----------------------------------------------------
//
// Verify theOutput is a valid output channel. If not set the error string and return false
//
bool XdpTrace::VerifyOutputChannel(XdpTrace::XdpTraceOutput theOutput)
{

    // Check output channel
    if (theOutput < XDP_TRACE_OUTPUT_NEXT) {

        // output is OK
        return true;
    }

    // Bad dolor

    SetErrorString("The value " + XdpConv::ToStr((UINT32)theOutput) +
                   " is invalid output channel value.");

    return false;

}



// ============================================================================
// ===
// === Cursor functions
// ===
// ============================================================================

// ---- SeekCursor ------------------------------------------------------------
//
// Moves the cursor theEntryCount entries relative to the current cursor.
// Positive value, move forward. Negative value, move backward.
//
// If the end of buffer is reached, cursor is set to the buffer begining/end
// depending on the movment direction and an info message is set. In this
// case the function returns true, and theEndOfBufferFlag is set to true,
// otherwise it returns false
//
// Normaly the code executes with interrupts disabled. But every
// ItsSeekInterReliefNumber iterration the interrups are restored and disabled
// again. This pervent long periods of disabled interrupts. Further more, every
// ItsSeekTaskReliefNumber the function SeekTaskRelief is call this enable to break
// long run time with this task.
//
// Return true if OK. Fails due to empty buffer, invalid records, invalid cursor,
// cursor lose during seek
//
// Sets the error string in case of error
//

bool XdpTrace::SeekCursor(INT32 theEntryCount, bool *theEndOfBufferFlagP) {

    bool forwardDirection; // Seek forward or backword
    UINT32 entriesLeft;    // Number of entries left to seek

    bool rc;
    UINT32 interruptsStatus;  // For disable/restore interruprs

    // Set forward flag and entriesLeft
    if (theEntryCount >= 0) {
        // Going forward
        forwardDirection = true;
        entriesLeft      = theEntryCount;
    }
    else {
        // Going backward
        forwardDirection = false;
        entriesLeft      = -theEntryCount;  // Negative to positive
    }

    // Clear the end of buffer flag
    *theEndOfBufferFlagP = false;

    // Check cursor valid before doing something
    rc = VerifyCursorValid();
    if (rc == false) {
        return false;
    }

    // Prepare for seek interrations

    // Clean return codes
    UINT32 rcCursorValid = true;
    UINT32 rcEntryMove   = true;

    // Don't make too much iteration with interrupts disabled.
    // Restore and reenable them from time to time
    // Start with zero to force cursor validity check on first iteration.

    UINT32 interReliefLeft = 0;

    // Don't make long execution times with is prioity,
    // from time to time call the task relief method

    UINT32 taskReliefLeft = ItsSeekTaskReliefNumber;

    // disable the interrups
    XDP_TRACE_DISABLE_INTERRUPTS(interruptsStatus);

    // Get current cursor location (it might be invalid)

    UINT32 *p = ItsCursorEntryP;

    // Loop while there are still entries to seek

    while (entriesLeft != 0) {


        // Check we need to relief the interrupts
        if (interReliefLeft == 0) {
            // Yes, relief interruprs
            // Restore interrups
            XDP_TRACE_RESTORE_INTERRUPTS(interruptsStatus);

            // Inc debug counter
            ItsSeekInterReliefCounter++;

            // Don't disable interrupts for too long. Interrups should happen now ....

            // Re-disable the interrups
            XDP_TRACE_DISABLE_INTERRUPTS(interruptsStatus);

            // Perpare for next interrupt relief
            interReliefLeft = ItsSeekInterReliefNumber;

            // Verify the cursor still is valid
            rcCursorValid = IsCursorValid();
            if (rcCursorValid == false) {
                // cursor not valid, break the loop
                break;
            }
        }

        // Check we need to relief the task
        if (taskReliefLeft == 0) {
            // Yes relieft the task. We don't forget to restore the interrupts before that.

            // Restore interrups
            XDP_TRACE_RESTORE_INTERRUPTS(interruptsStatus);

            // Inc debug counter
            ItsSeekTaskReliefCounter++;

            // Call the relief function with interrupts restored

            SeekCursorTaskRelief();

            // We are back, disable the interrupts and check cursor validity.

            // Re-disable the interrups
            XDP_TRACE_DISABLE_INTERRUPTS(interruptsStatus);

            // Perpare for next task relief
            taskReliefLeft = ItsSeekTaskReliefNumber;

            // Verify the cursor still is valid
            rcCursorValid = IsCursorValid();
            if (rcCursorValid == false) {
                // cursor not valid, break the loop
                break;
            }
        }


        // All relief stuff is done, let's do some real work

        // Try to move cursor, save the entry number delta and wrap count delta
        INT32 entryNumberDelta=0;
        INT32 wrapCounterDelta=0;

        if (forwardDirection) {
            // forward
            rcEntryMove = GetNextEntry(&p, &wrapCounterDelta);
            entryNumberDelta = 1;
        }
        else {
            // backward
            rcEntryMove = GetPrevEntry(&p, &wrapCounterDelta);
            entryNumberDelta = -1;
        }

        // Check for move error
        if (rcEntryMove == false) {
            // Move error, break the loop
            break;
        }

        // Move is OK, check got to end of buffer
        if (p == NULL) {
            // Got to end of buffer

            // Set the end of buffer flag
            *theEndOfBufferFlagP = true;

            // Set an info message depending on the direction
            if (forwardDirection) {
                // Forward
                SetInfoString("Cursor at end of buffer at entry " +
                                XdpConv::ToStr(ItsCursorEntryNumber) + ".");
            }
            else {
                // Backward
                 SetInfoString("Cursor at begging of buffer at entry " +
                                 XdpConv::ToStr(ItsCursorEntryNumber) + ".");
            }
            // ... and break the loop
            break;
        }

        // All OK, update cursor information
        ItsCursorEntryP       = p;

        ItsCursorEntryNumber += entryNumberDelta;
        ItsCursorWrapCounter += wrapCounterDelta;

        // Decrease entry left counter
        entriesLeft--;

        // Decrease relief counters
        interReliefLeft--;
        taskReliefLeft--;

    }

    // Restore interrups
    XDP_TRACE_RESTORE_INTERRUPTS(interruptsStatus);

    // Check for un-handled errors

    if (rcCursorValid == false) {
        // Error, invalid cursor during seek
        ItsCursorEntryP = NULL;
        SetErrorString("Cursor location lost during seek. (Try to stop the buffer).");
        return false;
    }

    if (rcEntryMove == false) {
        // Entry move error
        SetErrorString("Invalid buffer, GetNext/PrevEntry failed near 0x" +
                          XdpConv::ToHexStr(ItsCursorEntryP));

        ItsCursorEntryP = NULL;
        return false;
    }

    // Finally all OK !
    return true;
}

// --- AbsoluteSeekCursor -----------------------------------------------------
//
// Seek the cursor to an absolute entry number.
//
// If the end of buffer is reached, cursor is set to the buffer begining/end
// depending on the movment direction. In this case function returns true,
// and theEndOfBufferFlag is set to true, otherwise it is false
//
// If the cursor is not valid or the seek failed, the function returns false
//  and sets the error string.
//
//
bool XdpTrace::AbsoluteSeekCursor(UINT32 theEntryNumber, bool *theEndOfBufferFlagP) {

    bool rc;

    // Seek to the end of buffer becuase this is the only possible inital location
    rc = SeekCursorToEnd();
    if (rc == false) {
        return false;
    }

    // Read the end of buffer cursor entry number
    UINT32 endEntryNumber;

    rc = GetCursorEntryNumber(&endEntryNumber);
    if (rc == false) {
        return false;
    }

    // Seek beyond the buffer ?

    if (theEntryNumber > endEntryNumber) {
        // Yes, we already at the end of the buffer,

        // set theEndOfBufferP flag and all done

        *theEndOfBufferFlagP = true;

        // Set Info string
        SetInfoString("Cursor at begging of buffer at entry " +
                       XdpConv::ToStr(endEntryNumber) + ".");


        // All done OK

        return true;
    }

    // For now, we didn't reach the end of buffer, clear the flag

    *theEndOfBufferFlagP = false;

    // We can seek only +/-2G (INT32) but we might need to seek more. There for
    // the seek is done in a few smaller steps.
    // Note that we are seeking backword because the cursor is at the end of buffer

    UINT32 maxStep = 1000000000; // Max seek step size. Must be positive < 2G
    INT32 step;                  // The currnet seek step size. Always positive.
    UINT32 delta;                // Number of seek step required. Always positive

    // Calc the delta and the seek step size. Be very carful from arithmitic
    // overflows.

   // Seeking backword
   delta = endEntryNumber - theEntryNumber;

   // Do the seek in steps as long it is needed

    while(delta != 0) {

        // Seek the minimum of delta or the max step size (maxStep)
        if (delta > maxStep) {
            // maxStep is smaller, use it
            step = maxStep;
        }
        else {
            // delta is smaller, use it
            step = delta;
        }

        // Seek the the step backword, add minus sign
        rc=SeekCursor(-step,theEndOfBufferFlagP);
        if (rc == false) {
            return false;
        }

        // Adjust the delta
        delta -= step;

    } // while

    return true;
}

// --- SeekCursorToEnd --------------------------------------------------------
//
// Moves the cursor to the end of buffer. Update all cursor variables.
//
// Return true if OK. Fails due to empty buffer or invalid record
//
// Sets the error string in case of error
//

bool XdpTrace::SeekCursorToEnd() {

    // Check empty buffer
    if (ItsEntryCounter == 0) {

        // Empty buffer, error

        ItsCursorEntryP = NULL;
        SetErrorString("Buffer is empty.");
        return false;
    }

    // Set the cursor. Disable interrupts because it is critical section

	// disable interrputs
    UINT32 interruptsStatus;
    XDP_TRACE_DISABLE_INTERRUPTS(interruptsStatus);

    // Set cursor vars to the record after the last entry, invalid record

    ItsCursorEntryP      = ItsBufferWriteP;
    ItsCursorWrapCounter = ItsWrapCounter;
    ItsCursorEntryNumber = ItsEntryCounter - 1; // Entry number is entry counter - 1

    bool rc;

    // Go back one entry, from the invalid record to the last entry. Update the cursor wrap counter

    INT32 moveWrapDelta = 0;
    rc = GetPrevEntry(&ItsCursorEntryP, &moveWrapDelta);
    ItsCursorWrapCounter += moveWrapDelta;

	// Restore interrputs
    XDP_TRACE_RESTORE_INTERRUPTS(interruptsStatus);

    // Check for errors

    // invalid buffer
    if (rc == false) {
        // Error, invalid buffer
        ItsCursorEntryP = NULL;
        SetErrorString("Invalid buffer, GetPrevEntry failed near 0x" + XdpConv::ToHexStr(ItsBufferWriteP));
        return false;
    }

    // No previous entry (must be one becuase entry count > 0 )
    if (ItsCursorEntryP == NULL) {
        // Error, no prev entry
        ItsCursorEntryP = NULL;
        SetErrorString("Invalid buffer, No previous entry near 0x" + XdpConv::ToHexStr(ItsBufferWriteP));
        return false;
    }

    // Finally all OK !
    return true;
}




// --- IsCursorValid ----------------------------------------------------------
//
// Returns true if the cursor is valid. The cursor might become invalid due
// to buffer overwrite on the cursor location.
//
// If the cursor is invalid, also, set its pointer to NULL
//
// Must be called with interrupts disabled (or buffer stopped)
//
// Use VerifyCursorValid() when called with interrupts enabled and error string
// is required

bool XdpTrace::IsCursorValid() {

    // Check NULL cursor

    if (ItsCursorEntryP == NULL) {
        // NULL cursor pointer -> invalid
        return false;
    }


    // Check validity depending on buffer wraps

    UINT32 wrapDelta = ItsWrapCounter - ItsCursorWrapCounter;

    switch(wrapDelta) {

    case 0: // No wrap - cursor must be before the write pointer
        if (ItsCursorEntryP >= ItsBufferWriteP) {
            // not before, invalid
            ItsCursorEntryP = NULL;
            return false;
        }
        break;

    case 1: // Wrap - cursor must be after the write pointer

        if (ItsCursorEntryP <= ItsBufferWriteP) {
            // not after, invalid
            ItsCursorEntryP = NULL;
            return false;
        }
        break;

    default: // Two wraps or more, cursor must be invalid

       ItsCursorEntryP = NULL;
       return false;

    } // switch

    // All OK - :-)

    return true;

}

// --- VerifyCursorValid ------------------------------------------------------
//
// Returns true if the cursor is valid. The cursor might become invalid due
// to buffer overwrite on the cursor location.
//
// If the cursor is not valid the function returns false and set the error string.
//
// Use IsCursorValid for critical sections.
//
bool XdpTrace::VerifyCursorValid() {

    bool rc;

    UINT32 interruptsStatus;

    // Disable interrupts

    XDP_TRACE_DISABLE_INTERRUPTS(interruptsStatus);

    // Check cursor

    rc = IsCursorValid(); // Must be called with disable interrupts

    // Restore interrurps

    XDP_TRACE_RESTORE_INTERRUPTS(interruptsStatus);

    // Check for not valid

    if (rc == false) {
        // Cursor not valid
        SetErrorString("Cursor not valid.");
        return false;
    }

    // All done

    return true;

}


// --- GetCursorEntryNumber ---------------------------------------------------
//
// Write to theEntryNumberP the cursor's entry number.
//
// If the cursor is not valid the function returns false and set the error string.
//

bool XdpTrace::GetCursorEntryNumber(UINT32 *theEntryNumberP) {

    bool rc;

    UINT32 interruptsStatus;

    // Disable interrupts

    XDP_TRACE_DISABLE_INTERRUPTS(interruptsStatus);

    // Check cursor

    rc = IsCursorValid(); // Must be called with disable interrupts

    // Save the entry number

    UINT32 entryNumber = ItsCursorEntryNumber;

    // Restore interrurps

    XDP_TRACE_RESTORE_INTERRUPTS(interruptsStatus);

    // Check for not valid

    if (rc == false) {
        // Cursor not valid
        SetErrorString("Cursor not valid, can't get cursor entry number.");
        return false;
    }

    // OK, set the entry number

    *theEntryNumberP = entryNumber;

    // Done

    return true;

}

// ============================================================================
// ===
// === Color functions
// ===
// ============================================================================

// --- SetColor ---------------------------------------------------------------
//
// Enable or disable the color For ALL (this is for backward suppot)
// or specific application tag according (theAppTag)
//
// The command sets the status strings
//
// The last prameter is optional and has a default (XDP_TRACE_APP_ALL)
// value for backword support
//
bool XdpTrace::SetColor(bool theEnableFlag, UINT32 theColor, UINT32 theAppTag) {

    RN_TRACE(MASK_XDP, ItsLongName + " ::SetColor (" + XdpConv::ToHexStr(this) +
        ")  enable = " +XdpConv::ToStr(theEnableFlag) +
        " color = " + ToStr((XdpTraceColor)theColor) +
        " for " + ToStr((XdpTraceAppTag)theAppTag));

    bool rc;

    // Verify the color
    rc = VerifyColor(theColor);
    if (rc == false) {
        return false;
    }

    // Verify the application tag
    rc = VerifyAppTag(theAppTag);
    if (rc == false) {
        return false;
    }

    // Calc the color mask
    UINT32 colorMask = 1 << theColor;

    // Update the colors
    UpdateEnabledColors(theEnableFlag, colorMask, theAppTag);

    // All done
    return true;
}

// --- SetAboveColors ---------------------------------------------------------
//
// Enable or disable the colors above the given color for ALL application tags (this is for backward suppot)
//
// The command sets the status strings
//
// This function is for backword support
// The last prameter is optional and has a default (XDP_TRACE_APP_ALL)
// value for backword support
//
bool XdpTrace::SetAboveColors(bool theEnableFlag, UINT32 theColor, UINT32 theAppTag) {

    RN_TRACE(MASK_XDP, ItsLongName + " ::SetAboveColors (" + XdpConv::ToHexStr(this) +
        ")  enable = " +XdpConv::ToStr(theEnableFlag) +
        " color = " + ToStr((XdpTraceColor)theColor) +
        " for " + ToStr((XdpTraceAppTag)theAppTag));

    bool rc;

    // Verify the color
    rc = VerifyColor(theColor);
    if (rc == false) {
        return false;
    }

    // Verify the application tag
    rc = VerifyAppTag(theAppTag);
    if (rc == false) {
        return false;
    }

    // Calc the color mask
    UINT32 colorMask = 0;
    UINT32 col;

    for (col = theColor; col <= XDP_TRACE_COLOR_LAST ; col ++) {
        colorMask |= 1 << col;
    }

    // Update the colors
    UpdateEnabledColors(theEnableFlag, colorMask, theAppTag);

    // All done
    return true;
}


// --- SetBelowColors ---------------------------------------------------------
//
// Enable or disable the colors below the given color
//
// The command sets the status strings
//

// The last prameter is optional and has a default (XDP_TRACE_APP_ALL)
// value for backword support
//
bool XdpTrace::SetBelowColors(bool theEnableFlag, UINT32 theColor, UINT32 theAppTag) {

    RN_TRACE(MASK_XDP, ItsLongName + " ::SetBelowColors (" + XdpConv::ToHexStr(this) +
        ")  enable = " +XdpConv::ToStr(theEnableFlag) +
        " color = " + ToStr((XdpTraceColor)theColor) +
        " for " + ToStr((XdpTraceAppTag)theAppTag));

    bool rc;

    // Verify the color
    rc = VerifyColor(theColor);
    if (rc == false) {
        return false;
    }

    // Verify the application tag
    rc = VerifyAppTag(theAppTag);
    if (rc == false) {
        return false;
    }

    // Calc the color mask
    UINT32 colorMask = 0;
    UINT32 col;

    for (col = XDP_TRACE_COLOR_FIRST; col <= theColor ; col ++) {
        colorMask |= 1 << col;
    }

    // Update the colors
    UpdateEnabledColors(theEnableFlag, colorMask, theAppTag);

    // All done
    return true;
}

// --- UpdateEnabledColors ----------------------------------------------------
//
// Update the enabled flags.
//
// If the enable flag is true, the colors define by the color mask will be enabled
// If it is false, this will be disabled ,for ALL or specific application tag!.
//
// If the buffer is at on and active, the buffer active flags will be update as well
//
// This command is not valid in the recover state.
//
// The command sets the status strings
//
// This command affects ALL application tags
//
bool XdpTrace::UpdateEnabledColors(bool theEnableFlag, UINT32 theColorMask, UINT32 theAppTag ) {

    bool rc;

    // Verify in off or on state
    rc = VerifyStateOffOrOn();
    if (rc==false) {
        return false;
    }

    // Clear the none flag

    theColorMask &= ~(1 << XDP_TRACE_COLOR_NONE);

    // Update all application tags color masks
    if (theAppTag == XDP_TRACE_APP_ALL)
    {
        // Update the enabled colors according to enable or disable for ALL app tags
        UINT32 appTag;

        for (appTag = XDP_TRACE_APP_FIRST - 1; appTag <= XDP_TRACE_APP_LAST - 1; appTag++)
        {
            if (theEnableFlag) {
                // Enable the colors
                ItsEnabledColors[appTag] |= theColorMask;
            }
            else {
                // Disable the colors
                ItsEnabledColors[appTag] &= ~theColorMask;
            }
        } //for
    } //if

    // Update a specific application tag color mask
    else {
        if (theEnableFlag) {
            // Enable the colors
            ItsEnabledColors[theAppTag - 1] |= theColorMask;
        }
        else {
            // Disable the colors
            ItsEnabledColors[theAppTag - 1] &= ~theColorMask;
        }
    }

    // If the buffer is active, update the active color as well
    if (ItsBufferActiveFlag == true) {

        // On state, set the active color to the enabled colors
        SetBufferActiveColors(ItsEnabledColors);

    }

    // Set info string
    SetInfoString("Enabled colors:\n" + ColorMaskToStr(ItsEnabledColors));

    // All done OK

    return true;

}

// --- SetBufferActiveColors(UINT32 theNewColors) --------------------------------------------------
// Set the buffer active colors (ItsBufferActiveColors) ,
// Also update the common active colors (ItsActiveColors) for ALL apllication tags !
//
// Updates the colors according to UINT32 theNewColors, ALL application color masks will
// be the same.
//
void XdpTrace::SetBufferActiveColors(UINT32 theNewColors)
{
    UINT32 appTag;

    for (appTag = XDP_TRACE_APP_FIRST - 1; appTag <= XDP_TRACE_APP_LAST - 1; appTag++)
    {
       // Set the new flags
       ItsBufferActiveColors[appTag] = theNewColors;

       // Update the common active colors
       _ItsActiveColors[appTag] = ItsBufferActiveColors[appTag] | ItsPrintActiveColors[appTag];
    } //for
}

// --- SetBufferActiveColors(UINT32 theNewColors[]) --------------------------------------------------
// Set the buffer active colors (ItsBufferActiveColors)
// Also update the common active colors (ItsActiveColors) for ALL apllication tags !
//
// Updates the colors according to theNewColors ARRAY.
//
void XdpTrace::SetBufferActiveColors(UINT32 theNewColors[])
{
    UINT32 appTag;

    for (appTag = XDP_TRACE_APP_FIRST - 1; appTag <= XDP_TRACE_APP_LAST - 1; appTag++)
        {
            // Set the new flags
            ItsBufferActiveColors[appTag] = theNewColors[appTag];

            // Update the common active colors
        	_ItsActiveColors[appTag] = ItsBufferActiveColors[appTag] | ItsPrintActiveColors[appTag];
        } //for
}

// --- VerifyColor ------------------------------------------------------------
//
// Verify theColor is a valid color. If not set the error string and return false
//
bool XdpTrace::VerifyColor(UINT32 theColor)
{

    // Check color
    if ((theColor >= XDP_TRACE_COLOR_FIRST) && (theColor <= XDP_TRACE_COLOR_LAST) ) {

        // Color is OK
        return true;
    }

    // Bad color

    SetErrorString("The value " + XdpConv::ToStr(theColor) + " is invalid color value.");

    return false;

}

// --- IsAllColorsdisabled ----------------------------------------------------------
//
// Returns true is all the colors are disabled
//

bool XdpTrace::IsAllColorsDisabled(UINT32 theColorMask[]) {

    UINT32 appTag;

    for (appTag = XDP_TRACE_APP_FIRST - 1; appTag <= XDP_TRACE_APP_LAST - 1; appTag++)
    {
        if (theColorMask[appTag] != 0) //at least one color is enabled
        {
            return false;
        }
    }
    return true; // At this point all colors are disabled.
}

// ============================================================================
// ===
// === Application filter functions
// ===
// ============================================================================

// --- SetAppTag ---------------------------------------------------------------
//
// Enable or disable the application tag.
//
// The command sets the status strings
//

bool XdpTrace::SetAppTag(bool theEnableFlag, UINT32 theAppTag)
{

    RN_TRACE(MASK_XDP, ItsLongName + " ::SetAppTag (" + XdpConv::ToHexStr(this) +
        ")  enable = " +XdpConv::ToStr(theEnableFlag) +
        " tag = " + ToStr((XdpTraceAppTag)theAppTag));

    bool rc;

    // Verify the application tag
    rc = VerifyAppTag(theAppTag);
    if (rc == false) {
        return false;
    }

    // Calc the application tag mask
    UINT32 appTagMask = 1 << theAppTag;

    // Update the colors
    UpdateEnabledAppTags(theEnableFlag, appTagMask);

    // All done
    return true;

}

// --- SetPrintAppTag ---------------------------------------------------------------
//
// Enable or disable the application tag for printing.
//
// The command sets the status strings
//

bool XdpTrace::SetPrintAppTag(bool theEnableFlag, UINT32 theAppTag) {

    RN_TRACE(MASK_XDP, ItsLongName + " ::SetPrintAppTag (" + XdpConv::ToHexStr(this) +
        ")  enable = " +XdpConv::ToStr(theEnableFlag) +
        " tag = " + ToStr((XdpTraceAppTag)theAppTag) );

    bool rc;

    // Verify the application tag
    rc = VerifyAppTag(theAppTag);
    if (rc == false) {
        return false;
    }

    // Calc the color mask
    UINT32 appTagMask = 1 << theAppTag;

    // Update the colors
    UpdatePrintAppTags(theEnableFlag, appTagMask);

    // All done
    return true;
}

// --- UpdatePrintAppTags ------------------------------------------------------
//
// Update the print app tags flags.
//
// The buffer active flags are updated as well
//
bool XdpTrace::UpdatePrintAppTags(bool theEnableFlag, UINT32 theAppTagMask) {


    UINT32 printAppTags; // temporary

    printAppTags = ItsPrintActiveAppTags;

    // Remove the none flag
    theAppTagMask &= ~(1 << XDP_TRACE_APP_NONE);

    // Update the app tags according to enable or disable

    if (theEnableFlag) {
        // Enable the app tag
        printAppTags |= theAppTagMask;
    }
    else {
        // Disable the app tag
        printAppTags &= ~theAppTagMask;
    }

    // Set the print color to the enabled colors
    SetPrintActiveAppTags(printAppTags);

    // Set info string
    SetInfoString("Enabled print tags: " + AppTagMaskToStr(ItsPrintActiveAppTags));


    // All done OK

    return true;

}

// --- SetPrintActiveAppTags ---------------------------------------------------
// Set the print active application tags (ItsPrintActiveAppTags).
// Also update the common active application tags (ItsActiveAppTags)
//
void XdpTrace::SetPrintActiveAppTags(UINT32 theNewAppTags)
{

   // Set the new flags
   ItsPrintActiveAppTags = theNewAppTags;

   // Update the common active colors
   _ItsActiveAppTags = ItsBufferActiveAppTags | ItsPrintActiveAppTags;

}

// --- UpdateEnabledAppTags ----------------------------------------------------
//
// Update the enabled flags.
//
// If the enable flag is true, the tags define by the tag mask will be enabled
// If it is false, this will be disabled.
//
// If the buffer is at on and active, the buffer active flags will be update as well
//
// This command is not valid in the recover state.
//
// The command sets the status strings
//
// This command affects ALL application tags
//
bool XdpTrace::UpdateEnabledAppTags(bool theEnableFlag, UINT32 theAppTagMask) {

    bool rc;

    // Verify in off or on state
    rc = VerifyStateOffOrOn();
    if (rc==false) {
        return false;
    }


    // Remove the none flag
    theAppTagMask &= ~(1 << XDP_TRACE_APP_NONE);

    // Update the enabled colors according to enable or disable

    if (theEnableFlag) {
            // Enable the colors
            ItsEnabledAppTags |= theAppTagMask;
    }
    else {
            // Disable the colors
            ItsEnabledAppTags &= ~theAppTagMask;
    }

    // If the buffer is active, update the active color as well
    if (ItsBufferActiveFlag == true) {

        // On state, set the active color to the enabled colors
        SetBufferActiveAppTags(ItsEnabledAppTags);

    }

    // Set info string
    SetInfoString("Enabled application tags: " + AppTagMaskToStr(ItsEnabledAppTags));

    // All done OK

    return true;

}

// --- SetBufferActiveAppTags(UINT32 theNewAppTags) --------------------------------------------------
// Set the buffer active application tags (ItsBufferActiveAppTags) ,
// Also update the common active application tags (ItsActiveAppTags)
//
// Updates the application tags according to UINT32 theNewAppTags
//
void XdpTrace::SetBufferActiveAppTags(UINT32 theNewAppTags)
{
   // Set the new flags
   ItsBufferActiveAppTags = theNewAppTags;

   // Update the common active colors
   _ItsActiveAppTags = ItsBufferActiveAppTags | ItsPrintActiveAppTags;

}

// --- VerifyAppTag ------------------------------------------------------------
//
// Verify theAppTag is a valid application tag. If not set the error string and return false
//
bool XdpTrace::VerifyAppTag(UINT32 theAppTag)
{

    // Check application tag
    if ((theAppTag >= XDP_TRACE_APP_FIRST) && (theAppTag <= XDP_TRACE_APP_ALL) ) {

        // tag is OK
        return true;
    }

    // Bad tag

    SetErrorString("The value " + XdpConv::ToStr(theAppTag) + " is invalid application tag value.");

    return false;

}

// ============================================================================
// ===
// === Buffer control
// ===
// ============================================================================
// --- SetBufferTypeAndSize ----------------------------------------------------------
//
// Set the buffer type and size. If previous memory has been allocated for the buffer
// it will be free. If size is 0, the buffer memory is free
//
// Must at off state
//
bool XdpTrace::SetBufferSizeAndType( UINT32 theBufferSizeInK, XdpTrace::XdpTraceBuffer theBufferType) {


    RN_TRACE(MASK_XDP, ItsLongName + " ::SetBufferSizeAndType (" + XdpConv::ToHexStr(this) +
        ") size = " + XdpConv::ToStr(theBufferSizeInK) +
        "K type = " + ToStr(theBufferType));

    bool rc;

    // Verify off state;
    rc = VerifyStateOff();
    if ( rc == false ) {
        return false;
    }

    // Check free of non allocated buffer
    if ( (theBufferSizeInK == 0) && (ItsBufferBaseP == NULL) ) {
        // Free of non allocated buffer

        SetErrorString("Buffer memory has not been allocated.");
        return false;
    }

    // Going to free the buffer if it has been allocated

    if (ItsBufferBaseP != 0) {
        // Buffer has been allocated, free it.

        // Save buffer pointer
        UINT32 *oldBufP   = ItsBufferBaseP;
        UINT32 oldSizeInK = ItsBufferSizeInK;

        // Mark buffer as free

        ItsBufferBaseP          = NULL;
        ItsBufferSizeInK        = 0;
        ItsBufferSizeInRecords  = 0;

        // Free memory

        switch (ItsBufferType) {
            case XDP_TRACE_BUFFER_HEAP: // Heap

                // Free heap memory
                delete [] oldBufP;

                break;

            default:

                // Error

                SetErrorString("Can't free buffer memory. Memory the type " +
                                  ToStr(ItsBufferType) + " Is not supported.");
                return false;

        } // switch

        // Info the free
        SetInfoString("Buffer memory has been freed from " + XdpConv::ToHexStr(oldBufP)+
                         ". Size: " + XdpConv::ToStr(oldSizeInK) +
                         "K Bytes, Type: " + ToStr(ItsBufferType) + "." );

    } // if need to free

    // If the buffer size is 0 - we no need to allocated memory
    if (theBufferSizeInK == 0) {
        // No need to allocate memory, all done OK
        return true;
    }


    // Here, we need to allocate buffer memoey

    // Set buffer type
    ItsBufferType = theBufferType;

    // Allocate new buffer

    UINT32 *newBufP = NULL;

    switch (ItsBufferType) {
        case XDP_TRACE_BUFFER_HEAP: // Heap

            // Allocate buffer from heap

            newBufP = new UINT32[theBufferSizeInK * 1024 / sizeof(UINT32)];

            // Check error
            if (newBufP == 0) {

                // Error

                SetErrorString("Failed to allocated " + XdpConv::ToStr(theBufferSizeInK) +
                                   "K bytes for buffer from heap.");

                return false;
            }

            break;

        default:

            // Error

            SetErrorString("Can't allocated buffer memory. Memory the type " +
                             ToStr(ItsBufferType) + " Is not supported.");
            return false;
    }


    // Check again we have valid pointer

    if (newBufP == NULL) {
        SetErrorString("internal, newBufP is NULL");
        return false;
    }

    // Info the allocate
    SetInfoString("Buffer memory has been allocated at " + XdpConv::ToHexStr(newBufP) +
                    ". Size: " + XdpConv::ToStr(theBufferSizeInK) +
                   "K Bytes, Type: " + ToStr(ItsBufferType) + ".");


    // Update buffer variables

    ItsBufferBaseP         = newBufP;
    ItsBufferSizeInK       = theBufferSizeInK;
    ItsBufferSizeInRecords = ItsBufferSizeInK * 1024 / ( XDP_TRACE_UINT32_IN_RECORD * sizeof(UINT32) );


    // All done OK

    return true;
}


// --- VerifyStateOff ---------------------------------------------------------
//
// Verify the buffer state is off. If not set the error string and return false
//
bool XdpTrace::VerifyStateOff()
{

    // Check state
    if (ItsState == XDP_TRACE_STATE_OFF) {

        // State OK
        return true;
    }

    // Bad state

    SetErrorString("The operation is available only in Off state. The current state is " +
                   ToStr(ItsState) + ".");

    return false;

}

// --- VerifyStateOn ----------------------------------------------------------
//
// Verify the buffer state is on. If not set the error string and return false
//
bool XdpTrace::VerifyStateOn()
{

    // Check state
    if (ItsState == XDP_TRACE_STATE_ON) {

        // State OK
        return true;
    }

    // Bad state

    SetErrorString("The operation is available only in On state. The current state is " +
                   ToStr(ItsState) + ".");

    return false;

}

// --- VerifyStateRecover -----------------------------------------------------
//
// Verify the buffer state is recover. If not set the error string and return false
//
bool XdpTrace::VerifyStateRecover()
{

    // Check state
    if (ItsState == XDP_TRACE_STATE_RECOVER) {

        // State OK
        return true;
    }

    // Bad state

    SetErrorString("The operation is available only in Recover state. The current state is " +
                   ToStr(ItsState) + ".");

    return false;

}
// --- VerifyStateOffOrOn -----------------------------------------------------
//
// Verify the buffer state is off or on. If not set the error string and return false
//
bool XdpTrace::VerifyStateOffOrOn()
{

    // Check state
    if ((ItsState == XDP_TRACE_STATE_OFF) || (ItsState == XDP_TRACE_STATE_ON)) {

        // State OK
        return true;
    }

    // Bad state

    SetErrorString("The operation is available only in Off and On states. The current state is " +
                   ToStr(ItsState) + ".");

    return false;

}

// --- VerifyStateOffOrRecover ------------------------------------------------
//
// Verify the buffer state is off or recover. If not set the error string and return false
//
bool XdpTrace::VerifyStateOffOrRecover()
{

    // Check state
    if ((ItsState == XDP_TRACE_STATE_OFF) || (ItsState == XDP_TRACE_STATE_RECOVER)) {

        // State OK
        return true;
    }

    // Bad state

    SetErrorString("The operation is available only in Off and Recover states. The current state is " +
                   ToStr(ItsState) + ".");

    return false;

}


// --- VerifyStateOnOrRecover -------------------------------------------------
//
// Verify the buffer state is on or recover. If not set the error string and return false
//
bool XdpTrace::VerifyStateOnOrRecover()
{

    // Check state
    if ((ItsState == XDP_TRACE_STATE_ON) || (ItsState == XDP_TRACE_STATE_RECOVER)) {

        // State OK
        return true;
    }

    // Bad state

    SetErrorString("The operation is available only in On and Recover states. The current state is " +
                   ToStr(ItsState) + ".");

    return false;

}

// --- SetCyclicFlag ----------------------------------------------------------
//
// Set the cyclic flag. Verify off state
//

bool XdpTrace::SetCyclicFlag(bool theCyclicFlag) {

    RN_TRACE(MASK_XDP, ItsLongName + " ::SetCyclicFlag (" + XdpConv::ToHexStr(this) +
        ") cyclicFlag is " + XdpConv::ToStr(theCyclicFlag));

    bool rc;

    // Verify off state;
    rc = VerifyStateOff();
    if ( rc == false ) {
        return false;
    }

    // State is off, set the value;

    ItsCyclicFlag = theCyclicFlag;

    // All done OK

    return true;
}

// ============================================================================
// ===
// === Print functions
// ===
// ============================================================================

// --- SetPrintColor ----------------------------------------------------------
//
// Enable or disable the print color for ALL application tags.
//
// The command sets the status strings
//
bool XdpTrace::SetPrintColor(bool theEnableFlag, UINT32 theColor) {

    return SetPrintColor(theEnableFlag, theColor, XDP_TRACE_APP_ALL);

}

bool XdpTrace::SetPrintColor(bool theEnableFlag, UINT32 theColor, UINT32 theAppTag) {

    RN_TRACE(MASK_XDP, ItsLongName + " ::SetPrintColor (" + XdpConv::ToHexStr(this) +
        ")  enable = " +XdpConv::ToStr(theEnableFlag) +
        " color = " + ToStr((XdpTraceColor)theColor) +
        " for " + ToStr((XdpTraceAppTag)theAppTag));

    bool rc;

    // Verify the color
    rc = VerifyColor(theColor);
    if (rc == false) {
        return false;
    }

    // Verify the application tag
    rc = VerifyAppTag(theAppTag);
    if (rc == false) {
        return false;
    }

    // Calc the color mask
    UINT32 colorMask = 1 << theColor;

    // Update the colors
    UpdatePrintColors(theEnableFlag, colorMask, theAppTag);

    // All done
    return true;
}

// --- SetPrintAboveColors ---------------------------------------------------------
//
// Enable or disable the print colors above the given color
//
// The command sets the status strings
//
bool XdpTrace::SetPrintAboveColors(bool theEnableFlag, UINT32 theColor) {

    return XdpTrace::SetPrintAboveColors(theEnableFlag, theColor, XDP_TRACE_APP_ALL);

}

bool XdpTrace::SetPrintAboveColors(bool theEnableFlag, UINT32 theColor, UINT32 theAppTag) {

    RN_TRACE(MASK_XDP, ItsLongName + " ::SetPrintAboveColors (" + XdpConv::ToHexStr(this) +
        ")  enable = " +XdpConv::ToStr(theEnableFlag) +
        " color = " + ToStr((XdpTraceColor)theColor) +
        " for " + ToStr((XdpTraceAppTag)theAppTag));

    bool rc;

    // Verify the color
    rc = VerifyColor(theColor);
    if (rc == false) {
        return false;
    }

    rc = VerifyAppTag(theAppTag);
    if (rc == false) {
        return false;
    }

    // Calc the color mask
    UINT32 colorMask = 0;
    UINT32 col;

    for (col = theColor; col <= XDP_TRACE_COLOR_LAST ; col ++) {
        colorMask |= 1 << col;
    }

    // Update the colors
    UpdatePrintColors(theEnableFlag, colorMask, theAppTag);

    // All done
    return true;
}

// --- SetPrintBelowColors ----------------------------------------------------
//
// Enable or disable the print colors below the given color
//
// The command sets the status strings
//
bool XdpTrace::SetPrintBelowColors(bool theEnableFlag, UINT32 theColor) {

    return XdpTrace::SetPrintBelowColors(theEnableFlag, theColor, XDP_TRACE_APP_ALL);

}

bool XdpTrace::SetPrintBelowColors(bool theEnableFlag, UINT32 theColor, UINT32 theAppTag) {

    RN_TRACE(MASK_XDP, ItsLongName + " ::SetPrintBelowColors (" + XdpConv::ToHexStr(this) +
        ")  enable = " +XdpConv::ToStr(theEnableFlag) +
        " color = " + ToStr((XdpTraceColor)theColor) +
        " for " + ToStr((XdpTraceAppTag)theAppTag));

    bool rc;

    // Verify the color
    rc = VerifyColor(theColor);
    if (rc == false) {
        return false;
    }

    rc = VerifyAppTag(theAppTag);
    if (rc == false) {
        return false;
    }

    // Calc the color mask
    UINT32 colorMask = 0;
    UINT32 col;

    for (col = XDP_TRACE_COLOR_FIRST; col <= theColor ; col ++) {
        colorMask |= 1 << col;
    }

    // Update the colors
    UpdatePrintColors(theEnableFlag, colorMask, theAppTag);

    // All done
    return true;
}



// --- PrintEntry -------------------------------------------------------------
//
// Format and print the entry on the print output channel
//

void XdpTrace::PrintEntry(UINT32 theEntryHeader, // Includes color & line
                          UINT32 *theParamsP, UINT32 theParamsCount,
                          UINT8  *theBytesP,  UINT32 theBytesCount)

{

    bool rc;

	// If inside interrupt, we can't print, increase counter and return
	
    rc = XdpServices::InterruptsIsInside();

	if ( rc == true) {
		// Inside interrupt
    	ItsPrintInterruptDiscardCounter++;
    	return;
    }

    // We are not inside an interrupt, we can print

    RWCString s;

    // Get the entry count, if buffer is not on, set it to zero

    UINT32 entryCounter = ItsEntryCounter;

    if (ItsState != XDP_TRACE_STATE_ON) {
        // Buffer is on, zero the entry counter since it is not valid
        entryCounter = 0;
    }

    // If entryCounter >0 decrease one form it becuase it just has been increamented by the
    // buffer write,

    if (entryCounter > 0) {
        // Decrease the ebtry counter
        entryCounter--;
    }

    // Format the entry into a string

    s = FormatEntry(ItsPrintSeqNumber, entryCounter,
                    theEntryHeader,
                    theParamsP,  theParamsCount,
                    theBytesP,   theBytesCount);

    // Output the entry string

    PrintOutput(s.data());

    // Advance the print output sequence number

    ItsPrintSeqNumber++;


}

// --- PrintOutput ------------------------------------------------------------
//
// PrintOutput the string on the print output channel
//
void XdpTrace::PrintOutput(const char *theStringP) {

    bool rc = true;

    // Select the output channel
    switch (ItsPrintOutputChannel) {

        case XDP_TRACE_OUTPUT_UDP:
            // Print to UDP via the XDP services
            rc = ItsXdpServicesP->OutputToUdp(theStringP);
            if (rc == false) {
                ItsPrintOutputErrorCounter++;
            }
            break;

        case XDP_TRACE_OUTPUT_SERIAL:

            // Print to serial via the XDP services
            rc = ItsXdpServicesP->OutputToSerial(theStringP);
            if (rc == false) {
                ItsPrintOutputErrorCounter++;
            }
            break;

            //case  XDP_TRACE_OUTPUT_FILE:

        default:
            // unsuported channel
            ItsPrintOutputErrorCounter++;
            rc = false;
            break;

    } // switch

}

// --- SetPrintOutputChannel --------------------------------------------------
//
// Set the print output channel.
//
// If a failure occurs, the functions returns false and set the error string
//

bool XdpTrace::SetPrintOutputChannel(XdpTrace::XdpTraceOutput theOutput) {

    RN_TRACE(MASK_XDP, ItsLongName + " ::SetPrintOutputChannel (" + XdpConv::ToHexStr(this) +
                       ")  output = " + ToStr(theOutput) );

    bool rc;

    // Check theOutput is valid

    rc = VerifyOutputChannel(theOutput);
    if (rc == false) {
        return false;
    }

    // Valid, Set it

    ItsPrintOutputChannel = theOutput;

    // All done
    return true;
}


// --- UpdatePrintColors ------------------------------------------------------
//
// Update the print color flags for ALL or specific application tags according to (theAppTag)
//
// The buffer active flags are updated as well
//
bool XdpTrace::UpdatePrintColors(bool theEnableFlag, UINT32 theColorMask, UINT32 theAppTag) {


    // Clear the none flag

    theColorMask &= ~(1 << XDP_TRACE_COLOR_NONE);

    // Get current colors

    UINT32 appTag;
    UINT32 printColors[XDP_TRACE_APP_LAST]; // temporary

    //Initializing the array of color masks
    for (appTag = XDP_TRACE_APP_FIRST - 1; appTag <= XDP_TRACE_APP_LAST - 1; appTag++)
    {
        printColors[appTag] = ItsPrintActiveColors[appTag];
    }

    if (theAppTag == XDP_TRACE_APP_ALL) {

    // Update the  colors according to enable or disable

        for (appTag = XDP_TRACE_APP_FIRST - 1; appTag <= XDP_TRACE_APP_LAST - 1; appTag++)
        {
            if (theEnableFlag) {
                // Enable the colors
                printColors[appTag] |= theColorMask;
            }
            else {
                // Disable the colors
                printColors[appTag] &= ~theColorMask;
            }
        } // for
    } // if

    else {
        if (theEnableFlag) {
            // Enable the colors
            printColors[theAppTag - 1] |= theColorMask;
        }
        else {
            // Disable the colors
            printColors[theAppTag - 1] &= ~theColorMask;
        }
    }

    // Set the print color to the enabled colors
    SetPrintActiveColors(printColors);

    // Set info string
    SetInfoString("Enabled print colors:\n" + ColorMaskToStr(ItsPrintActiveColors));


    // All done OK

    return true;

}

// --- SetPrintActiveColors ---------------------------------------------------
// Set the print active colors (ItsPrintActiveColors).
// Also update the common active colors (ItsActiveColors) for ALL application tags !
//
void XdpTrace::SetPrintActiveColors(UINT32 theNewColors[])
{
    UINT32 appTag;

    for (appTag = XDP_TRACE_APP_FIRST - 1; appTag <= XDP_TRACE_APP_LAST - 1; appTag++)
    {
    	// Set the new flags
    	ItsPrintActiveColors[appTag] = theNewColors[appTag];

    	// Update the common active colors
    	_ItsActiveColors[appTag] = ItsBufferActiveColors[appTag] | ItsPrintActiveColors[appTag];
    }
}

// ============================================================================
// ===
// === Record & Entry core function
// ===
// ============================================================================

// --- AllocateRecord -----------------------------------------------------
// Allocate buffer record and return pointer to its begining
// The function return the buffer write pointer and advance it.
// In cyclic mode, when the pointer reaches the wrap point pointer it wraps
// to the begining.
// In non-cyclinc mode, when the pointer reaches the wrap pointer a record beyond
// the wrap point is returned and the buffer stops. It is OK to return this pointer
// since there is a reserved space between the wrap pointer to the end of the buffer.
//
// This function always returns valid pointer.
//
// Must be called with interrupts disabled (or buffer stopped)
//

UINT32 *XdpTrace::AllocateRecord()
{

	// Save the currnet write pointer. Its value will be returned

	UINT32 *originalWriteP = ItsBufferWriteP;

	// Check if wrap

	if (ItsBufferWriteP == ItsBufferWrapP) {
		// Wrap

		// Check cyclic buffer
		if (ItsCyclicFlag) {
			// Cyclic buffer, wrap buffer
			ItsBufferWriteP = ItsBufferBaseP;

			// Increase the wrap counter
			ItsWrapCounter++;
		}
		else {
			// Not cyclic buffer, contiue into the reserved space
			ItsBufferWriteP += XDP_TRACE_UINT32_IN_RECORD;

			// Stop the buffer becuase it is full (no new entries)
			SetBufferActiveColors((UINT32)0);

            // Mark buffer as inactive

            ItsBufferActiveFlag = false;

			// Mark the full stop flag;
			ItsFullStopFlag = true;
		}

	}
	else {
		// No wrap
		// Increase the write pointer
		ItsBufferWriteP += XDP_TRACE_UINT32_IN_RECORD;
	}

	// return the original write pointer

	return (originalWriteP);
}

// --- GetNextRecord ----------------------------------------------------------
//
// Returns a pointer to the next record if exists. If not, returns NULL
//
// Increase the wrap count if buffer wrap occured.
//
// Must be called with interrupts disabled (or buffer stopped)
//
UINT32 *XdpTrace::GetNextRecord(UINT32 *theRecordP, INT32 *theWrapCountP)
{

    UINT32 *p;

    // if we got pointer to the last record, the there is no "next" record
    if (theRecordP == ItsBufferWriteP) {
        // next record does not exists
        return NULL;
    }

    // Calc pointer to the next record
    p = theRecordP + XDP_TRACE_UINT32_IN_RECORD;

    // Wrap if cyclic and update wrap count
    if (ItsCyclicFlag && (p > ItsBufferWrapP) ) {
        p = ItsBufferBaseP;
        (*theWrapCountP)++;
    }

    // Record exists ?
    if (p == ItsBufferWriteP) {
        // next record does not exists
        return NULL;
    }

    // All Ok - Record exists
    return p;
}

// --- GetPrevRecord ----------------------------------------------------------
//
// Returns a pointer to the previous record if exists. If not, returns NULL
//
// Decrease the wrap count if buffer wrap occured.
//
// Must be called with interrupts disabled (or buffer stopped)
//
UINT32 *XdpTrace::GetPrevRecord(UINT32 *theRecordP, INT32 *theWrapCountP)
{

    UINT32 *p;

     // Calc pointer to the prev record
    p = theRecordP - XDP_TRACE_UINT32_IN_RECORD;

    // Is wrap to the beggining
    if (p < ItsBufferBaseP) {
        // Did the buffer wrapped at least one
        if (ItsWrapCounter > 0) {
            // Yes, wrap, the previous record it at the end of the buffer, the wrap point
            //   and update wrap count
            p = ItsBufferWrapP;
            (*theWrapCountP)--;

        }
        else {
            // Not wrapped, no previous record
            return NULL;
        }
    }

    // Record exists ?
    if (p == ItsBufferWriteP) {
        // Previous record does not exists
        return NULL;
    }

    // All Ok - Record exists
    return p;
}


// --- GetNextEntry -----------------------------------------------------------
//
// Find the next entry if exists and updates the theEntryP.
//
// At entry, *theEntryP pointer to the first record of the current entry
// At exit,  *theEntryP pointer to the first record of the next entry or NULL
//           if buffer ends
//
// Increase the wrap count if buffer wrap occured.
//
// Returns true if OK or false if invalid record is found.
//
// Must be called with interrupts disabled (or buffer stopped)
//
bool XdpTrace::GetNextEntry(UINT32 **theEntryP, INT32 *theWrapCountP)
{

    UINT32 *p = *theEntryP; // Current entry pointer

    UINT32 rec;

    // Search forward for record with the first flag set.
    // If more than ItsMaxRecordsInEntry records search, then we have an error

    for ( rec = 0 ; rec < ItsMaxRecordsInEntry ; rec++) {

        // Get next record
        p = GetNextRecord(p, theWrapCountP);

        // Record exists ?
        if (p == NULL) {
            // Not exist, return OK (no error) with NULL pointer
            *theEntryP = NULL;
            return true;
        }

        // Is it the first record of an entry
        if (*p & XDP_TRACE_RECORD_FIRST) {
            // New entry found, update the pointer and return OK
            *theEntryP = p;
            return true;
        }

    }

    // First entry not found with in ItsMaxRecordsInEntry, buffer error
    return false;

}

// --- ReadEntry --------------------------------------------------------------
//
// Validate cursor and read cursor entry parameters.

// Paramters are written to theParamsP array (only if cursor is valid)
//
// The theParamsP array must have room for XDP_TRACE_MAX_PARAMS_IN_ENTERY elements
//
// The entry header (the first UINT32 of the first record) is written to
// theEntryHeader
//
// The number of Parameter read is written to theParamCountP
//
// Cursor location is not changed in this function
//
// Returns true if OK or false if invalid record is found or cursor is invalid
//
// Sets the error string in case of error
//
bool XdpTrace::ReadEntry(UINT32 *theEntryHeader, UINT32 *theParamsP, UINT32 *theParamCountP) {


    bool rc;
    UINT32 interruptsStatus;  // For disable/restore interruprs

    // Disable interrups
    XDP_TRACE_DISABLE_INTERRUPTS(interruptsStatus);

    // Check cursor valid before doing something
    rc = IsCursorValid();

    // Cursor valid ?
    if (rc == false) {
        // Cursor not valid, restore interrups and handle error
        XDP_TRACE_RESTORE_INTERRUPTS(interruptsStatus);
        SetErrorString("Cursor not valid or lost, can't read the entry.");
        return false;
    }

    // Cursor is valid. Read the entry

    // Zero to paramter count
    *theParamCountP = 0;

    // Set the done flag to flase;
    bool doneFlag = false;

    // Get first record of cursor entry
    UINT32 *p = ItsCursorEntryP;

    UINT32 rec;

    // Read the paramters for all the records
    // If more than XDP_TRACE_MAX_PARAM_RECORDS_IN_ENTERY records search,
    // or we have an invalid record then we have an error

    for ( rec = 0 ; rec < XDP_TRACE_MAX_PARAM_RECORDS_IN_ENTERY ; rec++) {

        // Verify record is valid
        if ((p[0] & XDP_TRACE_RECORD_VALID) == 0) {
            // Record is not valid, break with error
            break;
        }

        // Is first record in entry ?
        if (rec == 0) {
            // First record, check first flag and get the entry flags
            if ((p[0] & XDP_TRACE_RECORD_FIRST) == 0 ) {
                // First record but the first flag is clear, break with error
                break;
            }

            // ... and get entry header
            *theEntryHeader = p[0];
        } // rec == 0

        // Copy record paramters, start from p+1 to skip the record header
        memcpy(theParamsP, p+1, (XDP_TRACE_UINT32_IN_RECORD-1) * sizeof(UINT32) );

        // advance parameters pointer & paramter count
        theParamsP      += (XDP_TRACE_UINT32_IN_RECORD-1);
        *theParamCountP += (XDP_TRACE_UINT32_IN_RECORD-1);

        // Is this record is the last ?
        if (p[0] & XDP_TRACE_RECORD_LAST) {
            // Last record, All done, set the done flag
            doneFlag = true;
            break;
        }

        // Not the last record, get next record
        INT32 dummyWrapCount;
        p = GetNextRecord(p, &dummyWrapCount);

        // Record exists ?
        if (p == NULL) {
            // Not exist. Error becuase we didn't get the last record
            break;
        }

    } // for rec

    // First of all, restore the interrupts
    XDP_TRACE_RESTORE_INTERRUPTS(interruptsStatus);

    // All done OK or error occured
    if (doneFlag == false) {
        // Not done -> error
        SetErrorString("Can't read the entry due to bad records. Cursor at 0x" +
                            XdpConv::ToHexStr(ItsCursorEntryP) + " rec=" + XdpConv::ToStr(rec) );
        return false;
    }

    // All done with no error :-)

    return true;

}


// --- GetPrevEntry -----------------------------------------------------------
//
// Find the previous entry if exists and updates the theEntryP.
//
// At entry, *theEntryP pointer to the first record of the current entry
// At exit,  *theEntryP pointer to the first record of the previous entry or NULL
//           if buffer ends
//
// At entry *theEntryP might also point to the invalid record just after the newest entry.
//
// Decrease the wrap count if buffer wrap occured.
//
// Returns true if OK or false if invalid record is found.
//
// Must be called with interrupts disabled (or buffer stopped)
//
bool XdpTrace::GetPrevEntry(UINT32 **theEntryP, INT32 *theWrapCountP)
{

    UINT32 *p = *theEntryP; // Current entry pointer

    UINT32 rec;

    // Search backward for record with the first flag set.
    // If more than ItsMaxRecordsInEntry records search, then we have an error

    for ( rec = 0 ; rec < ItsMaxRecordsInEntry ; rec++) {

        // Get previous record
        p = GetPrevRecord(p, theWrapCountP);

        // Record exists ?
        if (p == NULL) {
            // Not exist, return OK (no error) with NULL pointer
            *theEntryP = NULL;
            return true;
        }

        // Is it the first record of an entry
        if (*p & XDP_TRACE_RECORD_FIRST) {
            // New entry found, update the pointer and return OK
            *theEntryP = p;
            return true;
        }

    }

    // First entry not found with in ItsMaxRecordsInEntry, buffer error
    return false;

}

// --- GetEntryColor ----------------------------------------------------------
//
// Returns the color of the entry from the entry header
//

UINT32 XdpTrace::GetEntryColor(UINT32 theEntryHeader) {

    UINT32 v;

    v = theEntryHeader & XDP_TRACE_COLOR_MASK;

    return (v);
}

// --- GetEntryAppTag ----------------------------------------------------------
//
// Returns the application tag of the entry from the entry header
//

UINT32 XdpTrace::GetEntryAppTag(UINT32 theEntryHeader) {

    UINT32 v;

    v = (theEntryHeader >> XDP_TRACE_APP_SHIFT) & XDP_TRACE_APP_MASK;

    return (v);
}

// --- GetEntryLine -----------------------------------------------------------
//
// Returns the line number of the entry from the entry header
//

UINT32 XdpTrace::GetEntryLineNumber(UINT32 theEntryHeader) {

    UINT32 v;

    v = (theEntryHeader >> XDP_TRACE_LINE_NUM_SHIFT) & XDP_TRACE_LINE_NUM_MASK;

    return (v);
}

// --- ZeroAllRecords ---------------------------------------------------------
//
// Clear the buffers, sets all records to zero
//

void XdpTrace::ZeroAllRecords() {

    UINT32 i;

    UINT32 *p = ItsBufferBaseP;

    for( i = 0 ; i < ( ItsBufferSizeInRecords * XDP_TRACE_UINT32_IN_RECORD ) ; i++) {

        // Zero
        *p = 0;

        // next
        p++;

    }
}



// ============================================================================
// ===
// === Format functions
// ===
// ============================================================================

// --- SetFormat --------------------------------------------------------------
//
// Set the format of a specific fields
//
bool XdpTrace::SetFormat(XdpTrace::XdpTraceFormatField theFormatField, XdpTrace::XdpTraceFormat theFormat) {

    RN_TRACE(MASK_XDP, ItsLongName + " ::SetFormat (" + XdpConv::ToHexStr(this) +
        ")  formatField = " + ToStr(theFormatField) +
        " format = " + ToStr(theFormat) );

    bool rc;

    // Verify the format field
    rc = VerifyFormatField(theFormatField);
    if (rc == false) {
        return false;
    }

    // Verify the format
    rc = VerifyFormat(theFormat);
    if (rc == false) {
        return false;
    }

    switch (theFormatField) {

        case XDP_TRACE_FORMAT_FIELD_SEQ:         rc = SetFormatSeq(theFormat);          break;
        case XDP_TRACE_FORMAT_FIELD_NAME:        rc = SetFormatName(theFormat);         break;
        case XDP_TRACE_FORMAT_FIELD_ENTRY_NUM:   rc = SetFormatEntryNum(theFormat);     break;
        case XDP_TRACE_FORMAT_FIELD_COLOR:       rc = SetFormatColor(theFormat);        break;
        case XDP_TRACE_FORMAT_FIELD_APP_TAG:     rc = SetFormatAppTag(theFormat);       break; // Application tag
        case XDP_TRACE_FORMAT_FIELD_TASK:        rc = SetFormatTask(theFormat);         break;
        case XDP_TRACE_FORMAT_FIELD_TIME:        rc = SetFormatTime(theFormat);         break;
        case XDP_TRACE_FORMAT_FIELD_FILE:        rc = SetFormatFile(theFormat);         break;
        case XDP_TRACE_FORMAT_FIELD_FMT_STRING:  rc = SetFormatFmtString(theFormat);    break;
        case XDP_TRACE_FORMAT_FIELD_USER_PARAMS: rc = SetFormatUserParams(theFormat);   break;
        case XDP_TRACE_FORMAT_FIELD_USER_BYTES:  rc = SetFormatUserBytes(theFormat);    break;

        default:
            // Internal error
            rc = false;
            RN_ASSERT(0,"::SetFormat with invalid format");
            break;

    } // switch

    return true;

}

// --- FormatEntry ------------------------------------------------------------
//
// Return string of the entry data.
//

RWCString XdpTrace::FormatEntry(UINT32 theSeqNum, UINT32 theEntryNum,
                                UINT32 theEntryHeader, // Includes color & line
                                UINT32 *theParamsP, UINT32 theParamsCount,
                                UINT8  *theBytesP,  UINT32 theBytesCount)
{

    RWCString s;

    s += "^^";           // Mark entry begining
    s += FormatSeq       (theSeqNum);
    s += FormatName      ();
    s += FormatEntryNum  (theEntryNum);
    s += FormatColor     (theEntryHeader);
    s += FormatAppTag    (theEntryHeader);
    s += FormatTime      (theParamsP[XDP_TRACE_PARAMS_TIME]);
    s += FormatTask      (theParamsP[XDP_TRACE_PARAMS_TASK]);
    s += FormatFile      (theParamsP[XDP_TRACE_PARAMS_FMT_STRING], theEntryHeader);
    s += FormatFmtString (theParamsP[XDP_TRACE_PARAMS_FMT_STRING]);
    s += FormatUserParams(theParamsP[XDP_TRACE_PARAMS_FMT_STRING],
                          theParamsP+XDP_TRACE_PARAMS_USER,      // Skip internal params
                          theParamsCount-XDP_TRACE_PARAMS_USER); // Skip internal params
    s += FormatUserBytes (theBytesP, theBytesCount);

    s += "\n";

    return s;
}

// --- FormatSeq ---------------------------------------------------------
//
// Return string of the output sequence number according to the format variable
//
RWCString XdpTrace::FormatSeq(UINT32 theSeqNum)
{

    RWCString s;

    switch(ItsFormatSeq) {

    case XDP_TRACE_FORMAT_OFF:
        s = "";
        break;

    case XDP_TRACE_FORMAT_ON:
        s = "#" + XdpConv::ToStr(theSeqNum) + " ";
        s = XdpConv::Pad(s,7);
        break;

    case XDP_TRACE_FORMAT_HEX:
        s = "#" + XdpConv::ToHexStr(theSeqNum) + " ";
        break;

    default:
        s = "??seq?? ";
        break;
    }


    return s;
}

// --- FormatName ---------------------------------------------------------
//
// Return string of the trace name according to the format variable
//
RWCString XdpTrace::FormatName()
{

    RWCString s;

    switch(ItsFormatName) {

    case XDP_TRACE_FORMAT_OFF:
        s = "";
        break;

    case XDP_TRACE_FORMAT_ON:
        s = "[" + ItsShortName + "] ";
        break;

    case XDP_TRACE_FORMAT_LONG:
        s = "[" + ItsLongName + "] ";
        break;

    default:
        s = "??name?? ";
        break;
    }


    return s;
}
// --- FormatEntryNum ---------------------------------------------------------
//
// Return string of the entry number according to the format variable
//
RWCString XdpTrace::FormatEntryNum(UINT32 theEntryNum)
{

    RWCString s;

    switch(ItsFormatEntryNum) {

    case XDP_TRACE_FORMAT_OFF:
        s = "";
        break;

    case XDP_TRACE_FORMAT_ON:
        s = XdpConv::ToStr(theEntryNum, 5) + ": ";
        break;

    default:
        s = "??eventNum?? ";
        break;
    }


    return s;
}
// --- FormatColor ------------------------------------------------------------
//
// Return string of the entry color (header) according to the format variable
//

RWCString XdpTrace::FormatColor(UINT32 theEntryHeader)
{
    // Color number to single letter string convertor

    const static char *ColorMap[XDP_TRACE_COLOR_LAST+1] = {
        "n", // "None",
        "W", // "White",
        "G", // "Green",
        "Y", // "Yellow",
        "O", // "Orange",
        "R", // "Red",
        "B", // "Black"
    };


    RWCString s;

    // Get the entry color from the header

    UINT32 color = GetEntryColor(theEntryHeader);


    switch(ItsFormatColor) {

    case XDP_TRACE_FORMAT_OFF:
        s = "";
        break;

    case XDP_TRACE_FORMAT_ON: // Single letter color

        // Check color in range
        if (color <= XDP_TRACE_COLOR_LAST) {
            // In range, Convert the color to short form string
            s = RWCString(ColorMap[color]) + " ";
        }
        else {
            // Color not is range
            s = "e ";
            break;
        }


        break;


    case XDP_TRACE_FORMAT_LONG: // Full text color

        // Check color in range
        if (color <= XDP_TRACE_COLOR_LAST) {
            // In range, convert the color to full string
            s = ToStr( (XdpTraceColor) color );

        }
        else {
            // Color not is range, error
            s = "error?";
        }

        // Pad s

        s = XdpConv::Pad(s,-6) + " ";

        break;

    case XDP_TRACE_FORMAT_DEC: // Color number

        s = XdpConv::ToStr(color) + " ";

        break;

    case XDP_TRACE_FORMAT_HEX: // Trick, HEX means the entry header

        // Trick, HEX means the entry header
        s = XdpConv::ToHexStr(theEntryHeader) + " ";

        break;


    default:
        s = "??color?? ";
        break;
    }

    return s;
}

// --- FormatAppTag ------------------------------------------------------------
//
// Return string of the entry application tag (header) according to the format variable
//

RWCString XdpTrace::FormatAppTag(UINT32 theEntryHeader)
{
    // Color number to single/double letter string convertor
//
    const static char *AppTagMap[XDP_TRACE_APP_LAST] = {
        "D", // "APP default",
        "1", // "APP1",
        "2", // "APP2",
        "3", // "APP3",
        "4", // "APP4",
        "5", // "APP5",
        "6", // "APP6"
        "7", // "APP7",
        "8", // "APP8",
        "9", // "APP9",
        "10", // "APP10",
        "11", // "APP11",
        "12", // "APP12"
        "13", // "APP13",
        "14", // "APP14",
    };


    RWCString s;

    // Get the entry application tag from the header

    UINT32 appTag = GetEntryAppTag(theEntryHeader);


    switch(ItsFormatAppTag) {

    case XDP_TRACE_FORMAT_OFF:
        s = "";
        break;

    case XDP_TRACE_FORMAT_ON: // Single/double letter tag

        // Check application tag in range
        if (appTag <= XDP_TRACE_APP_LAST) {
            // In range, Convert the tag to short form string
            s = RWCString(AppTagMap[appTag]) + " ";
        }
        else {
            // Tag not is range
            s = "e ";
            break;
        }


        break;


    case XDP_TRACE_FORMAT_LONG: // Full text application tag

        // Check application tag in range
        if (appTag <= XDP_TRACE_APP_LAST) {
            // In range, convert the tag to full string
            s = ToStr( (XdpTraceAppTag) appTag );

        }
        else {
            // Application tag not is range, error
            s = "error?";
        }

        // Pad s

        s = XdpConv::Pad(s,-6) + " ";

        break;

    case XDP_TRACE_FORMAT_DEC: // Application tag number

        s = XdpConv::ToStr(appTag) + " ";

        break;

    case XDP_TRACE_FORMAT_HEX: // Trick, HEX means the entry header

        // Trick, HEX means the entry header
        s = XdpConv::ToHexStr(theEntryHeader) + " ";

        break;


    default:
        s = "??appTag?? ";
        break;
    }

    return s;
}

// --- FormatEntryTask --------------------------------------------------------
//
// Return string of the entry task according to the format variable
//
RWCString XdpTrace::FormatTask(UINT32 theTaskStamp)
{

    RWCString s;

    switch(ItsFormatTask) {

    case XDP_TRACE_FORMAT_OFF:
        s = "";
        break;

    case XDP_TRACE_FORMAT_ON: // Task format

        s = XdpConv::Pad(ItsXdpServicesP->TaskStampToString(theTaskStamp), -6 ) + " ";

        break;

    case XDP_TRACE_FORMAT_HEX: // Task in hex

        s = XdpConv::ToHexStr(theTaskStamp) + " ";

        break;


    default:
        s = "??task?? ";
        break;
    }

    return s;
}


// --- FormatEntryTime --------------------------------------------------------
//
// Return string of the entry time stamp according to the format variable
//
RWCString XdpTrace::FormatTime(UINT32 theTimeStamp)
{

    RWCString s;
    double entryTime;

    switch(ItsFormatTime) {

    case XDP_TRACE_FORMAT_OFF:
        s = "";
        break;

    case XDP_TRACE_FORMAT_ON: // Time format, display resolution is 1000 * required resolution

        entryTime = GetEntryTime(theTimeStamp);
        s = XdpConv::ToUnitsStr(entryTime, 8 ,ItsTimeStampRequiredResolution*1000) + " ";

        break;


    case XDP_TRACE_FORMAT_LONG: // Time format, display resolution is the required resolution

        entryTime = GetEntryTime(theTimeStamp);
        s = XdpConv::ToUnitsStr(entryTime, 8+3 ,ItsTimeStampRequiredResolution) + " ";


        break;

    case XDP_TRACE_FORMAT_DEC: // Time in decimal

        s = XdpConv::ToStr(theTimeStamp,10) + " ";

        break;


    default:
        s = "??Time - " + ToStr(ItsFormatTime) +" ??";
        break;
    }

    return s;
}

// --- FormatFile ------------------------------------------------------------
//
// Return string of the entry filename and line number according to the format
// variable & Width.
//
// Filename is derived from the format string. Line number is derived from
// the Entery header
//
RWCString XdpTrace::FormatFile(UINT32 theFmtString, UINT32 theEntryHeader)
{


    RWCString s;

    UINT32 len;          // The finename length in the format string
    const char *fmtStr = (const char *) theFmtString;

    // Check null pointer
    if (fmtStr == NULL) {
        // NULL, set to NULL text
        fmtStr = "!!NULL!!";
    }

    RWCString fullName; // Full filename extracted from the format string

    UINT32 lineNumber;

    switch(ItsFormatFile) {

    case XDP_TRACE_FORMAT_OFF:
        s = "";
        break;

    case XDP_TRACE_FORMAT_ON: // Base file name and line number

        // Get the file name length in fmtStr

        len = GetFmtStringFileNameLength(fmtStr);

        // Get the file name
        // Check file name exists
        if (len > 0) {
            // File name exists, build string of the file name (in a dumm way)
            fullName="";
            fullName.append( fmtStr, (unsigned int)len);
        }
        else {
            // No filename
            fullName = "none";
        }

        // Take only the base name

        s = XdpConv::ToBaseFilename(fullName.data() );

        // Get line number
        lineNumber = GetEntryLineNumber(theEntryHeader);

        // Add the line number and add ( )
        s += "(" + XdpConv::ToStr(lineNumber) + ")";

        // Pad it

        s = XdpConv::Pad(s,-(INT32)ItsFormatFileWidth) + " ";

        break;


    case XDP_TRACE_FORMAT_LONG: // Full file name

        // Get the file name length in fmtStr

        len = GetFmtStringFileNameLength(fmtStr);

        // Get the file name
        // Check file name exists
        if (len > 0) {
            // File name exists, build string of the file name (in a dumm way)
            fullName="";
            fullName.append( fmtStr, (unsigned int) len);
        }
        else {
            // No filename
            fullName = "none";
        }

        // Assign the filename
        s = fullName;

        // Get line number
        lineNumber = GetEntryLineNumber(theEntryHeader);

        // Add the line number and add ( )
        s += "(" + XdpConv::ToStr(lineNumber) + ")";

        // Pad it

        s = XdpConv::Pad(s,-(INT32)ItsFormatFileWidth) + " ";

        break;

    default:
        s = "??file?? ";
        break;
    }

    return s;
}


// --- FormatFmtString --------------------------------------------------------
//
// Return string of the entry format string according to the format variable
//
RWCString XdpTrace::FormatFmtString(UINT32 theFmtString)
{


    RWCString s;

    switch(ItsFormatFmtString) {

    case XDP_TRACE_FORMAT_OFF:
        s = "";
        break;

    case XDP_TRACE_FORMAT_ON: // The format string


        // Get string pointer, handle NULL
        if (theFmtString == NULL) {
            // NULL, set to NULL text
            s = "!!NULL!!";
        }
        else {
            // Get the real format string
            s = GetFmtStringOnly((const char*)theFmtString);
        }

        // Add "" to it
        s = "\"" + s + "\" ";

        break;


    case XDP_TRACE_FORMAT_HEX: // Pointer to the format string

        s = XdpConv::ToHexStr(theFmtString) + " ";

        break;

    default:
        s = "??fmtStr?? ";
        break;
    }

    return s;
}

// --- FormatUserParams -------------------------------------------------------
//
// Return string of the entry users params according to the format variable
//
RWCString XdpTrace::FormatUserParams(UINT32 theFmtString, UINT32 *theParamsP, UINT32 theCount)
{


    RWCString s;
    UINT32 i;

    char        *buf = NULL;        // sprint buffer
    UINT32      bufferSize = 2048;  // sprintf buffer size
    const char  *fmt;               // format string
    UINT32      len;                // formatted string length


    switch(ItsFormatUserParams) {

    case XDP_TRACE_FORMAT_OFF:
        s = "";
        break;

    case XDP_TRACE_FORMAT_ON: // The format the user parameters using the fomrat string


        // Get string pointer, handle NULL
        if (theFmtString == NULL) {
            // NULL, set to NULL text
            s = "!!NULL!! ";
            break;
        }

        // Get the format string (and only it)
        fmt = GetFmtStringOnly((const char*)theFmtString);

        // Allocate buffer for sprintf
        buf = new char[bufferSize];

        // Check no buffer
        if (buf == NULL) {
            // No buffer, set text to error
            s = "!!Failed to get buffer for sprintf!! ";
            break;
        }

        // Lets do it - sprintf

        len = sprintf(buf,fmt,theParamsP[0],  theParamsP[1],  theParamsP[2],  theParamsP[3],
                              theParamsP[4],  theParamsP[5],  theParamsP[6],  theParamsP[7],
                              theParamsP[8],  theParamsP[9],  theParamsP[10], theParamsP[11],
                              theParamsP[12], theParamsP[13], theParamsP[14], theParamsP[15]);

        // Check sprintf overflowed

        if (len > bufferSize - 5) {
            // overflowed, oopssssssss
            s = "!!!! Oops - sprintf buffer overflowed - memory currupted !! len=" +
                XdpConv::ToStr(len);
            break;
        }

        // All OK - assign buffer to s

        s = "- " + RWCString(buf) + " ";

        break;

    case XDP_TRACE_FORMAT_HEX: // Dump paramters in hex

        for(i=0 ; i < theCount; i++) {

            // new line
            if (i % 8 == 0) {
                s += "\n  ";
            }

            // Convert parameter to string
            s+= XdpConv::ToHexStr(theParamsP[i]) + " ";

        }

        break;

    case XDP_TRACE_FORMAT_UINT32: // Dump paramters in decimal (unsigned)

        for(i=0 ; i < theCount; i++) {

            // new line
            if (i % 8 == 0) {
                s += "\n  ";
            }

            // Convert parameter to string
            s+= XdpConv::ToStr((UINT32)(theParamsP[i]), 8) + " ";

        }

        break;

    case XDP_TRACE_FORMAT_INT32: // Dump paramters in decimal (signed)

        for(i=0 ; i < theCount; i++) {

            // new line
            if (i % 8 == 0) {
                s += "\n  ";
            }

            // Convert parameter to string
            s+= XdpConv::ToStr((INT32)(theParamsP[i]), 8) + " ";

        }

        break;

    case XDP_TRACE_FORMAT_APP1: // Application specific format 1

        s =  FormatUserParamsApp1(theFmtString, theParamsP, theCount);

        break;

    case XDP_TRACE_FORMAT_APP2: // Application specific format 2

        s =  FormatUserParamsApp2(theFmtString, theParamsP, theCount);

        break;

    case XDP_TRACE_FORMAT_APP3: // Application specific format 3

        s =  FormatUserParamsApp3(theFmtString, theParamsP, theCount);

        break;


    default:
        s = "??userParams?? ";
        break;
    }

    // if buffer is allocated, free it
    if (buf != NULL) {
        // Buffer allocated, free it
        delete[] buf;
    }

    // Done

    return s;

}
// --- FormatUserBytes -------------------------------------------------------
//
// Return string of the entry user bytes according to the format variable
//
// Not supported yet !
//
RWCString XdpTrace::FormatUserBytes(UINT8  *theBytesP, UINT32 theCount)
{

    RWCString s;

    switch (ItsFormatUserBytes) {

    case XDP_TRACE_FORMAT_OFF:
        s = "";
        break;

    default:
        s = "??userBytes?? ";
        break;
    }

    // Done

    return s;

}

// --- FormatUserParamsApp1 ---------------------------------------------------
//
// Return string of the entry user paramters with application specific format 1
//
// This function is not implemented in this class, it return an error string.
// It should be implemented in application specific derived classes
//

RWCString XdpTrace::FormatUserParamsApp1(UINT32 , UINT32 *, UINT32) {

    RWCString s="FormatUserParamsApp1 not supported in this application type ";

    return s;
}

// --- FormatUserParamsApp2 ---------------------------------------------------
//
// Return string of the entry user paramters with application specific format 2
//
// This function is not implemented in this class, it return an error string.
// It should be implemented in application specific derived classes
//

RWCString XdpTrace::FormatUserParamsApp2(UINT32 , UINT32 *, UINT32) {

    RWCString s="FormatUserParamsApp2 not supported in this application type ";

    return s;
}

// --- FormatUserParamsApp3 ---------------------------------------------------
//
// Return string of the entry user paramters with application specific format 3
//
// This function is not implemented in this class, it return an error string.
// It should be implemented in application specific derived classes
//

RWCString XdpTrace::FormatUserParamsApp3(UINT32 , UINT32 *, UINT32) {

    RWCString s="FormatUserParamsApp3 not supported in this application type ";

    return s;
}


// --- GetFmtStringFileNameLength ---------------------------------------------
//
// Returns the length of the file name in the (format) string
//
// Search for "~". It it is not found, return zero (assume no file name)
//
UINT32 XdpTrace::GetFmtStringFileNameLength(const char *theFmtString)
{

    UINT32 len = 0;

    const char *p=theFmtString;

    // Search until end of string

    while( *p != '\0') {

        // Is it the "~"
        if (*p == '~') {
            // Yes, end of file name
            return len;
        }

        // Continue the search, next char
        p++;
        len++;
    }

    return 0;


}

// --- GetFmtStringOnly -------------------------------------------------------
//
// Returns the pointer (in side the given string) to the begging of the format
// string.
//
//
const char *XdpTrace::GetFmtStringOnly(const char *theFmtString) {


    UINT32 filenameLen = GetFmtStringFileNameLength(theFmtString);


    // Get pointer after the file name
    const char *p = theFmtString + filenameLen;


    // If we hit the "~", skip it

    if ( *p == '~') {
        // Skip the "~"
        p++;
    }

    // Done
    return (p);

}

// --- VerifyFormat ------------------------------------------------------------
//
// Verify theFormat is a valic format. If not set the error string and return false
//
bool XdpTrace::VerifyFormat(XdpTrace::XdpTraceFormat theFormat)
{

    // Check color
    if ((theFormat < XDP_TRACE_FORMAT_NEXT) ) {

        // Format OK
        return true;
    }

    // Bad format

    SetErrorString("The value " + XdpConv::ToStr((UINT32)theFormat) + " is invalid format value.");

    return false;

}

// --- VerifyFormatField ------------------------------------------------------
//
// Verify theFormatField is a valic format field. If not set the error string and return false
//
bool XdpTrace::VerifyFormatField(XdpTrace::XdpTraceFormatField theFormatField)
{

    // Check color
    if ((theFormatField < XDP_TRACE_FORMAT_FIELD_NEXT) ) {

        // Format OK
        return true;
    }

    // Bad format

    SetErrorString("The value " + XdpConv::ToStr((UINT32)theFormatField) + " is invalid format field value.");

    return false;

}

// ============================================================================
// ===
// === Information (Task & time) (protected)
// ===
// ============================================================================

// --- SetTimeStampResolution -------------------------------------------------
//
// Set the time stamp resulotion and time stamp parameter using XDP time stamp
// service.
//
// If the required resolution is not avaliable, the neatest better resolution
// is used
//
// return true if OK
//

bool XdpTrace::SetTimeStampResolution(double theRequiredResolution) {

    RN_TRACE(MASK_XDP, ItsLongName + " ::SetTimeStampResolution (" + XdpConv::ToHexStr(this) +
                       ")  theRequiredResolution = " + XdpConv::ToUnitsStr(theRequiredResolution) + "Sec" );

    bool rc;

    // Verify off state;
    rc = VerifyStateOff();
    if ( rc == false ) {
        return false;
    }

    // State is off

    // Save the required resolution

    ItsTimeStampRequiredResolution = theRequiredResolution;

    // Get the time stamp paramter for the required resolution

    ItsTimeStampParameter = ItsXdpServicesP->TimeStampResolutionToParameter(theRequiredResolution);

    // Get the real resolution from the time stamp parameter

    ItsTimeStampRealResolution = ItsXdpServicesP->TimeStampParameterToResolution(ItsTimeStampParameter);

    RN_TRACE(MASK_XDP, ItsLongName + " ::SetTimeStampResolution (" + XdpConv::ToHexStr(this) +
                       ")  realResolution = " + XdpConv::ToUnitsStr(ItsTimeStampRealResolution) + "Sec" );

    // All done OK
    return true;
}

// --- GetStartTime -----------------------------------------------------------
//
// Returns the start time, the time when the buffer has been started
//

double XdpTrace::GetStartTime() {


    double startTime;

    // Convert the start time stamp to the start time
    startTime = ItsStartTimeStamp * ItsTimeStampRealResolution;

    // All done
    return startTime;
}


// --- GetEntryAbsoluteTime ---------------------------------------------------
//
// Returns the absolute time of an entry time stamp. The returned value does
// not consider the time offset
//

double XdpTrace::GetEntryAbsoluteTime(UINT32 theTimeStamp) {

    double timeStampAsDouble;
    double entryTime;

    // Convert the time start to a double

    timeStampAsDouble = theTimeStamp;

    // Check if the time stamp has been wrapped since buffer has been started
    // If so, add 2^32

    if (theTimeStamp < ItsStartTimeStamp) {
        // Wrapped, add 2^32 by adding 2^31 twice to prevent overflow problems
        timeStampAsDouble += (UINT32) 1 << 31;  // Half of 2^32
        timeStampAsDouble += (UINT32) 1 << 31;  // Second half
    }

    // Convert the time stamp into a time
    entryTime = timeStampAsDouble * ItsTimeStampRealResolution;

    // All done
    return entryTime;
}

// --- GetEntryTime -----------------------------------------------------------
//
// Returns the relative time (with time offset) of an entry time stamp.
// The returned value does consider the time offset
//

double XdpTrace::GetEntryTime(UINT32 theTimeStamp) {

    double entryTime;

    // Add the entry absolute time with the time offset
    entryTime = GetEntryAbsoluteTime(theTimeStamp) - ItsTimeOffset;

    // All done
    return entryTime;
}

// ============================================================================
// ===
// === Relief functions
// ===
// ============================================================================

// --- SeekCursorTaskRelief ---------------------------------------------------
//
// Call from SeekCursor every ItsSeekTaskReliefNumber iterations to break the
// task execution sequence.
//
// For now, sleep for 10mSec
//

void XdpTrace::SeekCursorTaskRelief()
{

    OSTimeUtil::Sleep(10);
}

// ============================================================================
// ===
// === To string functions
// ===
// ============================================================================

// --- ToStr(XdpTraceState) ---------------------------------------------------
//
// Returns the state as string
//
RWCString XdpTrace::ToStr(XdpTrace::XdpTraceState theState, INT32 theWidth) {

    const static char *statesMap[] = { "Off" , "On" , "Recover" };

    // Check out of range
    if ( theState >= sizeof(statesMap)/sizeof(statesMap[0])) {
        // Out of range
        return "!! Internal error. state ToStr out of range. val=" +
                XdpConv::ToStr((UINT32)theState) + "!!";
    }

    // OK, convert to string

    RWCString s = XdpConv::Pad( statesMap[(UINT32)theState], theWidth) ;

    return s;

}

// --- ToStr(XdpTraceColor) ---------------------------------------------------
//
// Returns the color as string
//
RWCString XdpTrace::ToStr(XdpTraceColor theColor, INT32 theWidth) {


    const static char *colorsMap[] ={"None", "White", "Green", "Yellow", "Orange",
                                     "Red", "Black"};

    // Check out of range
    if ( theColor >= sizeof(colorsMap)/sizeof(colorsMap[0])) {
        // Out of range
        return "!! Internal error. color ToStr out of range. val=" +
                XdpConv::ToStr((UINT32)theColor) + "!!";
    }

    // OK, convert to string

    RWCString s = XdpConv::Pad( colorsMap[(UINT32)theColor], theWidth) ;

    return s;

}

// --- ToStr(XdpTraceAppTag) ---------------------------------------------------
//
// Returns the application tag as string
//
RWCString XdpTrace::ToStr(XdpTraceAppTag theAppTag, INT32 theWidth) {


    const static char *appTagMap[] ={"None",
                                     "AppD",
                                     "App1",
                                     "App2",
                                     "App3",
                                     "App4",
                                     "App5",
                                     "App6",
                                     "App7",
                                     "App8",
                                     "App9",
                                     "App10",
                                     "App11",
                                     "App12",
                                     "App13",
                                     "App14"};

    // Check out of range
    if ( theAppTag >= sizeof(appTagMap)/sizeof(appTagMap[0])) {
        // Out of range
        return "!! Internal error. application tag ToStr out of range. val=" +
                XdpConv::ToStr((UINT32)theAppTag) + "!!";
    }

    // OK, convert to string

    RWCString s = XdpConv::Pad( appTagMap[(UINT32)theAppTag], theWidth) ;

    return s;

}

// --- ColorMaskToStr ---------------------------------------------------------
//
// Returns a string of the enable colors in the mask
//
RWCString XdpTrace::ColorMaskToStr(UINT32 theColorMask[]) {

    RWCString s;

     // Check all color are disable
    if (IsAllColorsDisabled(theColorMask) == true) {

        // All colors are disabled
        s = "None\n";
    }
    else {
        // At least one color is enabled

        s = "";

        // Scan all colors of ALL application tags

        UINT32 appTag;

        for (appTag = XDP_TRACE_APP_FIRST; appTag <= XDP_TRACE_APP_LAST; appTag++) {

        UINT32 color;

            s += ToStr((XdpTraceAppTag) appTag) + ":";

        for (color = XDP_TRACE_COLOR_FIRST; color <= XDP_TRACE_COLOR_LAST; color++) {

            // Color i enabled ?

                if ( ( theColorMask[appTag - 1] & (1 << color) ) != 0 ){

                // Enabled, add to the list
                s += ToStr((XdpTraceColor) color ) + " ";
            }

            } // for - color

            s += ".\n";
        } //for - app
    } // else


    // return it
    return s;
}

// --- AppTagMaskToStr ---------------------------------------------------------
//
// Returns a string of the enable application tags in the mask
//
RWCString XdpTrace::AppTagMaskToStr(UINT32 theAppTagMask) {

    RWCString s;

     // Check all tags are disable
    if (theAppTagMask == 0) {

        // All tags are disabled
        s = "None\n";
    }
    else {
        // At least one tag is enabled

        s = "";

        // Scan all application tags

        UINT32 appTag;

        for (appTag = XDP_TRACE_APP_FIRST; appTag <= XDP_TRACE_APP_LAST; appTag++) {

            // Tag i enabled ?

            if ( ( theAppTagMask & (1 << appTag) ) != 0 ){

                // Enabled, add to the list
                s += ToStr((XdpTraceAppTag) appTag ) + " ";
            }

        } // for

    } // else


    // return it
    return s;
}

// --- ToStr(XdpTraceOutput) --------------------------------------------------
//
// Returns a output channel as string
//
RWCString XdpTrace::ToStr(XdpTrace::XdpTraceOutput  theOutput, INT32 theWidth) {

    const static char *outputMap[] ={"CmdMngr", "UDP" , "Serial", "File"};

    // Check out of range
    if ( theOutput >= sizeof(outputMap)/sizeof(outputMap[0])) {
        // Out of range
        return "!! Internal error. output ToStr out of range. val=" +
                XdpConv::ToStr((UINT32)theOutput) + "!!";
    }

    // OK, convert to string

    RWCString s = XdpConv::Pad( outputMap[(UINT32)theOutput], theWidth) ;

    return s;

}

// --- ToStr(XdpTraceFormat) --------------------------------------------------
//
// Returns the format as string
//
RWCString XdpTrace::ToStr(XdpTrace::XdpTraceFormat theFormat, INT32 theWidth) {

    const static char *formatMap[] = {
          "Off",    // XDP_TRACE_FORMAT_OFF
          "On",     // XDP_TRACE_FORMAT_ON
          "Long",   // XDP_TRACE_FORMAT_LONG
          "Hex",    // XDP_TRACE_FORMAT_HEX
          "Dec",    // XDP_TRACE_FORMAT_DEC
          "HEX8",   // XDP_TRACE_FORMAT_HEX8
          "INT8",   // XDP_TRACE_FORMAT_INT8
          "UINT8",  // XDP_TRACE_FORMAT_UINT8
          "HEX16",  // XDP_TRACE_FORMAT_HEX16
          "INT16",  // XDP_TRACE_FORMAT_INT16
          "UINT16", // XDP_TRACE_FORMAT_UINT16
          "HEX32",  // XDP_TRACE_FORMAT_HEX32
          "INT32",  // XDP_TRACE_FORMAT_INT32
          "UINT32", // XDP_TRACE_FORMAT_UINT32
          "App1",   // XDP_TRACE_FORMAT_APP1
          "App2",   // XDP_TRACE_FORMAT_APP2
          "App3"    // XDP_TRACE_FORMAT_APP3
    };

    // Check out of range
    if ( theFormat >= sizeof(formatMap)/sizeof(formatMap[0])) {
        // Out of range
        return "!! Internal error. format ToStr out of range. val=" +
                XdpConv::ToStr((UINT32)theFormat) + "!!";
    }

    // OK, convert to string

    RWCString s = XdpConv::Pad( formatMap[(UINT32)theFormat], theWidth) ;

    return s;

}


// --- ToStr(XdpTraceFormatField) ---------------------------------------------
//
// Returns the format field as string
//

RWCString XdpTrace::ToStr(XdpTrace::XdpTraceFormatField  theField, INT32 theWidth) {

    const static char *formatMap[] = {
        "Sequance",     // XDP_TRACE_FORMAT_FIELD_SEQ
        "Name",         // XDP_TRACE_FORMAT_FIELD_NAME
        "EntryNumber",  // XDP_TRACE_FORMAT_FIELD_ENTRY_NUM
        "Color",        // XDP_TRACE_FORMAT_FIELD_COLOR
        "Task",         // XDP_TRACE_FORMAT_FIELD_TASK
        "Time",         // XDP_TRACE_FORMAT_FIELD_TIME
        "File",         // XDP_TRACE_FORMAT_FIELD_FILE
        "FmtString",    // XDP_TRACE_FORMAT_FIELD_FMT_STRING
        "User params",  // XDP_TRACE_FORMAT_FIELD_USER_PARAMS
        "User bytes",   // XDP_TRACE_FORMAT_FIELD_USER_BYTES
   };

    // Check out of range
    if ( theField >= sizeof(formatMap)/sizeof(formatMap[0])) {
        // Out of range
        return "!! Internal error. format field ToStr out of range. val=" +
                XdpConv::ToStr((UINT32)theField) + "!!";
    }

    // OK, convert to string

    RWCString s = XdpConv::Pad( formatMap[(UINT32)theField], theWidth) ;

    return s;

}


// --- ToStr(XdpTraceBuffer) --------------------------------------------------
//
// Returns the buffer type as string
//
RWCString XdpTrace::ToStr(XdpTrace::XdpTraceBuffer theBufferType, INT32 theWidth) {

    const static char *bufferTypeMap[] = {
          "Heap",        // XDP_TRACE_BUFFER_HEAP
          "EXT_SRAM",    // XDP_TRACE_BUFFER_EXT_SRAM
          "CPU_SRAM"     // XDP_TRACE_BUFFER_CPU_SRAM
    };

    // Check out of range
    if ( theBufferType >= sizeof(bufferTypeMap)/sizeof(bufferTypeMap[0])) {
        // Out of range
        return "!! Internal error. buffer type ToStr out of range. val=" +
                XdpConv::ToStr((UINT32)theBufferType) + "!!";
    }

    // OK, convert to string

    RWCString s = XdpConv::Pad( bufferTypeMap[(UINT32)theBufferType], theWidth) ;

    return s;

}

// ============================================================================
// ===
// === Error & info functions (support functions)
// ===
// ============================================================================

// --- ClearStatusStrings -----------------------------------------------------
//
// Clears the error & info strings
//

void XdpTrace::ClearStatusStrings() {

    ItsErrorString = "";
    ItsInfoString  = "";

}

// --- GetStatusStrings -------------------------------------------------------
//
// Return the error and info strings
//

RWCString XdpTrace::GetStatusStrings() {

    RWCString s = ItsErrorString + ItsInfoString;

    return s;

}


// --- SetErrorString ---------------------------------------------------------
//
// Appends the error string
//

void XdpTrace::SetErrorString(RWCString theError) {

    ItsErrorString += "Error: " + theError + "\n";

    RN_TRACE(MASK_XDP, ItsLongName + "(" + XdpConv::ToHexStr(this) +
                       ") Error: " + theError);
 }

// --- SetInfoString ----------------------------------------------------------
//
// Appends the info string
//

void XdpTrace::SetInfoString(RWCString theInfo) {

    ItsInfoString += "Info: " + theInfo + "\n";

    RN_TRACE(MASK_XDP, ItsLongName + "(" + XdpConv::ToHexStr(this) +
                       ") Info: " + theInfo);
}



// EOF
