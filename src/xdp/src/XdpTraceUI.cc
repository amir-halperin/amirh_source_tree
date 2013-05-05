/* XdpTraceUI.cc */

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

#include "Error.h"

#include "CmdMngr.h"  // For PrintString

#include "XdpConv.h"
#include "XdpParse.h"
#include "XdpParseNumber.h"
#include "XdpParseString.h"
#include "XdpParseBool.h"
#include "XdpParseEnum.h"
#include "XdpParseDoubleUnits.h"

#include "XdpTraceUI.h"
#include "XdpTrace.h"

extern const MaskFlagId MASK_XDP;


// --- Commands enum ----------------------------------------------------------
// Local enum for commands

enum { // Display
       XDP_TRACE_UI_CMD_DISPLAY,
       XDP_TRACE_UI_CMD_ENTRIES, 
       XDP_TRACE_UI_CMD_OUTPUT,
       XDP_TRACE_UI_CMD_RELIEF,

       // Cursor
       XDP_TRACE_UI_CMD_CURSOR,
       XDP_TRACE_UI_CMD_SEEK,
       XDP_TRACE_UI_CMD_BEGINING,
       XDP_TRACE_UI_CMD_END,

       // Colors
       XDP_TRACE_UI_CMD_COLORS,
       XDP_TRACE_UI_CMD_ABOVE_COLOR,
       XDP_TRACE_UI_CMD_BELOW_COLOR,

       //Application filter
       XDP_TRACE_UI_CMD_APPTAGS,
       XDP_TRACE_UI_CMD_APP_COLORS,
       XDP_TRACE_UI_CMD_APP_PRINT_COLORS,
       XDP_TRACE_UI_CMD_PRINT_APPTAGS,

       // Actions
       XDP_TRACE_UI_CMD_ON,
       XDP_TRACE_UI_CMD_OFF,
       XDP_TRACE_UI_CMD_START,
       XDP_TRACE_UI_CMD_STOP,
       XDP_TRACE_UI_CMD_RESUME,
       //save xdp trace configuration to file
       XDP_TRACE_UI_CMD_SAVE,
       XDP_TRACE_UI_CMD_SHOW_TRACE,


       // Format
	   XDP_TRACE_UI_CMD_FORMAT,
       XDP_TRACE_UI_CMD_FORMAT_SHOW,
       XDP_TRACE_UI_CMD_FORMAT_SEQ,
       XDP_TRACE_UI_CMD_FORMAT_NAME,
       XDP_TRACE_UI_CMD_FORMAT_ENTRY_NUMBER,
       XDP_TRACE_UI_CMD_FORMAT_COLOR,
       XDP_TRACE_UI_CMD_FORMAT_APP_TAG,
       XDP_TRACE_UI_CMD_FORMAT_TASK,
       XDP_TRACE_UI_CMD_FORMAT_TIME,
       XDP_TRACE_UI_CMD_FORMAT_FILE,
       XDP_TRACE_UI_CMD_FORMAT_STRING,
       XDP_TRACE_UI_CMD_FORMAT_PARAMS,
       XDP_TRACE_UI_CMD_FORMAT_BYTES,

       // Setup
       XDP_TRACE_UI_CMD_BUFFER,
       XDP_TRACE_UI_CMD_CYCLIC,
       XDP_TRACE_UI_CMD_RESOLUTION,

       // Misc
       XDP_TRACE_UI_CMD_WRITE,

       // Print
       XDP_TRACE_UI_CMD_PRINT_COLORS,
       XDP_TRACE_UI_CMD_PRINT_ABOVE_COLOR,
       XDP_TRACE_UI_CMD_PRINT_BELOW_COLOR,
       XDP_TRACE_UI_CMD_PRINT_OUTPUT,

       // Status
       XDP_TRACE_UI_CMD_STATUS,
       XDP_TRACE_UI_CMD_INTERNAL_STATUS,
       XDP_TRACE_UI_CMD_COUNTERS,

       //TESTING

       XDP_TRACE_UI_CMD_TEST,

};


// --- TheirSubCommands -------------------------------------------------------

// Shortcuts of buffer state as hot one for the table below

const UINT32 TRC_OFF      = 1 << XdpTrace::XDP_TRACE_STATE_OFF;
const UINT32 TRC_ON       = 1 << XdpTrace::XDP_TRACE_STATE_ON;
const UINT32 TRC_RECOVER  = 1 << XdpTrace::XDP_TRACE_STATE_RECOVER;

// Sub command list 

const XdpParse::XdpParseSubCommand  XdpTraceUI::TheirSubCommands[] = {

    // Display
  { "display",  "Display n entries from cursor",          
      XDP_TRACE_UI_CMD_DISPLAY,            TRC_ON | TRC_RECOVER },

  { "entries",  "Set/show default number of entries to display",  
      XDP_TRACE_UI_CMD_ENTRIES,           0 },

  { "output",   "Set/show output channel",  
      XDP_TRACE_UI_CMD_OUTPUT,            0 },

  { "relief",   "Set/show output relief value",  
      XDP_TRACE_UI_CMD_RELIEF,           0 },

    // Cursor
  { "cursor",   "Set/show cursor position",               
      XDP_TRACE_UI_CMD_CURSOR,             TRC_ON | TRC_RECOVER },

  { "seek",     "Seek/show cursor position Relatively",
      XDP_TRACE_UI_CMD_SEEK,               TRC_ON | TRC_RECOVER },

  { "begining", "Set cursor position to buffer begining (entry 0)",
      XDP_TRACE_UI_CMD_BEGINING,           TRC_ON | TRC_RECOVER },

  { "end",      "Set cursor position to buffer end",
      XDP_TRACE_UI_CMD_END,                TRC_ON | TRC_RECOVER },

    // Colors
  { "colors",    "Enable/disable write to buffer of entries colored with specific colors (for ALL app tags)",
      XDP_TRACE_UI_CMD_COLORS,             TRC_OFF | TRC_ON },

  { "aboveColor","Enable/disable write to buffer of entries colored 'color' or above (for ALL app tags)",
      XDP_TRACE_UI_CMD_ABOVE_COLOR,        TRC_OFF | TRC_ON },

  { "belowColor","Enable/disable write to buffer of entries colored 'color' or below (for ALL app tags)",
      XDP_TRACE_UI_CMD_BELOW_COLOR,        TRC_OFF | TRC_ON },
  
    // Application filter

  { "appTags",    "Enable/disable write to buffer of entries with specific application Tag",
      XDP_TRACE_UI_CMD_APPTAGS,             TRC_OFF | TRC_ON },

  { "printAppTags","Enable/disable direct print of entries with specific application Tag",
      XDP_TRACE_UI_CMD_PRINT_APPTAGS,        TRC_OFF | TRC_ON },

  { "appColors","Enable/disable write to buffer of entries with specific application Tag and color",
      XDP_TRACE_UI_CMD_APP_COLORS,        TRC_OFF | TRC_ON },

  { "appPrintColors","Enable/disable direct print of entries with specific application Tag and color",
      XDP_TRACE_UI_CMD_APP_PRINT_COLORS,        TRC_OFF | TRC_ON },

    // Actions
  { "on",        "Switch buffer to ON state which enables tracing",
      XDP_TRACE_UI_CMD_ON,                 TRC_OFF },

  { "off",       "Switch buffer to OFF state which enables buffer configuration",
      XDP_TRACE_UI_CMD_OFF,                TRC_ON },

  { "start",     "Start tracing (buffer is cleared)",
      XDP_TRACE_UI_CMD_START,              TRC_ON },

  { "stop",      "Stop tracing",
      XDP_TRACE_UI_CMD_STOP,               TRC_ON },

  { "resume",    "Resume tracing (beffer is appended)",
      XDP_TRACE_UI_CMD_RESUME,             TRC_ON },
// add save command
   { "save",    "Save trace configuration",
      XDP_TRACE_UI_CMD_SAVE,             TRC_ON },
   { "ShowTrace",    "Print the trace configuratiom",
      XDP_TRACE_UI_CMD_SHOW_TRACE,             TRC_ON },



    // Format
  { "format",    "Set/Show trace format",
      XDP_TRACE_UI_CMD_FORMAT,                  0 },


    // Setup
  { "Buffer",         "Allocate/Free/Show buffer memory",
      XDP_TRACE_UI_CMD_BUFFER,             TRC_OFF },

  { "Cyclic",         "Set/Show buffer cyclic mode",
      XDP_TRACE_UI_CMD_CYCLIC,             TRC_OFF },

  { "Resolution",     "Set/Show time stamp resolution",
      XDP_TRACE_UI_CMD_RESOLUTION,         TRC_OFF },


    // Misc
  { "write",           "Write entry to buffer",
      XDP_TRACE_UI_CMD_WRITE,               TRC_OFF | TRC_ON },

    // Print
  { "printColors",    "Enable/disable direct prints of entries colored with specific colors",
      XDP_TRACE_UI_CMD_PRINT_COLORS,        TRC_OFF | TRC_ON },

  { "printAboveColor","Enable/disable direct prints of entries colored 'color' or above",
      XDP_TRACE_UI_CMD_PRINT_ABOVE_COLOR,   TRC_OFF | TRC_ON },

  { "printBelowColor","Enable/disable direct prints of entries colored 'color' or below",
      XDP_TRACE_UI_CMD_PRINT_BELOW_COLOR,   TRC_OFF | TRC_ON },

  { "printOutput",   "Set/show print output channel",  
      XDP_TRACE_UI_CMD_PRINT_OUTPUT,        TRC_OFF | TRC_ON },

    // Status
  { "status",        "Show trace status",
      XDP_TRACE_UI_CMD_STATUS,              0 },

  { "internalStatus","Show trace internal status",
      XDP_TRACE_UI_CMD_INTERNAL_STATUS,     0 },

  { "counters",        "Show trace status",
      XDP_TRACE_UI_CMD_COUNTERS,            0 },

    //TESTING

  { "test",        "prints traces with differant colors and application tags",
      XDP_TRACE_UI_CMD_TEST,            0 },


    // End of list
  { NULL, "", 0,0} // Don't delete
};


// --- TheirColorEnumPairs ----------------------------------------------------
// Color string to color enum pair conversion table, used in xxColor commands parse

const XdpParseEnum::XdpParseEnumPair XdpTraceUI::TheirColorEnumPairs[] = {
    {"White",  XDP_TRACE_WHITE  },
    {"Green",  XDP_TRACE_GREEN  },
    {"Yellow", XDP_TRACE_YELLOW },
    {"Orange", XDP_TRACE_ORANGE },
    {"Red",    XDP_TRACE_RED    },
    {"Black",  XDP_TRACE_BLACK  },
    {NULL, 0 }  };

// --- TheirAppTagEnumPairs ----------------------------------------------------
// app tag string to app tag enum pair conversion table, used in xxappTag commands parse

const XdpParseEnum::XdpParseEnumPair XdpTraceUI::TheirAppTagEnumPairs[] = {
    {"AppD",   XDP_TRACE_APP_DEFAULT   },
    {"App1",   XDP_TRACE_APP1   },
    {"App2",   XDP_TRACE_APP2   },
    {"App3",   XDP_TRACE_APP3   },
    {"App4",   XDP_TRACE_APP4   },
    {"App5",   XDP_TRACE_APP5   },
    {"App6",   XDP_TRACE_APP6   },
    {"App7",   XDP_TRACE_APP7   },
    {"App8",   XDP_TRACE_APP8   },
    {"App9",   XDP_TRACE_APP9   },
    {"App10",  XDP_TRACE_APP10  },
    {"App11",  XDP_TRACE_APP11  },
    {"App12",  XDP_TRACE_APP12  },
    {"App13",  XDP_TRACE_APP13  },
    {"App14",  XDP_TRACE_APP14  },
    {NULL, 0 }  };

// --- TheirOutputEnumPairs ---------------------------------------------------
const XdpParseEnum::XdpParseEnumPair XdpTraceUI::TheirOutputEnumPairs[] = {
        {"CmdMngr", (UINT32) XdpTrace::XDP_TRACE_OUTPUT_CMDMNGR},
        {"UDP",     (UINT32) XdpTrace::XDP_TRACE_OUTPUT_UDP},
        {"Serial",  (UINT32) XdpTrace::XDP_TRACE_OUTPUT_SERIAL},
#ifdef RN_WIN32
        {"File",    (UINT32) XdpTrace::XDP_TRACE_OUTPUT_FILE},
#endif
        {NULL, 0} };

// --- TheirPrintOutputEnumPairs ----------------------------------------------
const XdpParseEnum::XdpParseEnumPair XdpTraceUI::TheirPrintOutputEnumPairs[] = {
        {"UDP",     (UINT32) XdpTrace::XDP_TRACE_OUTPUT_UDP},
        {"Serial",  (UINT32) XdpTrace::XDP_TRACE_OUTPUT_SERIAL},
#ifdef RN_WIN32
        {"File",    (UINT32) XdpTrace::XDP_TRACE_OUTPUT_FILE},
#endif
        {NULL, 0} };



// --- ctor ---------------------------------------------------------------
//
// Init all members
//

XdpTraceUI::XdpTraceUI() {

    ItsTraceP   = NULL;
    ItsCmdMngrP = NULL;
    ItsParserP  = NULL;

    RN_TRACE(MASK_XDP, "At ctor, this=" + XdpConv::ToHexStr(this) );

}


// --- ProcessCommand -----------------------------------------------------
//
// Process user command on theTrace trace object using theCmdMngr for user IO
//
void XdpTraceUI::ProcessCommand(XdpTrace *theTraceP, CmdMngr *theCmdMngrP) {

    // First of all verify the given objects

    RN_ASSERT(theTraceP, "theTraceP is NULL");
    RN_ASSERT(theCmdMngrP, "theCmdMngrP is NULL");

    // Save them in the object for use by other functions

    ItsTraceP   = theTraceP;
    ItsCmdMngrP = theCmdMngrP;

    // Create command parser & save its pointer
    XdpParse parser(ItsCmdMngrP);

    ItsParserP = &parser;

    // Get command and execute it

    GetAndExecuteCommand();

    // Set to NULL for long relavant pointers

    ItsTraceP   = NULL;
    ItsCmdMngrP = NULL;
    ItsParserP  = NULL;

}


// --- GetAndExecuteCommand ---------------------------------------------------
//
// Get and execute command has a few steps
//   1. Get buffer state
//   2. Get the sub command 
//   3. Clear trace status string
//   4. Execute it
//   5. Print trace status messages
//
bool XdpTraceUI::GetAndExecuteCommand() {
 
    bool rc;

    // 1. Get buffer state and convert it into active command bit map (one hot)

    XdpTrace::XdpTraceState bufState = ItsTraceP->GetState();

    UINT32 activeCmds = 1 << (UINT32) bufState;

    // 2. Get the sub command

    UINT32 cmdNumber=0xffffffff; // init to be on the safe side

    rc = ItsParserP->ParseSubCommands("trace","Manage XdpTrace",0, 
                                            TheirSubCommands, activeCmds , cmdNumber);

    // Check error
    if (rc == false) {
        return false;
    }

    // 3. Clear status string

    ItsTraceP->ClearStatusStrings();

    // 4. Execute the selected sub command
    bool cmdRc;

    switch (cmdNumber) {

        // Display
        case XDP_TRACE_UI_CMD_DISPLAY:  
            cmdRc = CommandDisplay(); 
            break;

        case XDP_TRACE_UI_CMD_ENTRIES:  
            cmdRc = CommandEnteries(); 
            break;

        case XDP_TRACE_UI_CMD_OUTPUT:  
            cmdRc = CommandOutput(); 
            break;

        case XDP_TRACE_UI_CMD_RELIEF:  
            cmdRc = CommandRelief(); 
            break;
        
        // Cursor
        case XDP_TRACE_UI_CMD_CURSOR:  
            cmdRc = CommandCursor(); 
            break;

        case XDP_TRACE_UI_CMD_SEEK:  
            cmdRc = CommandSeek(); 
            break;

        case XDP_TRACE_UI_CMD_BEGINING:  
            cmdRc = CommandBegining(); 
            break;

        case XDP_TRACE_UI_CMD_END:  
            cmdRc = CommandEnd(); 
            break;

        // Colors
        case XDP_TRACE_UI_CMD_COLORS:  
            cmdRc = CommandColors(); 
            break;

        case XDP_TRACE_UI_CMD_ABOVE_COLOR:  
            cmdRc = CommandAboveColor(); 
            break;

        case XDP_TRACE_UI_CMD_BELOW_COLOR:  
            cmdRc = CommandBelowColor(); 
            break;

        // Application filter

        case XDP_TRACE_UI_CMD_APPTAGS:
            cmdRc = CommandAppTags();
            break;

        case XDP_TRACE_UI_CMD_PRINT_APPTAGS:
            cmdRc = CommandPrintAppTags();
            break;

        case XDP_TRACE_UI_CMD_APP_COLORS:
            cmdRc = CommandAppColors();
            break;

        case XDP_TRACE_UI_CMD_APP_PRINT_COLORS:
            cmdRc = CommandAppPrintColors();
            break;

        // Actions
        case XDP_TRACE_UI_CMD_OFF:  
            cmdRc = CommandOff(); 
            break;

        case XDP_TRACE_UI_CMD_ON:  
            cmdRc = CommandOn(); 
            break;

        case XDP_TRACE_UI_CMD_START:  
            cmdRc = CommandStart(); 
            break;

        case XDP_TRACE_UI_CMD_STOP:  
            cmdRc = CommandStop(); 
            break;

        case
            XDP_TRACE_UI_CMD_RESUME:
            cmdRc = CommandResume(); 
            break;
//add save command
        case XDP_TRACE_UI_CMD_SAVE:
            cmdRc = CommandSave();
            break;

//print to screen XdpTrace params
            case XDP_TRACE_UI_CMD_SHOW_TRACE:
            cmdRc = CommandShowTrace();
            break;



        // Format
        case XDP_TRACE_UI_CMD_FORMAT:  
            cmdRc = CommandFormat(); 
            break;


        // Setup
        case XDP_TRACE_UI_CMD_BUFFER:  
            cmdRc = CommandBuffer(); 
            break;

        case XDP_TRACE_UI_CMD_CYCLIC:
            cmdRc = CommandCyclic(); 
            break;

        case XDP_TRACE_UI_CMD_RESOLUTION:
            cmdRc = CommandResolution(); 
            break;

        // Misc
        case XDP_TRACE_UI_CMD_WRITE:  
            cmdRc = CommandWrite(); 
            break;

        // Print
        case XDP_TRACE_UI_CMD_PRINT_COLORS:  
            cmdRc = CommandPrintColors(); 
            break;

        case XDP_TRACE_UI_CMD_PRINT_ABOVE_COLOR:  
            cmdRc = CommandPrintAboveColor(); 
            break;

        case XDP_TRACE_UI_CMD_PRINT_BELOW_COLOR:  
            cmdRc = CommandPrintBelowColor(); 
            break;

        case XDP_TRACE_UI_CMD_PRINT_OUTPUT:  
            cmdRc = CommandPrintOutput(); 
            break;

        // Status
        case XDP_TRACE_UI_CMD_STATUS:  
            cmdRc = CommandStatus(); 
            break;


        case XDP_TRACE_UI_CMD_INTERNAL_STATUS:  
            cmdRc = CommandInternalStatus(); 
            break;

        case XDP_TRACE_UI_CMD_COUNTERS:  
            cmdRc = CommandCounters(); 
            break;

        case XDP_TRACE_UI_CMD_TEST:
            cmdRc = CommandTest();
        break;

        default: // Unknowen commnad number
            PrintString("Error: Invalid command number " + XdpConv::ToStr(cmdNumber) + "\n");
            return false; 
    }


    // At this point the command has been executed and cmdRc has the the command status
    // but I don't know what to do with it

    // 5. get and print status message
    
    RWCString statusStr = ItsTraceP->GetStatusStrings();
    PrintString(statusStr);

    // 6. return the command execuation status

    return cmdRc;

}

// ============================================================================
// ===
// === Commands implementation
// ===
// ============================================================================

// --- CommandDisplay ---------------------------------------------------------
//
bool XdpTraceUI::CommandDisplay() {        

    bool rc;

    // Get the default Count 
    INT32 n = ItsTraceP->GetDefaultDisplayCount();

    // Get output channel
    UINT32 out = ItsTraceP->GetOutputChannel();

    // Parse parameters
    XdpParseNumber parse_n(n,   "n", "Number of entries to display from cursor forward(+) or backword(-)",
                          XDP_PARSE_PARAMETER_OPTIONAL);

    XdpParseEnum   parseOut(out, "OutputChannel", "Output channel", 
                          XDP_PARSE_PARAMETER_OPTIONAL, TheirOutputEnumPairs);

    rc = ItsParserP->ParseParameters( "" , "" , 0 , &parse_n, &parseOut );


    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Convert output to the right type
    XdpTrace::XdpTraceOutput outAsEnum = (XdpTrace::XdpTraceOutput) out;
    

    // Execute the command

    rc = ItsTraceP->Display(n, outAsEnum );


    return rc;

}

// --- CommandEntries --------------------------------------------------------
//
bool XdpTraceUI::CommandEnteries() {        

    bool rc;

    // Get the default Count 
    INT32 n = ItsTraceP->GetDefaultDisplayCount();

    XdpParseNumber parse_n(n,   "n", "Default number of entries to display from cursor forward(+) or backword(-)", 
                          XDP_PARSE_PARAMETER_OPTIONAL);

    rc = ItsParserP->ParseParameters("","",0, &parse_n);

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // If paramter set, it is a set command
    if ( parse_n.IsAssigned() == true) {

        // Set parameter given, set the value

        rc = ItsTraceP->SetDefaultDisplayCount(n);

        // Check error
        if (rc == false) {
            return false;
        }

    }
    else {
        // Simulate command executed OK
        rc = true;
    }

    // Get the default Count & show it
    n = ItsTraceP->GetDefaultDisplayCount();
    RWCString s = "Default number of entries to display is : " + XdpConv::ToStr(n) +"\n";
    PrintString(s);

    // done

    return rc;

}

// --- CommandOutput ----------------------------------------------------------
//
bool XdpTraceUI::CommandOutput() {        

    bool rc;

    // Get output channel
    UINT32 out = ItsTraceP->GetOutputChannel();

    // Output parser
    XdpParseEnum out_p (out, "OutputChannel", "Output channel", 
                          XDP_PARSE_PARAMETER_OPTIONAL, TheirOutputEnumPairs);
 
    rc = ItsParserP->ParseParameters("","",0,
        &out_p
        );

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Convert output to the right type
    XdpTrace::XdpTraceOutput outAsEnum = (XdpTrace::XdpTraceOutput) out;
    
    // Check if set
    if (out_p.IsAssigned() == true) {
        // Set command, set it
        rc = ItsTraceP->SetOutputChannel(outAsEnum);
        if (rc == false) {
            return false;
        }
    }

    // Get the output current value

    outAsEnum = ItsTraceP->GetOutputChannel();

    // Print it
    RWCString s;

    s = "The output channel is " + ItsTraceP->ToStr(outAsEnum) + "\n";
    PrintString(s);

    // All done
    return true;

}

// --- CommandRelief ----------------------------------------------------------
//
bool XdpTraceUI::CommandRelief() {        

    bool rc;

    // Get the default Count 
    INT32 n = ItsTraceP->GetOutputReliefNumber();

    XdpParseNumber parse_n(n,   "n", "Number of entries to display between reliefs (10mSec sleeps)", 
                          XDP_PARSE_PARAMETER_OPTIONAL);

    rc = ItsParserP->ParseParameters("","",0, &parse_n);

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // If paramter set, it is a set command
    if ( parse_n.IsAssigned() == true) {

        // Set parameter given, set the value

        rc = ItsTraceP->SetOutputReliefNumber(n);

        // Check error
        if (rc == false) {
            return false;
        }

    }
    else {
        // Simulate command executed OK
        rc = true;
    }

    // Get the default Count & show it
    n = ItsTraceP->GetOutputReliefNumber();
    RWCString s = "The number of entires displayed between reliefs is: " + XdpConv::ToStr(n) +"\n";
    PrintString(s);

    // done

    return rc;

}

// --- CommandCursor ---------------------------------------------------------
//
bool XdpTraceUI::CommandCursor() {        


    bool rc;

    // Entry number
    UINT32 n;

    XdpParseNumber parse_n(n,"n", "Absolute cursor position", XDP_PARSE_PARAMETER_OPTIONAL);

    rc = ItsParserP->ParseParameters("","",0, &parse_n );

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // If paramter set, it is a set command
    if ( parse_n.IsAssigned() == true) {

        // Set parameter given, set the cursor

        bool dummyEob;
        rc = ItsTraceP->AbsoluteSeekCursor(n,&dummyEob);

        // Check error
        if (rc == false) {
            return false;
        }

    }
    else {
        // Simulate command executed OK
        rc = true;
    }

    // In anycase, show cursor entry

    rc = ShowCursorEntry();

    return rc;

}

// --- CommandSeek -----------------------------------------------------------
//
bool XdpTraceUI::CommandSeek() {        


    bool rc;

    // delta
    INT32 delta;

    XdpParseNumber parseDelta(delta,"delta", 
                     "entries to seek relative to the current position. Forward(+) or backward(-)", 
                     XDP_PARSE_PARAMETER_OPTIONAL);

    rc = ItsParserP->ParseParameters("","",0, &parseDelta );

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // If paramter set, it is a set command
    if ( parseDelta.IsAssigned() == true) {

        // Set parameter given, set the cursor

        bool dummyEob;
        rc = ItsTraceP->SeekCursor(delta,&dummyEob);

        // Check error
        if (rc == false) {
            return false;
        }

    }
    else {
        // Simulate command executed OK
        rc = true;
    }

    // In anycase, show cursor entry

    rc = ShowCursorEntry();

    return rc;

}

// --- CommandBegining --------------------------------------------------------
//
bool XdpTraceUI::CommandBegining() {        


    bool rc;

    // Seek cursor to the entry 0

 
    bool dummyEob;
    rc = ItsTraceP->AbsoluteSeekCursor(0,&dummyEob);

    // Check error
    if (rc == false) {
        return false;
    }

    // In anycase, show cursor entry

    rc = ShowCursorEntry();

    return rc;

}

// --- CommandEnd -------------------------------------------------------------
//
bool XdpTraceUI::CommandEnd() {        


    bool rc;

    // Seek cursor to the end

    rc = ItsTraceP->SeekCursorToEnd();

    // Check error
    if (rc == false) {
        return false;
    }

    // In anycase, show cursor entry

    rc = ShowCursorEntry();

    return rc;

}


// --- CommandColors ----------------------------------------------------------
//
bool XdpTraceUI::CommandColors() {        


    bool rc;
    UINT32 i;
    const UINT32 colorNum = 5;

    // enable/disable Flag
    bool enableFlag = false;
    
    UINT32 appTag = XDP_TRACE_APP_ALL; // ALL application

    // the colors
    UINT32 colors[colorNum];

    // Init the colors

    for( i = 0 ; i < colorNum ; i++) {
 
        colors[i] = XDP_TRACE_COLOR_NONE;
    }

    // parse paramerters objects
    XdpParseBool   parseEnableFlag(enableFlag, "en/dis", 
                   "Enable or disable the listed color. When omited show the active colors",
                   XDP_PARSE_PARAMETER_OPTIONAL);

    XdpParseEnum   parseColor0(colors[0], "Color", "A color - omit for all colors", 
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);

    XdpParseEnum   parseColor1(colors[1], "Color", "A color", 
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);

    XdpParseEnum   parseColor2(colors[2], "Color", "A color", 
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);

    XdpParseEnum   parseColor3(colors[3], "Color", "A color", 
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);
    
    XdpParseEnum   parseColor4(colors[4], "Color", "A color", 
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);


    // make array of parse colors & fill it
    XdpParseEnum   *parseColors[colorNum];

    parseColors[0] = &parseColor0;
    parseColors[1] = &parseColor1;
    parseColors[2] = &parseColor2;
    parseColors[3] = &parseColor3;
    parseColors[4] = &parseColor4;
  
    // Parse parameters
    rc = ItsParserP->ParseParameters("","",0, &parseEnableFlag,
                     &parseColor0, &parseColor1, &parseColor2, &parseColor3, &parseColor4);

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // In color mask, each color has a bit
    UINT32 colorMask;

    // If paramter enable/disable parameter process the colors for set

    if ( parseEnableFlag.IsAssigned() == true ) {
        
        // We set the colors, build color mask
        colorMask = 0; // Empty mask

        // If all colors are omitted, then set all colors
        if (parseColors[0]->IsAssigned() == false) {

            // First color is omitted, set all colors

            for( i = XDP_TRACE_COLOR_FIRST ; i <= XDP_TRACE_COLOR_LAST ; i++) {

                // Add the color to the color mask

                colorMask |= 1 << i;
            } // for

        }
        else {

            // At least one color is set, use the color list
            // Convert color parameters into a color mask
                
            for( i = 0 ; i < colorNum ; i++) {

                // Add the color to the color mask, unassigned color have the default none
                colorMask |= 1 << (UINT32) colors[i];
            } // for

        } // colors set
    
        // Now we have the color mask

        // Update the colors
        rc = ItsTraceP->UpdateEnabledColors(enableFlag, colorMask, appTag);

        // Check error
        if (rc == false) {
            return false;
        }

    } // Enable/disable flag assigned
    else {
        
        // Just show, get and print the enabled color status

        UINT32 *appTagsColorMask;

        appTagsColorMask = ItsTraceP->GetEnableColors();
        ShowColorsStatus(appTagsColorMask);

        // fake command is OK

        rc = true;

    } // Enable/disable flag assigned

    // Done
    return rc;

}

// --- CommandAppColors ----------------------------------------------------------
//
bool XdpTraceUI::CommandAppColors() {


    bool rc;
    UINT32 i;
    const UINT32 colorNum = 5;

    // enable/disable Flag
    bool enableFlag = false;

    UINT32 appTag = XDP_TRACE_APP_DEFAULT; // Default application

    // the colors
    UINT32 colors[colorNum];

    // Init the colors

    for( i = 0 ; i < colorNum ; i++) {

        colors[i] = XDP_TRACE_COLOR_NONE;
    }

    // parse paramerters objects
    XdpParseBool   parseEnableFlag(enableFlag, "en/dis",
                   "Enable or disable the listed color.",
                   0);

    XdpParseEnum   parseAppTag(appTag, "AppTag", "An application tag",
                   0, TheirAppTagEnumPairs);

    XdpParseEnum   parseColor0(colors[0], "Color", "A color - omit for all colors",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);

    XdpParseEnum   parseColor1(colors[1], "Color", "A color",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);

    XdpParseEnum   parseColor2(colors[2], "Color", "A color",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);

    XdpParseEnum   parseColor3(colors[3], "Color", "A color",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);

    XdpParseEnum   parseColor4(colors[4], "Color", "A color",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);


    // make array of parse colors & fill it
    XdpParseEnum   *parseColors[colorNum];

    parseColors[0] = &parseColor0;
    parseColors[1] = &parseColor1;
    parseColors[2] = &parseColor2;
    parseColors[3] = &parseColor3;
    parseColors[4] = &parseColor4;

    // Parse parameters
    rc = ItsParserP->ParseParameters("","",0, &parseEnableFlag, &parseAppTag,
                     &parseColor0, &parseColor1, &parseColor2, &parseColor3, &parseColor4);

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // In color mask, each color has a bit
    UINT32 colorMask;

    // We set the colors, build color mask
    colorMask = 0; // Empty mask

        // If all colors are omitted, then set all colors
    if (parseColors[0]->IsAssigned() == false) {

        // First color is omitted, set all colors

        for( i = XDP_TRACE_COLOR_FIRST ; i <= XDP_TRACE_COLOR_LAST ; i++) {

            // Add the color to the color mask

            colorMask |= 1 << i;
        } // for

    }
    else {

        // At least one color is set, use the color list
        // Convert color parameters into a color mask

        for( i = 0 ; i < colorNum ; i++) {

            // Add the color to the color mask, unassigned color have the default none
            colorMask |= 1 << (UINT32) colors[i];
        } // for

    } // colors set

        // Now we have the color mask

        // Update the colors
        rc = ItsTraceP->UpdateEnabledColors(enableFlag, colorMask, appTag);

        // Check error
        if (rc == false) {
            return false;
        }

    // Done
    return rc;

}

// --- CommandAppPrintColors ----------------------------------------------------------
//
bool XdpTraceUI::CommandAppPrintColors() {


    bool rc;
    UINT32 i;
    const UINT32 colorNum = 5;

    // enable/disable Flag
    bool enableFlag = false;

    UINT32 appTag = XDP_TRACE_APP_DEFAULT; // Default application

    // the colors
    UINT32 colors[colorNum];

    // Init the colors

    for( i = 0 ; i < colorNum ; i++) {

        colors[i] = XDP_TRACE_COLOR_NONE;
    }

    // parse paramerters objects
    XdpParseBool   parseEnableFlag(enableFlag, "en/dis",
                   "Enable or disable the listed color.",
                   0);

    XdpParseEnum   parseAppTag(appTag, "AppTag", "An application tag",
                   0, TheirAppTagEnumPairs);

    XdpParseEnum   parseColor0(colors[0], "Color", "A color - omit for all colors",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);

    XdpParseEnum   parseColor1(colors[1], "Color", "A color",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);

    XdpParseEnum   parseColor2(colors[2], "Color", "A color",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);

    XdpParseEnum   parseColor3(colors[3], "Color", "A color",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);

    XdpParseEnum   parseColor4(colors[4], "Color", "A color",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);


    // make array of parse colors & fill it
    XdpParseEnum   *parseColors[colorNum];

    parseColors[0] = &parseColor0;
    parseColors[1] = &parseColor1;
    parseColors[2] = &parseColor2;
    parseColors[3] = &parseColor3;
    parseColors[4] = &parseColor4;

    // Parse parameters
    rc = ItsParserP->ParseParameters("","",0, &parseEnableFlag, &parseAppTag,
                     &parseColor0, &parseColor1, &parseColor2, &parseColor3, &parseColor4);

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // In color mask, each color has a bit
    UINT32 colorMask;

    // We set the colors, build color mask
    colorMask = 0; // Empty mask

        // If all colors are omitted, then set all colors
    if (parseColors[0]->IsAssigned() == false) {

        // First color is omitted, set all colors

        for( i = XDP_TRACE_COLOR_FIRST ; i <= XDP_TRACE_COLOR_LAST ; i++) {

            // Add the color to the color mask

            colorMask |= 1 << i;
        } // for

    }
    else {

        // At least one color is set, use the color list
        // Convert color parameters into a color mask

        for( i = 0 ; i < colorNum ; i++) {

            // Add the color to the color mask, unassigned color have the default none
            colorMask |= 1 << (UINT32) colors[i];
        } // for

    } // colors set

        // Now we have the color mask

        // Update the colors
        rc = ItsTraceP->UpdatePrintColors(enableFlag, colorMask, appTag);

        // Check error
        if (rc == false) {
            return false;
        }

    // Done
    return rc;

}


// --- CommandAboveColor ------------------------------------------------------
//

bool XdpTraceUI::CommandAboveColor() {        


    bool rc;
    UINT32 i;

    // enable/disable Flag
    bool enableFlag;
    
    UINT32 appTag = XDP_TRACE_APP_ALL; // ALL application

    // the color;
    UINT32 color;

    // Parse parameters
    XdpParseBool parseEnableFlag(enableFlag, "en/dis", "Enable or disable colors" , 0);

    XdpParseEnum parseColor(color,      "color" , "Act from this color to the last color", 
                            0, TheirColorEnumPairs);

    rc = ItsParserP->ParseParameters( "" , "" , 0 , &parseEnableFlag, &parseColor);

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // In color mask, each color has a bit, init to an empty mask
    UINT32 colorMask = 0;

    // Loop all color for the user color to the last one
    for( i = color ; i <= XDP_TRACE_COLOR_LAST ; i++) {

        // Add the color to the color mask

        colorMask |= 1 << i;
    } // for
        
    // Now we have the color mask

    // Update the colors
    rc = ItsTraceP->UpdateEnabledColors(enableFlag, colorMask, appTag);

    // Check error
    if (rc == false) {
        return false;
    }

    // Done
    return rc;

}

 
// --- CommandBelowColor ------------------------------------------------------
//

bool XdpTraceUI::CommandBelowColor() {        


    bool rc;
    UINT32 i;

    // enable/disable Flag
    bool enableFlag;
    
    UINT32 appTag = XDP_TRACE_APP_ALL; // ALL application

    // the color;
    UINT32 color;

    // Parse parameters

    XdpParseBool parseEnableFlag(enableFlag, "en/dis", "Enable or disable colors" , 0);

    XdpParseEnum parseColor     (color,      "color" , "Act from first color to the this color", 
                                 0, TheirColorEnumPairs) ;

    rc = ItsParserP->ParseParameters( "" , "" , 0 , &parseEnableFlag , &parseColor);

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // In color mask, each color has a bit, init to an empty mask
    UINT32 colorMask = 0;

    // Loop all color for the first color to the user color
    for( i = XDP_TRACE_COLOR_FIRST ; i <= color ; i++) {

        // Add the color to the color mask

        colorMask |= 1 << i;
    } // for
        
    // Now we have the color mask

    // Update the colors
    rc = ItsTraceP->UpdateEnabledColors(enableFlag, colorMask, appTag);

    // Check error
    if (rc == false) {
        return false;
    }

    // Done
    return rc;

}

// --- CommandAppTags ------------------------------------------------------
//

bool XdpTraceUI::CommandAppTags() {

    bool rc;
    UINT32 i;
    const UINT32 appTagNum = 15;

    // enable/disable Flag
    bool enableFlag = false;

    // the application tags
    UINT32 appTags[appTagNum];

    // Init the tags

    for( i = 0 ; i < appTagNum ; i++) {

        appTags[i] = XDP_TRACE_APP_NONE; // no app tag.
    }

    // parse paramerters objects
    XdpParseBool   parseEnableFlag(enableFlag, "en/dis",
                   "Enable or disable the listed tags. When omited show the active tags",
                   XDP_PARSE_PARAMETER_OPTIONAL);

    XdpParseEnum   parseAppTag0(appTags[0], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag1(appTags[1], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag2(appTags[2], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag3(appTags[3], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag4(appTags[4], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag5(appTags[5], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag6(appTags[6], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag7(appTags[7], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag8(appTags[8], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag9(appTags[9], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag10(appTags[10], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag11(appTags[11], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag12(appTags[12], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag13(appTags[13], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag14(appTags[14], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    // make array of parse app tags & fill it
    XdpParseEnum   *parseAppTags[appTagNum];

    parseAppTags[0] = &parseAppTag0;
    parseAppTags[1] = &parseAppTag1;
    parseAppTags[2] = &parseAppTag2;
    parseAppTags[3] = &parseAppTag3;
    parseAppTags[4] = &parseAppTag4;
    parseAppTags[5] = &parseAppTag5;
    parseAppTags[6] = &parseAppTag6;
    parseAppTags[7] = &parseAppTag7;
    parseAppTags[8] = &parseAppTag8;
    parseAppTags[9] = &parseAppTag9;
    parseAppTags[10] = &parseAppTag10;
    parseAppTags[11] = &parseAppTag11;
    parseAppTags[12] = &parseAppTag12;
    parseAppTags[13] = &parseAppTag13;
    parseAppTags[14] = &parseAppTag14;

    // Parse parameters
    rc = ItsParserP->ParseParameters("","",0, &parseEnableFlag,
                     &parseAppTag0, &parseAppTag1, &parseAppTag2, &parseAppTag3, &parseAppTag4,
                     &parseAppTag5, &parseAppTag6, &parseAppTag7, &parseAppTag8, &parseAppTag9,
                     &parseAppTag10, &parseAppTag11, &parseAppTag12, &parseAppTag13, &parseAppTag14);

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // In app tag mask, each color has a bit
    UINT32 appTagMask;

    // If paramter enable/disable parameter process the app tags for set

    if ( parseEnableFlag.IsAssigned() == true ) {

        // We set the app tags, build app tags mask
        appTagMask = 0; // Empty mask

        // If all tags are omitted, then set all tags
        if (parseAppTags[0]->IsAssigned() == false) {

            // First tag is omitted, set all tags

            for( i = XDP_TRACE_APP_FIRST ; i <= XDP_TRACE_APP_LAST ; i++) {

                // Add the tag to the app tags mask

                appTagMask |= 1 << i;
            } // for

        }

        else {

            // At least one tag is set, use the tag list
            // Convert tag parameters into a tag mask

            for( i = 0 ; i < appTagNum ; i++) {

                // Add the tag to the tag mask, unassigned tag have the none tag!!!
                appTagMask |= 1 << (UINT32) appTags[i];
            } // for

        } // app tags set


        // Now we have the app tags mask

        // Update the tags
        rc = ItsTraceP->UpdateEnabledAppTags(enableFlag, appTagMask);


        // Check error
        if (rc == false) {
          return false;
        }

    } // Enable/disable flag assigned
    else {

        // Just show, get and print the enabled color status

        appTagMask = ItsTraceP->GetEnableAppTags();
        ShowAppTagsStatus(appTagMask);

        // fake command is OK

        rc = true;

    } // Enable/disable flag assigned

    // Done
    return rc;

}

// --- CommandPrintAppTags ----------------------------------------------------
//
bool XdpTraceUI::CommandPrintAppTags() {


    bool rc;
    UINT32 i;
    const UINT32 appTagNum = 15;

    // enable/disable Flag
    bool enableFlag = false;

    // the app tags
    UINT32 appTags[appTagNum];

    // Init the app tags

    for( i = 0 ; i < appTagNum ; i++) {

        appTags[i] = XDP_TRACE_APP_NONE;// no app tag
    }

    // parse paramerters objects
    XdpParseBool   parseEnableFlag(enableFlag, "en/dis",
                   "Enable or disable the listed tags. When omited show the active tags",
                   XDP_PARSE_PARAMETER_OPTIONAL);

    XdpParseEnum   parseAppTag0(appTags[0], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag1(appTags[1], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag2(appTags[2], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag3(appTags[3], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag4(appTags[4], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag5(appTags[5], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag6(appTags[6], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag7(appTags[7], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag8(appTags[8], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag9(appTags[9], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag10(appTags[10], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag11(appTags[11], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag12(appTags[12], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag13(appTags[13], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    XdpParseEnum   parseAppTag14(appTags[14], "AppTag", "An application tag",
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirAppTagEnumPairs);

    // make array of parse app tags & fill it
    XdpParseEnum   *parseAppTags[appTagNum];

    parseAppTags[0] = &parseAppTag0;
    parseAppTags[1] = &parseAppTag1;
    parseAppTags[2] = &parseAppTag2;
    parseAppTags[3] = &parseAppTag3;
    parseAppTags[4] = &parseAppTag4;
    parseAppTags[5] = &parseAppTag5;
    parseAppTags[6] = &parseAppTag6;
    parseAppTags[7] = &parseAppTag7;
    parseAppTags[8] = &parseAppTag8;
    parseAppTags[9] = &parseAppTag9;
    parseAppTags[10] = &parseAppTag10;
    parseAppTags[11] = &parseAppTag11;
    parseAppTags[12] = &parseAppTag12;
    parseAppTags[13] = &parseAppTag13;
    parseAppTags[14] = &parseAppTag14;

    // Parse parameters
    rc = ItsParserP->ParseParameters("","",0, &parseEnableFlag,
                     &parseAppTag0, &parseAppTag1, &parseAppTag2, &parseAppTag3, &parseAppTag4,
                     &parseAppTag5, &parseAppTag6, &parseAppTag7, &parseAppTag8, &parseAppTag9,
                     &parseAppTag10, &parseAppTag11, &parseAppTag12, &parseAppTag13, &parseAppTag14);

    // Check parameters error
    if (rc == false) {
        return false;
    }

     // In app tag mask, each color has a bit
    UINT32 appTagMask;

    // If paramter enable/disable parameter process the app tags for set

    if ( parseEnableFlag.IsAssigned() == true ) {

        // We set the app tags, build app tags mask
        appTagMask = 0; // Empty mask

        // If all tags are omitted, then set all tags
        if (parseAppTags[0]->IsAssigned() == false) {

            // First tag is omitted, set all tags

            for( i = XDP_TRACE_APP_FIRST ; i <= XDP_TRACE_APP_LAST ; i++) {

                // Add the tag to the app tags mask

                appTagMask |= 1 << i;
            } // for

        }
        else {

            // At least one tag is set, use the tag list
            // Convert tag parameters into a tag mask

            for( i = 0 ; i < appTagNum ; i++) {

                // Add the tag to the tag mask, unassigned tag have the none tag!!!
                appTagMask |= 1 << (UINT32) appTags[i];
            } // for

        } // app tags set

        // Now we have the app tags mask

        // Update the tags
        rc = ItsTraceP->UpdatePrintAppTags(enableFlag, appTagMask);


        // Check error
        if (rc == false) {
            return false;
        }

    } // Enable/disable flag assigned
    else {

        // Just show, get and print the enabled color status

        appTagMask = ItsTraceP->GetPrintAppTags();
        ShowAppTagsStatus(appTagMask);

        // fake command is OK

        rc = true;

    } // Enable/disable flag assigned

    // Done

    // Done
    return rc;

}

// --- CommandOn --------------------------------------------------------------
//
bool XdpTraceUI::CommandOn() {        


    bool rc;

    rc = ItsTraceP->On();

    return rc;

}

// --- CommandOff --------------------------------------------------------------
//
bool XdpTraceUI::CommandOff() {        


    bool rc;

    rc = ItsTraceP->Off();

    return rc;

}

// --- CommandStart -----------------------------------------------------------
//
bool XdpTraceUI::CommandStart() {        


    bool rc;

    rc = ItsTraceP->Start();

    return rc;

}
// --- CommandStop ------------------------------------------------------------
//
bool XdpTraceUI::CommandStop() {        


    bool rc;

    rc = ItsTraceP->Stop();

    return rc;

}
// --- CommandResume ----------------------------------------------------------
//
bool XdpTraceUI::CommandResume() {        


    bool rc;

    rc = ItsTraceP->Resume();

    return rc;

}



// --- CommandSave ----------------------------------------------------------
//
bool XdpTraceUI::CommandSave() {

    bool rc;

    rc = ItsTraceP->SaveConfig();

    return rc;


}




bool XdpTraceUI::CommandShowTrace(){
    ItsTraceP->ShowTraceConfig();
    return true;
}


// --- CommandFormat ----------------------------------------------------------
//
// Get and execute the sub-sub commands for the format sub-command by calling 
// the proper function.
//
// Get and execute command has a few steps
//   1. Get buffer state
//   2. Get the format sub-sub command 
//   3. Execute it
//   4. return the command status
//
bool XdpTraceUI::CommandFormat() {

	// Sub command list 

static const XdpParse::XdpParseSubCommand  formatSubCommands[] = {


    // Format
  { "show",     "Show all formats",
      XDP_TRACE_UI_CMD_FORMAT_SHOW,         0 },

  { "seq",      "Set output sequence number",
      XDP_TRACE_UI_CMD_FORMAT_SEQ,          0 },

  { "name",     "Set trace name format",
      XDP_TRACE_UI_CMD_FORMAT_NAME,         0 },

  { "entryNum", "Set entry number format",
      XDP_TRACE_UI_CMD_FORMAT_ENTRY_NUMBER, 0 },

  { "color",    "Set color format",    
      XDP_TRACE_UI_CMD_FORMAT_COLOR,        0 },

   { "appTag",    "Set application tag format",
      XDP_TRACE_UI_CMD_FORMAT_APP_TAG,        0 },

  { "task",     "Set task format", 
      XDP_TRACE_UI_CMD_FORMAT_TASK,         0 },

  { "time",     "Set time format",
      XDP_TRACE_UI_CMD_FORMAT_TIME,         0 },

  { "file",     "Set file and line number format",
      XDP_TRACE_UI_CMD_FORMAT_FILE,         0 },

  { "string",   "Set format string format",
      XDP_TRACE_UI_CMD_FORMAT_STRING,       0 },

  { "params",   "Set parameters format",
      XDP_TRACE_UI_CMD_FORMAT_PARAMS,       0 },

  { "bytes",    "Set user bytes format",
      XDP_TRACE_UI_CMD_FORMAT_BYTES,        0 },

    // End of list
  { NULL, "", 0,0} // Don't delete

};


    bool rc;

    // 1. Get buffer state and convert it into active command bit map (one hot)

    XdpTrace::XdpTraceState bufState = ItsTraceP->GetState();

    UINT32 activeCmds = 1 << (UINT32) bufState;

    // 2. Get the sub command of the format command

    UINT32 cmdNumber=0xffffffff; // init to be on the safe side

    rc = ItsParserP->ParseSubCommands("","Set/Show trace format",0, 
                                          formatSubCommands, activeCmds , cmdNumber);

    // Check error
    if (rc == false) {
        return false;
    }

    // 3. Execute the selected format sub command

    bool cmdRc;

    switch (cmdNumber) {


        // Format
        case XDP_TRACE_UI_CMD_FORMAT_SHOW:  
            cmdRc = CommandFormatShow(); 
            break;

        case XDP_TRACE_UI_CMD_FORMAT_SEQ:  
            cmdRc = CommandFormatSeq(); 
            break;
 
        case XDP_TRACE_UI_CMD_FORMAT_NAME:  
            cmdRc = CommandFormatName(); 
            break;
 
        case XDP_TRACE_UI_CMD_FORMAT_ENTRY_NUMBER:  
            cmdRc = CommandFormatEntryNumber(); 
            break;

        case XDP_TRACE_UI_CMD_FORMAT_COLOR:  
            cmdRc = CommandFormatColor(); 
            break;

        case XDP_TRACE_UI_CMD_FORMAT_APP_TAG:
            cmdRc = CommandFormatAppTag();
            break;

        case XDP_TRACE_UI_CMD_FORMAT_TASK:  
            cmdRc = CommandFormatTask(); 
            break;

        case XDP_TRACE_UI_CMD_FORMAT_TIME:  
            cmdRc = CommandFormatTime(); 
            break;

        case XDP_TRACE_UI_CMD_FORMAT_FILE:  
            cmdRc = CommandFormatFile(); 
            break;

        case XDP_TRACE_UI_CMD_FORMAT_STRING:  
            cmdRc = CommandFormatString(); 
            break;

        case XDP_TRACE_UI_CMD_FORMAT_PARAMS:  
            cmdRc = CommandFormatParams(); 
            break;

        case XDP_TRACE_UI_CMD_FORMAT_BYTES:  
            cmdRc = CommandFormatBytes(); 
            break;


        default: // Unknowen commnad number
            PrintString("Error: Invalid format command number " + XdpConv::ToStr(cmdNumber) + "\n");
            return false; 
    }


    // 4. return the command execuation status

    return cmdRc;

}


// --- CommandFormatShow ------------------------------------------------------
//
bool XdpTraceUI::CommandFormatShow() {

    RWCString s;

    // Output sequence
    s = "Sequence Num  : " + ItsTraceP->ToStr(ItsTraceP->GetFormatSeq()) + "\n";
    PrintString(s);

    // Trace name
    s = "Name          : " + ItsTraceP->ToStr(ItsTraceP->GetFormatName()) + "\n";
    PrintString(s);

    // Entry number
    s = "Entry number  : " + ItsTraceP->ToStr(ItsTraceP->GetFormatEntryNum()) + "\n";
    PrintString(s);

    // Color
    s = "Color         : " + ItsTraceP->ToStr(ItsTraceP->GetFormatColor()) + "\n";
    PrintString(s);

    // Task
    s = "Task          : " + ItsTraceP->ToStr(ItsTraceP->GetFormatTask()) + "\n";
    PrintString(s);

    // Time
    s = "Time          : " + ItsTraceP->ToStr(ItsTraceP->GetFormatTime()) + "\n";
    PrintString(s);

    // File & line number
    s = "File & line # : " + ItsTraceP->ToStr(ItsTraceP->GetFormatFile()) + 
            "    Width is " + XdpConv::ToStr(ItsTraceP->GetFormatFileWidth()) + "\n";
    PrintString(s);

    // 
    s = "Format string : " + ItsTraceP->ToStr(ItsTraceP->GetFormatFmtString()) + "\n";
    PrintString(s);

    // 
    s = "Parameters    : " + ItsTraceP->ToStr(ItsTraceP->GetFormatUserParams()) + "\n";
    PrintString(s);

    // User bytes
    s = "User Bytes    : " + ItsTraceP->ToStr(ItsTraceP->GetFormatUserBytes()) + "\n";
    PrintString(s);

    // All done
    return true;

}

// --- CommandFormatSeq -------------------------------------------------------
//
bool XdpTraceUI::CommandFormatSeq() {

    static XdpParseEnum::XdpParseEnumPair formatEnumPairs[] = {
        {"Off",       (UINT32) XdpTrace::XDP_TRACE_FORMAT_OFF},
        {"On",        (UINT32) XdpTrace::XDP_TRACE_FORMAT_ON},
        {"Hex",        (UINT32) XdpTrace::XDP_TRACE_FORMAT_HEX},
        {NULL, 0} };

    bool rc;

    UINT32 format;

    XdpParseEnum parseFormat(format, "format", "The display format", 0, formatEnumPairs );

    rc = ItsParserP->ParseParameters("","",0, &parseFormat);


    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Convert format to the right type
    XdpTrace::XdpTraceFormat formatAsEnum = (XdpTrace::XdpTraceFormat) format;
    

    // Set the format

    rc = ItsTraceP->SetFormatSeq( formatAsEnum );

    return rc;

}

// --- CommandFormatName ------------------------------------------------------
//
bool XdpTraceUI::CommandFormatName() {

    static XdpParseEnum::XdpParseEnumPair formatEnumPairs[] = {
        {"Off",       (UINT32) XdpTrace::XDP_TRACE_FORMAT_OFF},
        {"On",        (UINT32) XdpTrace::XDP_TRACE_FORMAT_ON},
        {"Long",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_LONG},
        {NULL, 0} };

    bool rc;

    UINT32 format;

    XdpParseEnum parseFormat(format, "format", "The display format", 0, formatEnumPairs );

    rc = ItsParserP->ParseParameters( "" , "" , 0, &parseFormat );

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Convert format to the right type
    XdpTrace::XdpTraceFormat formatAsEnum = (XdpTrace::XdpTraceFormat) format;
    

    // Set the format

    rc = ItsTraceP->SetFormatName( formatAsEnum );

    return rc;

}
// --- CommandFormatEntryNumber ----------------------------------------------
//
bool XdpTraceUI::CommandFormatEntryNumber() {

    static XdpParseEnum::XdpParseEnumPair formatEnumPairs[] = {
        {"Off",       (UINT32) XdpTrace::XDP_TRACE_FORMAT_OFF},
        {"On",        (UINT32) XdpTrace::XDP_TRACE_FORMAT_ON},
        {NULL, 0} };

    bool rc;

    UINT32 format;

    XdpParseEnum parseFormat(format, "format", "The display format", 0, formatEnumPairs );

    rc = ItsParserP->ParseParameters( "" , "" , 0 , &parseFormat );

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Convert format to the right type
    XdpTrace::XdpTraceFormat formatAsEnum = (XdpTrace::XdpTraceFormat) format;
    

    // Set the format

    rc = ItsTraceP->SetFormatEntryNum( formatAsEnum );

    return rc;

}
// --- CommandFormatColor -----------------------------------------------------
//
bool XdpTraceUI::CommandFormatColor() {

    static XdpParseEnum::XdpParseEnumPair formatEnumPairs[] = {
        {"Off",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_OFF},
        {"On",       (UINT32) XdpTrace::XDP_TRACE_FORMAT_ON},
        {"Long",     (UINT32) XdpTrace::XDP_TRACE_FORMAT_LONG},
        {"Dec",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_DEC},
        {"Hex",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_HEX},
        {NULL, 0} };

    bool rc;

    UINT32 format;

    XdpParseEnum parseFormat(format, "format", "The display format", 0, formatEnumPairs );

    rc = ItsParserP->ParseParameters( "" , "" , 0 ,&parseFormat);

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Convert format to the right type
    XdpTrace::XdpTraceFormat formatAsEnum = (XdpTrace::XdpTraceFormat) format;
    

    // Set the format

    rc = ItsTraceP->SetFormatColor( formatAsEnum );

    return rc;

}

// --- CommandFormatAppTag -----------------------------------------------------
//
bool XdpTraceUI::CommandFormatAppTag() {

    static XdpParseEnum::XdpParseEnumPair formatEnumPairs[] = {
        {"Off",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_OFF},
        {"On",       (UINT32) XdpTrace::XDP_TRACE_FORMAT_ON},
        {"Long",     (UINT32) XdpTrace::XDP_TRACE_FORMAT_LONG},
        {"Dec",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_DEC},
        {"Hex",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_HEX},
        {NULL, 0} };

    bool rc;

    UINT32 format;

    XdpParseEnum parseFormat(format, "format", "The display format", 0, formatEnumPairs );

    rc = ItsParserP->ParseParameters( "" , "" , 0 ,&parseFormat);

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Convert format to the right type
    XdpTrace::XdpTraceFormat formatAsEnum = (XdpTrace::XdpTraceFormat) format;


    // Set the format

    rc = ItsTraceP->SetFormatAppTag( formatAsEnum );

    return rc;

}


// --- CommandFormatTask ------------------------------------------------------
//
bool XdpTraceUI::CommandFormatTask() {

    static XdpParseEnum::XdpParseEnumPair formatEnumPairs[] = {
        {"Off",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_OFF},
        {"On",       (UINT32) XdpTrace::XDP_TRACE_FORMAT_ON},
        {"Hex",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_HEX},
        {NULL, 0} };

    bool rc;

    UINT32 format;

    XdpParseEnum parseFormat(format, "format", "The display format", 0, formatEnumPairs );

    rc = ItsParserP->ParseParameters( "" , "" , 0 , &parseFormat );

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Convert format to the right type
    XdpTrace::XdpTraceFormat formatAsEnum = (XdpTrace::XdpTraceFormat) format;
    

    // Set the format

    rc = ItsTraceP->SetFormatTask( formatAsEnum );

    return rc;

}

// --- CommandFormatTime----------------------------------------------------
//
bool XdpTraceUI::CommandFormatTime(){

    static XdpParseEnum::XdpParseEnumPair formatEnumPairs[] = {
        {"Off",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_OFF},
        {"On",       (UINT32) XdpTrace::XDP_TRACE_FORMAT_ON},
        {"Long",     (UINT32) XdpTrace::XDP_TRACE_FORMAT_LONG},
        {"Dec",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_DEC},
        {NULL, 0} };

    bool rc;

    UINT32 format;

    XdpParseEnum parseFormat(format, "format", "The display format", 0, formatEnumPairs );

    rc = ItsParserP->ParseParameters( "" , "" , 0 , &parseFormat);

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Convert format to the right type
    XdpTrace::XdpTraceFormat formatAsEnum = (XdpTrace::XdpTraceFormat) format;
    

    // Set the format

    rc = ItsTraceP->SetFormatTime(formatAsEnum );

    return rc;

}

// --- CommandFormatFile ------------------------------------------------------
//
bool XdpTraceUI::CommandFormatFile() {

    static XdpParseEnum::XdpParseEnumPair formatEnumPairs[] = {
        {"Off",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_OFF},
        {"On",       (UINT32) XdpTrace::XDP_TRACE_FORMAT_ON},
        {"Long",     (UINT32) XdpTrace::XDP_TRACE_FORMAT_LONG},
        {NULL, 0} };

    bool rc;

    UINT32 format;
    UINT32 width = ItsTraceP->GetFormatFileWidth();

    XdpParseEnum   parseFormat(format, "format", "The display format", 0, formatEnumPairs );
    XdpParseNumber parseWidth (width,"width", "File and line number field width", 
                               XDP_PARSE_PARAMETER_OPTIONAL);

    rc = ItsParserP->ParseParameters( "" , "" , 0 , &parseFormat, &parseWidth );

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Convert format to the right type
    XdpTrace::XdpTraceFormat formatAsEnum = (XdpTrace::XdpTraceFormat) format;
    

    // Set the format

    rc = ItsTraceP->SetFormatFile( formatAsEnum );
    if (rc == false) {
        return false;
    }

    // Set width

    rc = ItsTraceP->SetFormatFileWidth( width );
    if (rc == false) {
        return false;
    }


    return rc;

}

// --- CommandFormatString ----------------------------------------------------
//
bool XdpTraceUI::CommandFormatString() {

    static XdpParseEnum::XdpParseEnumPair formatEnumPairs[] = {
        {"Off",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_OFF},
        {"On",       (UINT32) XdpTrace::XDP_TRACE_FORMAT_ON},
        {"Hex",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_HEX},
        {NULL, 0} };

    bool rc;

    UINT32 format;

    XdpParseEnum parseFormat(format, "format", "The display format", 0, formatEnumPairs );

    rc = ItsParserP->ParseParameters( "" , "" , 0 , &parseFormat );

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Convert format to the right type
    XdpTrace::XdpTraceFormat formatAsEnum = (XdpTrace::XdpTraceFormat) format;
    

    // Set the format

    rc = ItsTraceP->SetFormatFmtString( formatAsEnum );

    return rc;

}

// --- CommandFormatParams ----------------------------------------------------
//
bool XdpTraceUI::CommandFormatParams() {

    static XdpParseEnum::XdpParseEnumPair formatEnumPairs[] = {
        {"Off",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_OFF},
        {"On",       (UINT32) XdpTrace::XDP_TRACE_FORMAT_ON},
        {"Hex",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_HEX},
        {"UINT32",   (UINT32) XdpTrace::XDP_TRACE_FORMAT_UINT32},
        {"INT32",    (UINT32) XdpTrace::XDP_TRACE_FORMAT_INT32},
        {"App1",     (UINT32) XdpTrace::XDP_TRACE_FORMAT_APP1},
        {"App2",     (UINT32) XdpTrace::XDP_TRACE_FORMAT_APP2},
        {"App3",     (UINT32) XdpTrace::XDP_TRACE_FORMAT_APP3},
        {NULL, 0} };

    bool rc;

    UINT32 format;

    XdpParseEnum parseFormat(format, "format", "The display format", 0, formatEnumPairs );

    rc = ItsParserP->ParseParameters( "" , "" , 0 , &parseFormat );

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Convert format to the right type
    XdpTrace::XdpTraceFormat formatAsEnum = (XdpTrace::XdpTraceFormat) format;
    

    // Set the format

    rc = ItsTraceP->SetFormatUserParams( formatAsEnum );

    return rc;

}

// --- CommandFormatBytes -----------------------------------------------------
//
bool XdpTraceUI::CommandFormatBytes() {

    static XdpParseEnum::XdpParseEnumPair formatEnumPairs[] = {
        {"Off",      (UINT32) XdpTrace::XDP_TRACE_FORMAT_OFF},
        {NULL, 0} };

    bool rc;

    UINT32 format;

    XdpParseEnum parseFormat(format, "format", "The display format", 0, formatEnumPairs );
    rc = ItsParserP->ParseParameters( "" , "" , 0 , &parseFormat);

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Convert format to the right type
    XdpTrace::XdpTraceFormat formatAsEnum = (XdpTrace::XdpTraceFormat) format;
    

    // Set the format

    rc = ItsTraceP->SetFormatUserBytes( formatAsEnum );

    return rc;

}


// --- CommandBuffer ----------------------------------------------------------
//
bool XdpTraceUI::CommandBuffer() {

    static XdpParseEnum::XdpParseEnumPair bufferTypeEnumPairs[] = {
        {"Heap",      (UINT32) XdpTrace::XDP_TRACE_BUFFER_HEAP},
        {"EXT_SRAM",  (UINT32) XdpTrace::XDP_TRACE_BUFFER_EXT_SRAM},
        {"CPU_SRAM",  (UINT32) XdpTrace::XDP_TRACE_BUFFER_CPU_SRAM},
        {NULL, 0} };

    bool rc;

    UINT32 size = 0 ;
    UINT32 bufferType = (UINT32) XdpTrace::XDP_TRACE_BUFFER_HEAP;

    // Parse parameters
    XdpParseNumber parseSize(size,"size","Buffer size in K Bytes. 0 - free buffer",
                             XDP_PARSE_PARAMETER_OPTIONAL,0,100*1000);

    XdpParseEnum   parseBufferType(bufferType, "type", "Buffer memory type", 
                                   XDP_PARSE_PARAMETER_OPTIONAL, bufferTypeEnumPairs );

    rc = ItsParserP->ParseParameters( "" , "" , 0 , &parseSize , &parseBufferType );

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // convert buffer type to enmu

    XdpTrace::XdpTraceBuffer bufTypeAsEnum = (XdpTrace::XdpTraceBuffer) bufferType;

    // Assign or show
    if (parseSize.IsAssigned() == true) {

        // Parameter assigned, set buffer parameters
    
        rc = ItsTraceP->SetBufferSizeAndType( size, bufTypeAsEnum );
        if (rc == false) {
            return false;
        }

    }
   

    // Get buffer size
    size = ItsTraceP->GetBufferSizeInK();

    // Is buffer exists 
    if (size == 0) {
        // No memory
        PrintString( RWCString("No memory is allocated to the buffer\n") );
    }
    else {
        // Memory exists

        // Get buffer type
        bufTypeAsEnum = ItsTraceP->GetBufferType();
        
        // Show it
        RWCString s;

        s = "Buffer size is " + XdpConv::ToStr(size) + "K Bytes at " + 
               ItsTraceP->ToStr( bufTypeAsEnum ) + "\n";
    
        PrintString(s);
    }

    
    // All done 
    return true;

}


// --- CommandCyclic ----------------------------------------------------------
//
bool XdpTraceUI::CommandCyclic() {

    bool rc;

    // Get cyclic flag
    bool cyclicFlag = ItsTraceP->GetCyclicFlag();;

    // Parse parameters
    XdpParseBool parseCyclic(cyclicFlag,"cyclic","Enable (true) = cyclic buffer. Disable (false) = non-cyclic buffer, stop when full",
                             XDP_PARSE_PARAMETER_OPTIONAL);

    rc = ItsParserP->ParseParameters("","",0, &parseCyclic);

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Assign or show
    if (parseCyclic.IsAssigned() == true) {

        // Parameter set, assign it
    
        rc = ItsTraceP->SetCyclicFlag( cyclicFlag );
        if (rc == false) {
            return false;
        }

    }
    
    // Get cyclic flag
    cyclicFlag = ItsTraceP->GetCyclicFlag();

    // Show print if
    if (cyclicFlag == true) {
        // cyclic
        PrintString(RWCString("Buffer mode is cyclic.\n"));
    }
    else {
        // cyclic
        PrintString(RWCString("Buffer mode is non-cyclic (stop when full).\n"));
    }
    
    // All done 
    return true;

}

// --- CommandResolution ------------------------------------------------------
//
bool XdpTraceUI::CommandResolution() {

    bool rc;

    // Get time stamp resolution
    double resolution = ItsTraceP->GetTimeStampRequiredResolution();

    // Parse parameters
    XdpParseDoubleUnits parseResolution(resolution, "Resolution","Time stamp resolution",
                                        XDP_PARSE_PARAMETER_OPTIONAL, 1e-9, 0.1);

    rc = ItsParserP->ParseParameters("","",0, &parseResolution);

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Assign or show
    if (parseResolution.IsAssigned() == true) {

        // Parameter set, assign it
    
        rc = ItsTraceP->SetTimeStampResolution(resolution);
        if (rc == false) {
            return false;
        }

    }
    
    // Get real & required resolutions
    double realResolution     = ItsTraceP->GetTimeStampRealResolution();
    double requiredResolution = ItsTraceP->GetTimeStampRequiredResolution();

    // Calc time stamp max value
    
    // We are with second

    double timeStampMax        = 0xffffffff * realResolution;
    RWCString timeStampMaxText = XdpConv::ToUnitsStr(timeStampMax) + "Sec";

    // Convert into hours ?
    if (timeStampMax >= 60*60) {
        // hours 
        timeStampMaxText = XdpConv::ToUnitsStr(timeStampMax/(60*60)) + "Hours";
    }

    // Convert into days ?
    if (timeStampMax >= 24*60*60) {
        // Days
        timeStampMaxText = XdpConv::ToUnitsStr(timeStampMax/(24*60*60)) + "Days";
    }


    // Show it
    
    PrintString("Time stamp required resolution is " + XdpConv::ToUnitsStr(requiredResolution) + 
                 "Sec\n" );

    PrintString("Time stamp real (HW) resolution is " + XdpConv::ToUnitsStr(realResolution) + 
                 "Sec, Time stamp is up to " + timeStampMaxText +
                 "\n" );
    
    // All done 
    return true;

}


// --- CommandWrite -----------------------------------------------------------
//
bool XdpTraceUI::CommandWrite() {

    UINT32 color;
    INT32  value = 0;
    UINT32 count = 1;

    bool rc;

    XdpParseEnum   parseColor(color, "color", "Entry color",                 
                              0 , TheirColorEnumPairs );

    XdpParseNumber parseCount(count, "count", "Entries count",               
                              XDP_PARSE_PARAMETER_OPTIONAL);

    XdpParseNumber parseValue(value, "value", "First entry parameter value", 
                              XDP_PARSE_PARAMETER_OPTIONAL);

    rc = ItsParserP->ParseParameters( "" , "" , 0 , &parseColor, &parseCount, &parseValue);


    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Write count entries

    UINT32 i;
    for (i = 0; i < count ; i++) {

       XDP_TRACE3(ItsTraceP, color, "Debug entry: value=%d count=%u index=%u",value,count,i);

    }

    // All Done
    return true;

}

// --- CommandPrintColors ----------------------------------------------------
//
bool XdpTraceUI::CommandPrintColors() {        


    bool rc;
    UINT32 i;
    const UINT32 colorNum = 5;

    // enable/disable Flag
    bool enableFlag = false;
    
    UINT32 appTag = XDP_TRACE_APP_ALL; // ALL application

    // the colors
    UINT32 colors[colorNum];

    // Init the colors

    for( i = 0 ; i < colorNum ; i++) {
 
        colors[i] = XDP_TRACE_COLOR_NONE;
    }

    // parse paramerters objects
    XdpParseBool   parseEnableFlag(enableFlag, "en/dis", 
                   "Enable or disable the listed color. When omited show the active colors",
                   XDP_PARSE_PARAMETER_OPTIONAL);

    XdpParseEnum   parseColor0(colors[0], "Color", "A color - omit for all colors", 
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);

    XdpParseEnum   parseColor1(colors[1], "Color", "A color", 
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);

    XdpParseEnum   parseColor2(colors[2], "Color", "A color", 
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);

    XdpParseEnum   parseColor3(colors[3], "Color", "A color", 
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);
    
    XdpParseEnum   parseColor4(colors[4], "Color", "A color", 
                   XDP_PARSE_PARAMETER_OPTIONAL, TheirColorEnumPairs);

    // make array of parse colors & fill it
    XdpParseEnum   *parseColors[colorNum];

    parseColors[0] = &parseColor0;
    parseColors[1] = &parseColor1;
    parseColors[2] = &parseColor2;
    parseColors[3] = &parseColor3;
    parseColors[4] = &parseColor4;

    // Parse parameters
    rc = ItsParserP->ParseParameters("","",0, &parseEnableFlag,
                     &parseColor0, &parseColor1, &parseColor2, &parseColor3, &parseColor4);

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // In color mask, each color has a bit
    UINT32 colorMask;

    // If paramter enable/disable parameter process the colors for set

    if ( parseEnableFlag.IsAssigned() == true ) {
        
        // We set the colors, build color mask
        colorMask = 0; // Empty mask

        // If all colors are omitted, then set all colors
        if (parseColors[0]->IsAssigned() == false) {

            // First color is omitted, set all colors

            for( i = XDP_TRACE_COLOR_FIRST ; i <= XDP_TRACE_COLOR_LAST ; i++) {

                // Add the color to the color mask

                colorMask |= 1 << i;
            } // for

        }
        else {

            // At least one color is set, use the color list
            // Convert color parameters into a color mask
                
            for( i = 0 ; i < colorNum ; i++) {

                // Add the color to the color mask, unassigned color have the default none
                colorMask |= 1 << (UINT32) colors[i];
            } // for

        } // colors set
    
        // Now we have the color mask

        // Update the colors
        rc = ItsTraceP->UpdatePrintColors(enableFlag, colorMask, appTag);

        // Check error
        if (rc == false) {
            return false;
        }

    } // Enable/disable flag assigned
    else {

        // Just show, get and print the enabled color status
    UINT32 *appTagsColorMask;

        appTagsColorMask = ItsTraceP->GetPrintColors();
        ShowColorsStatus(appTagsColorMask);

        // fake command is OK

        rc = true;

    } // Enable/disable flag assigned

    // Done
    return rc;

}

// --- CommandPrintAboveColor -------------------------------------------------
//

bool XdpTraceUI::CommandPrintAboveColor() {        


    bool rc;
    UINT32 i;

    // enable/disable Flag
    bool enableFlag;
    
    UINT32 appTag = XDP_TRACE_APP_ALL; // ALL application

    // the color;
    UINT32 color;

    // Parse parameters

    XdpParseBool parseEnableFlag(enableFlag, "en/dis", "Enable or disable colors" , 0) ;

    XdpParseEnum parseColor(color,      "color" , "Act from this color to the last color", 
                            0, TheirColorEnumPairs);

    rc = ItsParserP->ParseParameters( "" , "" , 0 , &parseEnableFlag , &parseColor );

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // In color mask, each color has a bit, init to an empty mask
    UINT32 colorMask = 0;

    // Loop all color for the user color to the last one
    for( i = color ; i <= XDP_TRACE_COLOR_LAST ; i++) {

        // Add the color to the color mask

        colorMask |= 1 << i;
    } // for
        
    // Now we have the color mask

    // Update the colors
    rc = ItsTraceP->UpdatePrintColors(enableFlag, colorMask, appTag);

    // Check error
    if (rc == false) {
        return false;
    }

    // Done
    return rc;

}

 
// --- CommandPrintBelowColor ------------------------------------------------------
//

bool XdpTraceUI::CommandPrintBelowColor() {        


    bool rc;
    UINT32 i;

    // enable/disable Flag
    bool enableFlag;
    
    UINT32 appTag = XDP_TRACE_APP_ALL; // ALL application

    // the color;
    UINT32 color;

    // Parse parameters
    XdpParseBool parseEnableFlag(enableFlag, "en/dis", "Enable or disable colors" , 0);

    XdpParseEnum parseColor(color,      "color" , "Act from first color to the this color", 
                            0, TheirColorEnumPairs) ;

    rc = ItsParserP->ParseParameters( "" , "" , 0 , &parseEnableFlag, &parseColor );

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // In color mask, each color has a bit, init to an empty mask
    UINT32 colorMask = 0;

    // Loop all color for the first color to the user color
    for( i = XDP_TRACE_COLOR_FIRST ; i <= color ; i++) {

        // Add the color to the color mask

        colorMask |= 1 << i;
    } // for
        
    // Now we have the color mask

    // Update the colors
    rc = ItsTraceP->UpdatePrintColors(enableFlag, colorMask, appTag);

    // Check error
    if (rc == false) {
        return false;
    }

    // Done
    return rc;

}

// --- CommandPrintOutput -----------------------------------------------------
//
bool XdpTraceUI::CommandPrintOutput() {        

    bool rc;

    // Get output channel
    UINT32 out = ItsTraceP->GetPrintOutputChannel();

    // Output parser
    XdpParseEnum out_p (out, "OutputChannel", "Print output channel", 
                          XDP_PARSE_PARAMETER_OPTIONAL, TheirPrintOutputEnumPairs);
 
    rc = ItsParserP->ParseParameters("","",0,
        &out_p
        );

    // Check parameters error
    if (rc == false) {
        return false;
    }

    // Convert output to the right type
    XdpTrace::XdpTraceOutput outAsEnum = (XdpTrace::XdpTraceOutput) out;
    
    // Check if set
    if (out_p.IsAssigned() == true) {
        // Set command, set it
        rc = ItsTraceP->SetPrintOutputChannel(outAsEnum);
        if (rc == false) {
            return false;
        }
    }

    // Get the output current value

    outAsEnum = ItsTraceP->GetPrintOutputChannel();

    // Print it
    RWCString s;

    s = "The print output channel is " + ItsTraceP->ToStr(outAsEnum) + "\n";
    PrintString(s);

    // All done
    return true;

}

// ============================================================================
// ===
// === Status functions
// ===
// ============================================================================

// --- CommandStatus ----------------------------------------------------------
// 
// Print trace status
//   
bool XdpTraceUI::CommandStatus() {        

    INT32 w = -23;

    PrintString("\n");

    PrintString( XdpConv::Pad("Long name: "  , w ) + ItsTraceP->ItsLongName + "\n" );
    PrintString( XdpConv::Pad("Short name: " , w ) + ItsTraceP->ItsShortName + "\n");

    PrintString("\n");

    PrintString( XdpConv::Pad("Enabled application tags:\n" , w ) + ItsTraceP->AppTagMaskToStr(ItsTraceP->ItsEnabledAppTags) + "\n");
    PrintString( XdpConv::Pad("Print application tags colors:\n" , w ) + ItsTraceP->AppTagMaskToStr(ItsTraceP->ItsPrintActiveAppTags) + "\n");
    PrintString( XdpConv::Pad("Enabled colors:\n" , w ) + ItsTraceP->ColorMaskToStr(ItsTraceP->ItsEnabledColors) + "\n");
    PrintString( XdpConv::Pad("Output channel: "  , w ) + ItsTraceP->ToStr(ItsTraceP->ItsOutputChannel) + "\n");
    PrintString( XdpConv::Pad("Display entries: " , w ) + XdpConv::ToStr(ItsTraceP->ItsDefaultDisplayCount) + "\n");
    PrintString( XdpConv::Pad("Output relief: "  , w ) + XdpConv::ToStr(ItsTraceP->ItsOutputReliefNumber) + "\n");

    PrintString("\n");

    PrintString( XdpConv::Pad("Buffer state: "         , w ) + ItsTraceP->ToStr(ItsTraceP->ItsState) +  "\n");
    PrintString( XdpConv::Pad("Active Buffer: "        , w ) + XdpConv::ToStr(ItsTraceP->ItsBufferActiveFlag) + "\n");
    PrintString( XdpConv::Pad("Full buffer: "          , w ) + XdpConv::ToStr(ItsTraceP->ItsFullStopFlag) +  "\n");
    PrintString( XdpConv::Pad("Buffer size: "          , w ) + XdpConv::ToStr(ItsTraceP->ItsBufferSizeInK) + "K \n");
    PrintString( XdpConv::Pad("Buffer type: "          , w ) + ItsTraceP->ToStr(ItsTraceP->ItsBufferType) +  "\n");
    PrintString( XdpConv::Pad("Cyclic buffer: "        , w ) + XdpConv::ToStr(ItsTraceP->ItsCyclicFlag) +  "\n");
    PrintString( XdpConv::Pad("Buffer last entry: "    , w ) + XdpConv::ToStr(ItsTraceP->ItsEntryCounter) +  "\n");
    PrintString( XdpConv::Pad("Buffer wrap counter: "  , w ) + XdpConv::ToStr(ItsTraceP->ItsWrapCounter) +  "\n");

    PrintString("\n");
    
    PrintString( XdpConv::Pad("Time stamp required resolution: ", w ) + XdpConv::ToUnitsStr(ItsTraceP->ItsTimeStampRequiredResolution) + "\n");
    PrintString( XdpConv::Pad("Time stamp real(HW) resolution:  ", w )  + XdpConv::ToUnitsStr(ItsTraceP->ItsTimeStampRealResolution) + "\n");
    
    PrintString("\n");

    PrintString( XdpConv::Pad("Cursor valid: " , w ) + XdpConv::ToStr(ItsTraceP->IsCursorValid()) +  "\n");
    PrintString( XdpConv::Pad("Cursor entry: " , w ) + XdpConv::ToStr(ItsTraceP->ItsCursorEntryNumber) +  "\n");

    PrintString("\n");


    PrintString( XdpConv::Pad( "Print output channel: " , w ) + ItsTraceP->ToStr(ItsTraceP->ItsPrintOutputChannel) +  "\n");
    PrintString( XdpConv::Pad( "Print colors:\n" , w ) + ItsTraceP->ColorMaskToStr(ItsTraceP->ItsPrintActiveColors) +  "\n");

    PrintString("\n");

    // All done
    return true;
}

// --- CommandInternalStatus --------------------------------------------------
// 
// Print trace internal status
//   
bool XdpTraceUI::CommandInternalStatus() {        

    INT32 w = -27;
    
    PrintString("\n");    

    PrintString( XdpConv::Pad("Buffer active colors: "    , w ) + ItsTraceP->ColorMaskToStr(ItsTraceP->ItsBufferActiveColors) + "\n");
    PrintString( XdpConv::Pad("Buffer base pointer: "     , w ) + XdpConv::ToHexStr(ItsTraceP->ItsBufferBaseP) + "\n");
    PrintString( XdpConv::Pad("Buffer end pointer: "      , w ) + XdpConv::ToHexStr(ItsTraceP->ItsBufferEndP) + "\n");
    PrintString( XdpConv::Pad("Buffer wrap pointer: "     , w ) + XdpConv::ToHexStr(ItsTraceP->ItsBufferWrapP) + "\n");
    PrintString( XdpConv::Pad("Buffer write pointer: "    , w ) + XdpConv::ToHexStr(ItsTraceP->ItsBufferWriteP) + "\n");
    PrintString( XdpConv::Pad("Buffer size in records: "  , w ) + XdpConv::ToStr(ItsTraceP->ItsBufferSizeInRecords) + "\n");
    PrintString( XdpConv::Pad("Max records in entry: "    , w ) + XdpConv::ToStr(ItsTraceP->ItsMaxRecordsInEntry) + "\n");
    PrintString( XdpConv::Pad("Time stamp Parameter: "    , w ) + XdpConv::ToStr(ItsTraceP->ItsTimeStampParameter) + "\n");
    
    PrintString("\n");    
    
    PrintString( XdpConv::Pad("Cursor entry pointer: "    , w ) + XdpConv::ToHexStr(ItsTraceP->ItsCursorEntryP) + "\n");
    PrintString( XdpConv::Pad("Cursor entry number: "     , w ) + XdpConv::ToStr(ItsTraceP->ItsCursorEntryNumber) + "\n");
    PrintString( XdpConv::Pad("Cursor wrap counter: "     , w ) + XdpConv::ToStr(ItsTraceP->ItsCursorEntryNumber) + "\n");
    
    PrintString("\n");    

    PrintString( XdpConv::Pad("Print seq. number: "       , w ) + XdpConv::ToStr(ItsTraceP->ItsPrintSeqNumber) + "\n");
    PrintString( XdpConv::Pad("Display seq. number: "     , w ) + XdpConv::ToStr(ItsTraceP->ItsDisplaySeqNumber) + "\n");
    PrintString( XdpConv::Pad("Output Relief counter: "   , w ) + XdpConv::ToStr(ItsTraceP->ItsOutputReliefCounter) + "\n");
    
    PrintString("\n");
    
    PrintString( XdpConv::Pad("User interface pointer: "  , w ) + XdpConv::ToHexStr(ItsTraceP->ItsUserInterfaceP) + "\n");
    PrintString( XdpConv::Pad("XDP Services pointer: "    , w ) + XdpConv::ToHexStr(ItsTraceP->ItsXdpServicesP) + "\n");

    PrintString("\n");

    PrintString( XdpConv::Pad("Seek inter relief: "       , w ) + XdpConv::ToStr(ItsTraceP->ItsSeekInterReliefNumber) + "\n");
    PrintString( XdpConv::Pad("Seek inter relief count: " , w ) + XdpConv::ToStr(ItsTraceP->ItsSeekInterReliefCounter) + "\n");
    PrintString( XdpConv::Pad("Seek task relief: "        , w ) + XdpConv::ToStr(ItsTraceP->ItsSeekTaskReliefNumber) + "\n");
    PrintString( XdpConv::Pad("Seek task relief count: "  , w ) + XdpConv::ToStr(ItsTraceP->ItsSeekTaskReliefCounter) + "\n");
       
    // All done
    return true;
}

// --- CommandCounters --------------------------------------------------------
// 
// Print trace counters
//   
bool XdpTraceUI::CommandCounters() {        
    
    // Description width
    INT32 width = -33;

    // Counter width
    UINT32 pad = 6;

    // Print it

    PrintString("\n");

    PrintString(XdpConv::Pad("Non active discards",width) + 
                XdpConv::ToStr(ItsTraceP->_ItsNonActiveDiscardCounter,pad) + "\n");

    PrintString("\n");

    PrintString(XdpConv::Pad("Print output errors",width) + 
                XdpConv::ToStr(ItsTraceP->ItsPrintOutputErrorCounter,pad) + "\n");

    PrintString(XdpConv::Pad("Print inside interrupt discards",width) + 
                XdpConv::ToStr(ItsTraceP->ItsPrintInterruptDiscardCounter,pad) + "\n");

    PrintString("\n");

    // All done
    return true;
}

// ============================================================================
// ===
// === Misc functions
// ===
// ============================================================================

// --- PrintString ------------------------------------------------------------
// 
// Print string to the command manager
//   
void XdpTraceUI::PrintString(const RWCString &theString) {


    // Print the string
    ItsCmdMngrP->PrintString(theString);

}


// --- ShowCursorEntry --------------------------------------------------------
// 
// Print the current cursor entry
//   
bool XdpTraceUI::ShowCursorEntry() {

    bool rc;

    UINT32 entry;

    rc = ItsTraceP->GetCursorEntryNumber(&entry);
    if (rc == false) {
        return false;
    }

    RWCString s;

    s = "Cursor at entry " + XdpConv::ToStr(entry) + ".\n";

    PrintString(s);

    return true;

}

// --- ShowColorsStatus -------------------------------------------------------
// 
// Print enabled colors by the given color mask
//   
void XdpTraceUI::ShowColorsStatus(UINT32 *theAppTagsColorMask) {

    RWCString s;

     // Check all color are disable
    if ((ItsTraceP->IsAllColorsDisabled(theAppTagsColorMask)) == true) {

        // All colors are disabled
        s = "All colors are disabled. \n";
    }
    else {
        // At least one color is enabled

        s = "The enabled colors are:";

        // Scan all colors
        
        UINT32 appTag;

        for (appTag = XDP_TRACE_APP_FIRST; appTag <= XDP_TRACE_APP_LAST; appTag++)
        {

            s+= " " + ItsTraceP->ToStr((XdpTraceAppTag) appTag) + ":";

        UINT32 color;

        for (color = XDP_TRACE_COLOR_FIRST; color <= XDP_TRACE_COLOR_LAST; color++) {

            // Color i enabled ?
            
                if ( ( theAppTagsColorMask[appTag - 1] & (1 << color) ) != 0 ){

                // Enabled, add to the list
                s += " " + ItsTraceP->ToStr((XdpTraceColor) color );
            }
            
            } // for - colors

        s += ".\n";
        } // for - tags
    } // else


    // Print it
    PrintString(s);

}

// --- ShowAppTagsStatus -------------------------------------------------------
//
// Print enabled application tags by the given application tag mask
//
void XdpTraceUI::ShowAppTagsStatus(UINT32 theAppTagMask) {

    RWCString s;

     // Check all color are disable
    if (theAppTagMask == 0) {

        // All tags are disabled
        s = "All application tags are disabled. \n";
    }
    else {
        // At least one tag is enabled

        s = "The enabled tags are:";

        // Scan all tags

        UINT32 appTag;

        for (appTag = XDP_TRACE_APP_FIRST; appTag <= XDP_TRACE_APP_LAST; appTag++) {

            // App tag i enabled ?

            if ( ( theAppTagMask & (1 << appTag) ) != 0 ){

                // Enabled, add to the list
                s += " " + ItsTraceP->ToStr((XdpTraceAppTag) appTag );
            }

        } // for

        s += ".\n";

    } // else


    // Print it
    PrintString(s);

}

// ---TestTrace -------------------------------------------------------
//
// Print traces for testing.
//

bool XdpTraceUI::CommandTest() {

    XDP_TRACE0(ItsTraceP,XDP_TRACE_WHITE,"this is a test for regular XDP trace");
    XDP_TRACE0(ItsTraceP,XDP_TRACE_GREEN,"this is a test for regular XDP trace");
    XDP_TRACE0(ItsTraceP,XDP_TRACE_RED,"this is a test for regular XDP trace");
    XDP_TRACE0(ItsTraceP,XDP_TRACE_YELLOW,"this is a test for regular XDP trace");
    XDP_TRACE0(ItsTraceP,XDP_TRACE_ORANGE,"this is a test for regular XDP trace");
    XDP_TRACE0(ItsTraceP,XDP_TRACE_BLACK,"this is a test for regular XDP trace");

    XDP_TRACE_ENH0(ItsTraceP,XDP_TRACE_GREEN,XDP_TRACE_APP_DEFAULT,"this is a test for ENHANCED XDP trace with default app tag ");
    XDP_TRACE_ENH0(ItsTraceP,XDP_TRACE_GREEN,XDP_TRACE_APP1,"this is a test for ENHANCED XDP trace with app tag number 1");
    XDP_TRACE_ENH0(ItsTraceP,XDP_TRACE_GREEN,XDP_TRACE_APP2,"this is a test for ENHANCED XDP trace with app tag number 2");
    XDP_TRACE_ENH0(ItsTraceP,XDP_TRACE_GREEN,XDP_TRACE_APP3,"this is a test for ENHANCED XDP trace with app tag number 3");
    XDP_TRACE_ENH0(ItsTraceP,XDP_TRACE_GREEN,XDP_TRACE_APP4,"this is a test for ENHANCED XDP trace with app tag number 4");
    XDP_TRACE_ENH0(ItsTraceP,XDP_TRACE_GREEN,XDP_TRACE_APP5,"this is a test for ENHANCED XDP trace with app tag number 5");
    XDP_TRACE_ENH0(ItsTraceP,XDP_TRACE_GREEN,XDP_TRACE_APP6,"this is a test for ENHANCED XDP trace with app tag number 6");
    XDP_TRACE_ENH0(ItsTraceP,XDP_TRACE_GREEN,XDP_TRACE_APP7,"this is a test for ENHANCED XDP trace with app tag number 7");
    XDP_TRACE_ENH0(ItsTraceP,XDP_TRACE_GREEN,XDP_TRACE_APP8,"this is a test for ENHANCED XDP trace with app tag number 8");
    XDP_TRACE_ENH0(ItsTraceP,XDP_TRACE_GREEN,XDP_TRACE_APP9,"this is a test for ENHANCED XDP trace with app tag number 9");
    XDP_TRACE_ENH0(ItsTraceP,XDP_TRACE_GREEN,XDP_TRACE_APP10,"this is a test for ENHANCED XDP trace with app tag number 10");
    XDP_TRACE_ENH0(ItsTraceP,XDP_TRACE_GREEN,XDP_TRACE_APP11,"this is a test for ENHANCED XDP trace with app tag number 11");
    XDP_TRACE_ENH0(ItsTraceP,XDP_TRACE_GREEN,XDP_TRACE_APP12,"this is a test for ENHANCED XDP trace with app tag number 12");
    XDP_TRACE_ENH0(ItsTraceP,XDP_TRACE_GREEN,XDP_TRACE_APP13,"this is a test for ENHANCED XDP trace with app tag number 13");
    XDP_TRACE_ENH0(ItsTraceP,XDP_TRACE_GREEN,XDP_TRACE_APP14,"this is a test for ENHANCED XDP trace with app tag number 14");

    return true;
}
