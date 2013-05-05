/* XdpTraceUI.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpTraceUI

   This class process all the user command from the command managers

AUTHOR
   Gaash, SB


**************************************************************************/

#ifndef XdpTraceUI_h
#define XdpTraceUI_h

#include "basictypes.h"
#include "RWHdr/cstring.h"
#include "XdpParse.h"
#include "XdpParseEnum.h"

//----------------------------
// Forward Declarations
//----------------------------

class XdpTrace;
class CmdMngr;

class XdpTraceUI
{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------


// Constructor & Destructor

    XdpTraceUI();

// Miscellaneous Operations

    void ProcessCommand(XdpTrace *theTraceP, CmdMngr *theCmdMngrP);

    void PrintString(const RWCString &theString);

//-------------------------------------------------------------------------
protected:
//-------------------------------------------------------------------------


// The commands functions

    // Display commands
    bool CommandDisplay();
    bool CommandEnteries();
    bool CommandOutput();
    bool CommandRelief();
    bool CommandTimeOffset();

    // Cursor commands
    bool CommandCursor();
    bool CommandSeek();
    bool CommandBegining();
    bool CommandEnd();

    // Color commands

    bool CommandColors();
    bool CommandAboveColor();
    bool CommandBelowColor();

    // Application filter

    bool CommandAppTags();
    bool CommandPrintAppTags();
    bool CommandAppColors();
    bool CommandAppPrintColors();

    // Action commands
    bool CommandOff();
    bool CommandOn();
    bool CommandStart();
    bool CommandStop();
    bool CommandResume();
    bool CommandSave();
    bool CommandPrintFiles();
    bool CommandDeleteFile();
    bool CommandShowTrace();

    // Format commands

    bool CommandFormat();
    bool CommandFormatShow();
    bool CommandFormatSeq();
    bool CommandFormatName();
    bool CommandFormatEntryNumber();
    bool CommandFormatColor();
    bool CommandFormatAppTag();
    bool CommandFormatTask();
    bool CommandFormatTime();
    bool CommandFormatFile();
    bool CommandFormatString();
    bool CommandFormatParams();
    bool CommandFormatBytes();

    // Setup
    bool CommandBuffer();
    bool CommandCyclic();
    bool CommandResolution();

    // Write
    bool CommandWrite();


    // Print commands
    bool CommandPrintColors();
    bool CommandPrintAboveColor();
    bool CommandPrintBelowColor();
    bool CommandPrintOutput();
    bool CommandPrintRelief();

    // Status commands
    bool CommandStatus();
    bool CommandInternalStatus();
    bool CommandCounters();

// Miscellaneous Operations

    bool GetAndExecuteCommand();
    bool ShowCursorEntry();
    void ShowColorsStatus(UINT32 *theColorMask);
    void ShowAppTagsStatus(UINT32 theAppTagMask);

    //TESTING
    bool CommandTest();

// --- Data members -----------------------------------------------------------

    // Object pointers

    XdpTrace    *ItsTraceP;
    CmdMngr     *ItsCmdMngrP;
    XdpParse    *ItsParserP;

    // Static
    static const XdpParse::XdpParseSubCommand  TheirSubCommands[]; // For XdpParse

    static const XdpParseEnum::XdpParseEnumPair TheirColorEnumPairs[]; // For xxColor commands

    static const XdpParseEnum::XdpParseEnumPair TheirAppTagEnumPairs[]; // For xxAppTag commands

    static const XdpParseEnum::XdpParseEnumPair TheirOutputEnumPairs[]; // For display & output
    static const XdpParseEnum::XdpParseEnumPair TheirPrintOutputEnumPairs[]; // For print output


//-----------------------------------------------------------------------------
private:
//-----------------------------------------------------------------------------

// --- dtor, copy ctor, operator = --------------------------------------------
//
// dtor, copy ctor & operator = are declared private and has no implemenation
// this procted from calling them

    // dtor
    ~XdpTraceUI();

    // Copy ctor
    XdpTraceUI(const XdpTraceUI &);

    // Operator =
    XdpTraceUI & operator=( const XdpTraceUI) ;

    // This declaration will supress compilation warnings
    friend void Dummy();

};

#endif // XdpTraceUI_h


