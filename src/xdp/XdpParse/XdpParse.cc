/* XdpParse.cc - <module's short description>  */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpParse
   This base is used to parse easily command line strings. Usually it is
   used to easily interpret command manager command arguments.

AUTHOR
   Gaash, SB

		
*************************************************************************/

#include "prepc.h"

#include "RWHdr/cstring.h"
#include "CmdMngr.h"
#include "XdpConv.h"
#include "XdpParseParameter.h"
#include "XdpParse.h"


//-------------------------------------------------------------------------
// public
//-------------------------------------------------------------------------

// Constructor & Destructor

// --- ctor -------------------------------------------------------------------
//
// The ctor init some class vars, get the CmdMngr line and saves
//   the CmdMngr pointer for future use.
//
// Input:
//    theCmdMngr     - The CmdMngr is used for user IO
//


XdpParse::XdpParse(CmdMngr *theCmdMngr) :
    ItsCmdMngrP(theCmdMngr),
    ItsFlags(0),
    ItsOK(false),
	ItsTokenP(NULL)
{

    // Get the full string
    char *fullStr=ItsCmdMngrP->GetFullString();
    //
    if (fullStr==NULL) {
        // Empty string
        ItsOriginalTokenString="";
    }
    else {
        // String preset
        ItsOriginalTokenString=fullStr;
    }

    // Use the string for token processing
    SetTokenString(ItsOriginalTokenString);

  }

// --- ParseParameters --------------------------------------------------------
//
// The function is the main API function to process parameters
//
// Input:
//    theCommandName - command name
//    theDescription - command description
//    theFlags       - or-ed XDP_PARSE_xxxx flags - currently not used
//    P1..P16        - Command paramters. must derive from XdpParseParameter
//                     The first null agrument, terminate the parameter list
//
// If the command name is empty ("") the internal command name is used.
// If the description is empty ("") the internal description is used.
// The above exception are usefull the PrasePramerers is called after
// ParseSubCommand and those internal field are set by it.
//
// Return if the parsing was OK
//	

bool XdpParse::ParseParameters(
             const RWCString &theCommandName,
             const RWCString &theDescription,
             UINT32   theFlags,         // Operation flags
             // The paramertes, upto 16, NULL if not used
             XdpParseParameter *theP1  , XdpParseParameter *theP2 ,
             XdpParseParameter *theP3  , XdpParseParameter *theP4 ,
             XdpParseParameter *theP5  , XdpParseParameter *theP6 ,
             XdpParseParameter *theP7  , XdpParseParameter *theP8 ,
             XdpParseParameter *theP9  , XdpParseParameter *theP10,
             XdpParseParameter *theP11 , XdpParseParameter *theP12,
             XdpParseParameter *theP13 , XdpParseParameter *theP14,
             XdpParseParameter *theP15 , XdpParseParameter *theP16)
{

    // If the command name is not empty, use it, else stay with the internal command name
    if (theCommandName != "") {
        // Not empry - use it
        ItsCommandName    = theCommandName;
    }

    // If the descirption is not empty, use it, else stay with the internal description
    if (theDescription != "") {
        // Not empry - use it
        ItsDescription = theDescription;
    }

  	// Assign some argumenets

    ItsFlags          = theFlags;
    ItsOK             = false;
    ItsLastAssigned   = 0;
    ItsNumberOfParams = 0;
    ItsDebugFlag      = false;
    ItsHelpFlag       = false;
    ItsPrintUsageFlag = false;

    // Make an array of parameters pointers, as SW like
    ItsParam[0]  = theP1;
    ItsParam[1]  = theP2;
    ItsParam[2]  = theP3;
    ItsParam[3]  = theP4;
    ItsParam[4]  = theP5;
    ItsParam[5]  = theP6;
    ItsParam[6]  = theP7;
    ItsParam[7]  = theP8;
    ItsParam[8]  = theP9;
    ItsParam[9]  = theP10;
    ItsParam[10] = theP11;
    ItsParam[11] = theP12;
    ItsParam[12] = theP13;
    ItsParam[13] = theP14;
    ItsParam[14] = theP15;
    ItsParam[15] = theP16;

    // Process the line
    ItsOK = ParamsProcessLine();

    // Print usage if required
    if ((ItsPrintUsageFlag == true) || (ItsHelpFlag == true)) {
        // Print usage
        ParamsPrintUsage();
    }

    // Print debug info if requied
    if (ItsDebugFlag == true) {
        // Print debug info
        ParamsPrintDebug();
    }

	return (ItsOK);
}

// --- ParseSubCommands -------------------------------------------------------
//
// The function is the main API function to process sub command (like a menu)
//
// Input:
//    theCommandName     - command name
//    theDescription     - command description
//    theFlags           - or-ed XDP_PARSE_xxxx flags - currently not used
//    theSubCommandsP    - An array of XdpParseSubCommandEntry which define the
//                         sub command list. Each entry has:
//                         name        - Sub command name
//                         description - Desciption text (for usage messge)
//                         id          - The command it that will be set if selected
//                         group mask  - Bitmap that defined to which groups it belongs
//                         An entry with NULL name terminates the list
//
//    theActiveGroups   - Defines which group is enabled.
//
// Output:
//    theSelectedId      - The selected command ID (only if OK)
//
//
// There are 32 command groups. Each command can be member in one or more groups
// as defined by the command group mask bit vector. Command can be selected only
// from an active group. An command that does not belong to any of the active groups
// is like a grayed-out item in a menu.
//
// For simplicity there is an execption: a command that has a zero group mask
// is considered member in all groups. This enable to put zero when the command is
// always ready.
//
// If OK, the selected sub command name is appended to the intenally save command
// name and the is description is also save internaly. This is preperation for the
// following call to ParseParametes
//
// Returns true if the parsing was OK
//	

bool XdpParse::ParseSubCommands(
             const RWCString &theCommandName,
             const RWCString &theDescription,
             UINT32           theFlags,
             const XdpParse::XdpParseSubCommand *theSubCommandsP,
             UINT32           theActiveGroups,
             UINT32          &theSelectedId)
{

	// Assign some argumenets
    ItsCommandName    += theCommandName; // Append
    ItsDescription    = theDescription;
    ItsFlags          = theFlags;
    ItsOK             = false;
    ItsDebugFlag      = false;
	ItsHelpFlag       = false;
    ItsPrintUsageFlag = false;

    // Process the sub commands line
    ItsOK = SubCmdsProcessLine(theSubCommandsP, theActiveGroups, theSelectedId);

    // Print usage if required
    if ((ItsPrintUsageFlag == true) || (ItsHelpFlag == true)) {
        // Print usage
        SubCmdsPrintUsage(theSubCommandsP, theActiveGroups);
    }

    // Print debug info if requied
    if (ItsDebugFlag == true) {
        // Print debug info
        SubCmdsPrintDebug(theSubCommandsP, theActiveGroups, theSelectedId);
    }

	return (ItsOK);
}

//--- GetToken ----------------------------------------------------------------
bool XdpParse::GetToken(RWCString &theToken){

// to be improved !!!!!!!!!!!!1to be improved !!!!!!!!!!!!1

    // First time ?
    if (ItsFirstTokenFlag == true) {
        // First time
        strcpy(ItsLineBuffer, ItsTokenString.data());
        ItsTokenP = strtok(ItsLineBuffer, " \t\n");
        ItsFirstTokenFlag = false;
    }
    else {

        if (ItsTokenP != NULL) {
            ItsTokenP = strtok(NULL, " \t\n");
        }
    }

    // Do we have something
    if (ItsTokenP==NULL) {
        // No
        theToken="!EMPTY!";
        return false;
    }
    else {
        theToken=ItsTokenP;
        return true;
    }

}

//--- GetRestOfLineLine -------------------------------------------------------
bool XdpParse::GetRestOfLine(RWCString &theLine){

// to be improved !!!!!!!!!!!!1to be improved !!!!!!!!!!!!1

    // First time ?
    if (ItsFirstTokenFlag == true) {
        // First time
        strcpy(ItsLineBuffer, ItsTokenString.data());
        ItsTokenP = strtok(ItsLineBuffer, "\n");
        ItsFirstTokenFlag = false;
    }
    else {

        if (ItsTokenP != NULL) {
            ItsTokenP = strtok(NULL, "\n");
        }
    }

    // Do we have something
    if (ItsTokenP==NULL) {
        // No
        theLine="!EMPTY!";
        return false;
    }
    else {
        theLine=ItsTokenP;
        return true;
    }

}

//-------------------------------------------------------------------------
// protected
//-------------------------------------------------------------------------

// ========================================================================
// ===
// === Parse paramters functions
// ===
// ========================================================================

// --- ParamsProcessLine --------------------------------------------------------
//
// Process the paramters. This is the PraseParameters main function
//
// The processing has a few steps
//    1. Find the number of parameters
//    2. Check internal flags
//    3. Test paramets
//    4. Set parameters
//

bool XdpParse::ParamsProcessLine()
{


    ItsLastAssigned = 0;

    UINT32 i;

    // 1. find the number of parametes
    ItsNumberOfParams = 0;
    for(i=0; i < XDP_PARSE_MAX_PARAMS ; i++) {
        // Break if last parameter
        if (ItsParam[i] == NULL) {
            break;
        }
        ItsNumberOfParams++;
    }


    // 2. Process internal flags

    bool OK;

    OK = ProcessInternalFlags();

    // Check errors in internal flags
    if (OK == false) {
        // Error
        PrintString("Error with internal flags processing.\n");
        ItsPrintUsageFlag = true;
    }

    // Check if print usage is required - error occured
    if (ItsPrintUsageFlag == true) {
        // Error
        return false;
    }

    // Check if the help flag is set
    if (ItsHelpFlag == true) {
       // help flag is set, don't execute the command (just print the help msg)
       return false;
    }

    // 3. Test params

    for(i=0; i < ItsNumberOfParams ; i++) {

        // Test param
        OK = ItsParam[i]->PreSetTest(this);

        if (OK == false) {
            // 5. Parameter failure
            PrintString("Error with " + ItsParam[i]->GetNameString() +
                        ": " + ItsParam[i]->GetErrorString() + "\n");

            ItsPrintUsageFlag = true;

            return false;
        }
    }


    // All params are OK

    // Check that all tokens has been used

    RWCString t;
    bool tokenOK = GetToken(t);

    if (tokenOK == true) {
         PrintString("Error: Extra parameters on line \"" + t + "\"\n");

         ItsPrintUsageFlag = true;

         return false;
    }


    // 4. Set Parameters
    ItsLastAssigned = 0;
    for(i=0; i < ItsNumberOfParams ; i++) {

        // Test param
        bool WasSet = ItsParam[i]->Set();

        if (WasSet == true) {
            ItsLastAssigned = i + 1;
        }
    }

    // All Done, all OK

    return true;

}

// --- ParamsPrintUsage -------------------------------------------------------
// Print command usage for ParseParameters
void XdpParse::ParamsPrintUsage()
{

    RWCString s;

    UINT32 i;

    // 1. Print Description

    PrintString("\n" + ItsDescription + "\n");


    // 2. Print syntax line

    s = "Usage: " + ItsCommandName + " [-help|-debug] ";

    for(i=0; i < ItsNumberOfParams ; i++) {

        s += " " + ItsParam[i]->GetUsageString();

    }

    PrintString(s+"\n");

    // 3. Print the descriptions

    // Find the longest parameter name

    UINT32 maxParamLen =0;
    UINT32 paramLen;
    for(i=0; i < ItsNumberOfParams ; i++) {

        // Get parameter name length
        paramLen = ItsParam[i]->GetNameString().length();
        // Update max length
        if (paramLen > maxParamLen) {
            // Yes
            maxParamLen = paramLen;
        }
    }

    // Print the paramter info
    for(i=0; i < ItsNumberOfParams ; i++) {

        // Resize name
        RWCString resizedName = ItsParam[i]->GetNameString();
        resizedName.resize((unsigned int)maxParamLen);

        // Parameter line

        s = "  " + resizedName + " " + ItsParam[i]->GetDescriptionString() + "\n";

        PrintString(s);
    }


}

// --- ParmasPrintDebug ---------------------------------------------------
// Print Debug information for ParseParameters
void XdpParse::ParamsPrintDebug()
{

    RWCString s;

    UINT32 i;

    // Print Parser debug line

    s = "\n\n DebugInfo - " + ItsCommandName + " -";

    // Add ItsOK
    if (ItsOK==true) {
        // OK
        s += " IsOK=true";
    }
    else {
        // Not OK
        s += " IsOK=false";
    }

    // Add Last ItsLastAssigned
    char buf[30];

    sprintf(buf," ItsLastAssigned=%d",ItsLastAssigned);

    s += buf;

    PrintString(s+"\n");

    // Print the debug line per parameter

    // Find the longest parameter name

    UINT32 maxParamLen =0;
    UINT32 paramLen;
    for(i=0; i < ItsNumberOfParams ; i++) {

        // Get parameter name length
        paramLen = ItsParam[i]->GetNameString().length();
        // Update max length
        if (paramLen > maxParamLen) {
            // Yes
            maxParamLen = paramLen;
        }
    }

    // Print the paramter info
    for(i=0; i < ItsNumberOfParams ; i++) {

        // Resize name
        RWCString resizedName = ItsParam[i]->GetNameString();
        resizedName.resize( (unsigned int) maxParamLen );

        // Parameter line

        s = "  " + resizedName + " " + ItsParam[i]->GetDebugString() + "\n";

        PrintString(s);
    }


    PrintString("\n");
}

// ========================================================================
// ===
// === Parse sub commands functions
// ===
// ========================================================================

// --- SubCmdsProcessLine -------------------------------------------------------
//
// Process the sub commmands. This is the ParseSubCommands main function
//
// The processing has a few steps
//    1. Check internal flags
//    2. Get command token
//    3. Find command id
//    4. Check found command is active
//    5. Set command id
//    6. Save append the sub command name to ItsCommandName and
//         set the description into ItsDescription (for use by the parse parameters)

bool XdpParse::SubCmdsProcessLine(
             const XdpParse::XdpParseSubCommand *theSubCommandsP,
             UINT32                              theActiveGroups,
             UINT32                             &theSelectedId)
{


    // 1. Check internal flags

    bool OK;

    OK = ProcessInternalFlags();

    // Check errors in internal flags
    if (OK == false) {
        // Error
        PrintString("Error with internal flags processing.\n");
        ItsPrintUsageFlag = true;
        return false;
    }

    // 2. Get token

    // Get command token

    RWCString cmdToken;
    bool tokenOK = GetToken(cmdToken);

    // Token error
    if (tokenOK == false) {
         // No sub command token, it is a way to print the usage
         ItsPrintUsageFlag = true;
         return false;
    }

    // 3. Find Command id

    INT32 cmdIndex = SubCmdFindCommandIndex(theSubCommandsP, cmdToken);

    // Check for error
    if (cmdIndex < 0) {
        // Error
         ItsPrintUsageFlag = true;
         return false;
    }

    // 4. Check the found command is not active
    //    Is is not member in an active group and it's group mask is not zero

    if ( ((theSubCommandsP[cmdIndex].groupMask & theActiveGroups) == 0 ) &&
          (theSubCommandsP[cmdIndex].groupMask != 0 )  ) {

        // Command is not active

        PrintString("Error: Command is currently inactive (grayed-out).\n");
        return false;
    }

    // 5. Set command id
    theSelectedId = theSubCommandsP[cmdIndex].id;

    // 6. Save append the sub command name to ItsCommandName and
    //    set the description into ItsDescription (for use by the parse parameters)

    ItsCommandName += RWCString(" ") + theSubCommandsP[cmdIndex].name;
    ItsDescription  = theSubCommandsP[cmdIndex].desc;

    // All Done, all OK

    return true;

}

// --- SubCmdFindCommandIndex -------------------------------------------------
//
// Find search the command token in the sub-commands array. Returns it index in
// the array. If not found, display the error and return -1;
//
// The search is partial match type
//

INT32 XdpParse::SubCmdFindCommandIndex(
             const XdpParse::XdpParseSubCommand *theSubCommandsP,
             RWCString                          &theCmdToken)
{

    const XdpParseSubCommand *p;

    UINT32    matchCount = 0;  // The number of entries matched the command token
    UINT32    matchIndex;      // The index of the last match commnad
	RWCString matchedCommands; // The list of the match commands name

    UINT32    cmdIndex   = 0;  // Current command index
	RWCString cmdName;         // Current command name

	
	UINT32 charIndex;          // match char index (tmp var)

    // Loop the command entries
    for( p = theSubCommandsP ; p->name != NULL ; p++) {

		// Convert the current commands name to RW string
		cmdName = p->name;

		// Check exact match

		if (theCmdToken == cmdName) {
			// Exact match,

			// Simulate signle match
			matchCount = 1;
			matchIndex = cmdIndex;

			// Stop searching
			break;
		}


        // Check match
        charIndex = cmdName.index(theCmdToken, 0, RWCString::ignoreCase);

	    if (charIndex == 0) {

            // match - increase match count, save match index, and add to the command list

            matchCount++;

            matchIndex = cmdIndex;

			matchedCommands += " " + cmdName;
        }

        // Next command
        cmdIndex++;


    } // for


    // Act on match count

    switch (matchCount) {

        case 0: // Not match - Print error message and return error

            PrintString("Error: Command \"" + theCmdToken + "\" not found.\n");

            ItsPrintUsageFlag = true;

            return (-1);

        case 1: // Single match - OK - return the command index

            return (matchIndex);

        default: // multiple matches - Print error message and return error

			if (matchCount < 10) {
				PrintString("Error: Command \"" + theCmdToken +
					"\" matched to " + XdpConv::ToStr(matchCount) + " commands: " +
					matchedCommands + "\n");
			}
			else {
				PrintString("Error: Command \"" + theCmdToken +
					"\" matched to " + XdpConv::ToStr(matchCount) + " commands.\n");
			}

            ItsPrintUsageFlag = true;

            return (-1);
    }

    // Should never be here, just in case

    return (-1);

}


// --- SubCmdsPrintUsage ------------------------------------------------------
//
// Print command usage for ParseSubCommands
//
void XdpParse::SubCmdsPrintUsage(const XdpParse::XdpParseSubCommand *theSubCommandsP,
                                 UINT32 theActiveGroups)
{

    RWCString s;

    // 1. Print Description

    PrintString("\n" + ItsDescription + "\n");


    // 2. Print syntax line

    s = "Usage: " + ItsCommandName + " [-help|-debug] <sub-commands> [specific arguments]\n";

    PrintString(s);

    // 3. Print the commands

    // Find the longest command name & command count

    INT32 maxLen =0;
    INT32 len;

	UINT32 cmdCount = 0;

    // Loop the command entries
    const XdpParseSubCommand *p;

    for( p = theSubCommandsP ; p->name != NULL ; p++) {

        // Get command name length
        len = strlen(p->name);

        // Update max length
        if (len > maxLen) {
            // Yes
            maxLen = len;
        }

		// Increase command ccount
		cmdCount++;
    }

    // 4. Print the sub commands info if we have less than 10 commands or the help flag is on

	if ((cmdCount < 10) || (ItsHelpFlag == true) ) {

	    // Loop the command entries
	    RWCString cmdName;

	    for( p = theSubCommandsP ; p->name != NULL ; p++) {

		    // add command name to line. Also add () if command is inactive

	       if ( ((p->groupMask & theActiveGroups) == 0 ) &&  (p->groupMask != 0 )  ) {

		       // Command is inactive, add command name and ( )
		
			   cmdName = "(" + RWCString(p->name) + ")";

	       }
		   else {

			   // Command is active, add command name

	           cmdName = " " + RWCString(p->name) + " ";
		}

		// Pad command name,  +2 is for ( )  and add the description

	       s = "  " + XdpConv::Pad(cmdName, -(maxLen + 2) ) +
			" - " + RWCString(p->desc) + "\n";

	       PrintString(s);
	    }


		PrintString("   * Commands surrounded by ( ) are currently inactive (grayed-out)\n");;

	} // print commands
	else {
		PrintString("   use -help to get the command list\n");
	}

}

// --- SubCmdsPrintDebug --------------------------------------------------
// Print Debug information for ParseSubCommands
void XdpParse::SubCmdsPrintDebug(
             const XdpParse::XdpParseSubCommand *theSubCommandsP,
             UINT32                              theActiveGroups,
             UINT32                              theSelectedId)
{

    RWCString s;

    // Print debug line

    s = "\n\n DebugInfo - " + ItsCommandName + " - IsOK=" + XdpConv::ToStr(ItsOK);

    s += " ActiveGroups=0x" + XdpConv::ToHexStr(theActiveGroups);

    s += " SelectedId=" + XdpConv::ToStr(theSelectedId) + "\n";

    PrintString(s);

    // Find the longest command name

    INT32 maxLen =0;
    INT32 len;

    // Loop the command entries
    const XdpParseSubCommand *p;

    for( p = theSubCommandsP ; p->name != NULL ; p++) {

        // Get command name length
        len = strlen(p->name);

        // Update max length
        if (len > maxLen) {
            // Yes
            maxLen = len;
        }
    }

    // 4. Print the sub commands debug info

    // Loop the command entries

    for( p = theSubCommandsP ; p->name != NULL ; p++) {

        s = "  " + XdpConv::Pad(p->name, -(maxLen) ) + " ID=" + XdpConv::ToStr(p->id) +
            " groupMask=0x" + XdpConv::ToHexStr(p->groupMask) + "\n";

       PrintString(s);
    }

}



// ========================================================================
// ===
// === Common functions
// ===
// ========================================================================
// --- ProcessInternalFlags ---------------------------------------------------
bool XdpParse::ProcessInternalFlags(){

    bool OK;

    // Save tokens

    RWCString savedTokenString;

    OK = GetRestOfLine(savedTokenString);

    // Empty line ?
    if (OK==false) {
        // Empty, nothing to do
        return true;
    }


    // Restore token line
    SetTokenString(savedTokenString);


    // GetToken;
    RWCString t;

    OK = GetToken(t);

    // No token ?
    if (OK==false) {
        // Empty, nothing to do
        return true;
    }

    // Check "-help"
    if (t == "-help") {
        // "-help"
		ItsHelpFlag       = true;
        return true;
    }

    // Check "-debug"
    if (t == "-debug") {
        // "-debug"
        ItsDebugFlag = true;
        return true;
    }

    // Not on internal flag, restore token line

    // Restore token line
    SetTokenString(savedTokenString);

    return true;
}

//--- SetTokenString ----------------------------------------------------------
// Init String for token processing.

void XdpParse::SetTokenString(RWCString &theString) {

    // Prepare for token - to be improved !!!!!!!!!!!!1
    ItsFirstTokenFlag=true;

    // Set the string
    ItsTokenString=theString;
}
// --- PrintString ------------------------------------------------------------
// Print a string to the command manager

void XdpParse::PrintString(RWCString theString) {

    // Print it
    ItsCmdMngrP->PrintString(theString);

}

//-------------------------------------------------------------------------
// private
//-------------------------------------------------------------------------

// Constructor & Destructor

// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

// Operators

// Conv Operators

// Data members

// Friend Declarations

