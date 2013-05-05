/* XdpMngrCmdMngr.cc */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpMngrCmdMngr
   
   This class is the command manager for all the XDP services and 
   the XDP manager.

AUTHOR
   Gaash, SB


**************************************************************************/

#include "XdpMngrCmdMngr.h"

#include "XdpParse.h"
#include "XdpParseNumber.h"
#include "XdpParseString.h"
#include "XdpParseBool.h"
#include "XdpParseEnum.h"
#include "XdpParseIp.h"
#include "XdpConv.h"

#include "Error.h"
#include "MaskFlagId.h"

#include "XdpMngr.h"
#include "XdpServices.h"
#include "SerialPrint.h"

#ifdef RN_SFC_PPE
#include "XdpInSfcConfigMngr.h"
#endif



extern const MaskFlagId MASK_XDP;

#define This         (XdpMngr::GetIt())
#define ThisServices (XdpServices::GetIt() )

// --- static/globals ---------------------------------------------------------

const CommandEntry XdpMngrCmdMngr::CommandTable[] = {
    {"OutputToUdp",      (CmdMngrP) &OutputToUdpCommand    },
    {"UDP",              (CmdMngrP) &OutputToUdpCommand    },
    {"OutputToSerial",   (CmdMngrP) &OutputToSerialCommand },
    {"Serial",           (CmdMngrP) &OutputToSerialCommand },
    {"TimeStamp",        (CmdMngrP) &TimeStampCommand },
    {"Save",             (CmdMngrP) &SaveConfig },
    {"PrintGlobals",     (CmdMngrP) &PrintGlobals },
    {"PrintFiles",       (CmdMngrP) &PrintFiles },
    {"DeleteFiles",       (CmdMngrP) &DeleteFiles },
    {"PrintState",       (CmdMngrP) &PrintState },
    
    {"",                 NULL} };  // Last

//-------------------------------------------------------------------------
// public 
//-------------------------------------------------------------------------

// --- ctor ---------------------------------------------------------------
XdpMngrCmdMngr::XdpMngrCmdMngr(): CmdMngr("XdpMngr",  CommandTable) {

    RN_TRACE(MASK_XDP, "XdpMngrCmdMngr (" + XdpConv::ToHexStr(this) + ") - ctor" );

}

// ============================================================================
// ====
// ==== Output to UDP service
// ====
// ============================================================================

// --- OutputToUdpCommand -----------------------------------------------------
//
// Handles all the output to UDP sub-commands
//

void XdpMngrCmdMngr::OutputToUdpCommand() {


    // sum commands numbres
    const UINT32 COMMAND_IP       = 0;
    const UINT32 COMMAND_PORT     = 1;
    const UINT32 COMMAND_ENABLE   = 2;
    const UINT32 COMMAND_DISABLE  = 3;
    const UINT32 COMMAND_STATUS   = 4;
    const UINT32 COMMAND_COUNTERS = 5;
    const UINT32 COMMAND_TEST     = 6;

    // sub-ommands table
    static const XdpParse::XdpParseSubCommand  subCommands[] = {

      { "IP",       "Set destination IP address",      COMMAND_IP,       0 },
      { "Port",     "Set destination port number",     COMMAND_PORT,     0 },
      { "Enable",   "Enable output to UDP",            COMMAND_ENABLE,   0 },
      { "Disable",  "Disable output to UDP",           COMMAND_DISABLE,  0 },
      { "Status",   "Show output to UDP status",       COMMAND_STATUS,   0 },
      { "Countres", "Show output to UDP counters",     COMMAND_COUNTERS, 0 },
      { "Test",     "Send five test lines via UDP",   COMMAND_TEST,     0 },
      { NULL, NULL,0,0} } ; // Last

    
    // Create a parser

    XdpParse parser(this);

    // Get the sub commands
    UINT32 cmdNumber;

    bool rc;
    rc = parser.ParseSubCommands("OutputToUdp","Manage OutputToUdp service",0, 
                                            subCommands, 0, cmdNumber);

    // Check error
    if (rc == false) {
        return;
    }

    // Clear status string

    ThisServices->ClearStatusStrings();

    // Execute the selected sub command

    switch (cmdNumber) {

        // IP
        case COMMAND_IP:  {
            
            IpAddress tmpIP;

            XdpParseIp parseIP (tmpIP,   "IP", "Destination IP address", 0 );

            rc = parser.ParseParameters("","",0, &parseIP);

            // Check parameters error
            if (rc == false) {
                return ;
            }
 
            // Do it

            rc = ThisServices->OutputToUdpSetIP(tmpIP);

            // No need to check return code

            break;
        }

        // Port
        case COMMAND_PORT:  {
            
            UINT16 port;

            XdpParseNumber parsePort(port,"Port", "Destination port number", 0 , 5000,7000 );

            rc = parser.ParseParameters( "" , "" , 0 , &parsePort );

            // Check parameters error
            if (rc == false) {
                return ;
            }
 
            // Do it

            rc = ThisServices->OutputToUdpSetPort(port);

            // No need to check return code

            break;
        }

        // Enable
        case COMMAND_ENABLE:  {
            
            // Do it

            rc = ThisServices->OutputToUdpEnable(true);

            // No need to check return code

            break;
        }


        // Disable
        case COMMAND_DISABLE:  {
            
            // Do it

            rc = ThisServices->OutputToUdpEnable(false);

            // No need to check return code

            break;
        }

        // Status
        case COMMAND_STATUS:  {
            
            // Do it

            OutputToUdpStatus();
            break;
        }

        // Counters
        case COMMAND_COUNTERS:  {
            
            // Do it

            OutputToUdpCounters();
            break;
        }


        // Test
        case COMMAND_TEST:  {
            
            UINT32 i;
            RWCString s;

            // Send five test lines
        
            for (i = 0 ; i < 5 ; i++) {

                s = "Output to UDP from command manager test string number " 
                        + XdpConv::ToStr(i+1) + "\n" ;
                ThisServices->OutputToUdp(s);
            }

            break;
        }

        default: // Unknowen commnad number
            PrintString("Error: Invalid command number " + XdpConv::ToStr(cmdNumber) + "\n");
            return; 
    }


    // At this point the command has been executed 

    // Get and print status message
    
    RWCString statusStr = ThisServices->GetStatusStrings();
    PrintString(statusStr);

    // All done

}

// --- OutputToUdpStatus ------------------------------------------------------
//
// Show output to UDP status
//
// An helper function to OutputToUdpCommand
//

void XdpMngrCmdMngr::OutputToUdpStatus() {

    // Short cut to XDP services
    XdpServices *srv = ThisServices;

    // Print it

    PrintString( "IP address: " + srv->ItsOutputToUdpHostIP.ToStr() + 
                 " Port: " + XdpConv::ToStr(srv->ItsOutputToUdpHostPort) + 
                 "\n");

    PrintString( "Start flag: " + XdpConv::ToStr(srv->ItsOutputToUdpStartFlag) + 
                 " Enable flag: " + XdpConv::ToStr(srv->ItsOutputToUdpEnableFlag) + 
                 "\n");

    PrintString( "Task priority: " + XdpConv::ToStr(srv->ItsOutoutToUdpTaskPriority) + 
                 " Queue size: " + XdpConv::ToStr(srv->ItsOutputToUdpQueueSize) + 
                 " Max message size: " + XdpConv::ToStr(srv->ItsOutputToUdpMaxMsgSize) + 
                 "\n");

    PrintString( "Socket number: " + XdpConv::ToStr((UINT32)(srv->ItsOutputToUdpSocket)) + 
                "\n");    
}

// --- OutputToUdpCounters ----------------------------------------------------
//
// Show output to UDP counters
//
// An helper function to OutputToUdpCommand
//

void XdpMngrCmdMngr::OutputToUdpCounters() {

    // Description width
    INT32 width = -33;

    // Counter width
    UINT32 pad = 6;

    // Short cut to XDP services
    XdpServices *srv = ThisServices;

    // Print it

    PrintString("\n");

    PrintString(XdpConv::Pad("Send to queue",width) + 
                XdpConv::ToStr(srv->ItsOutputToUdpQueueSendCounter,pad) + "\n");

    PrintString(XdpConv::Pad("Receive from queue",width) + 
                XdpConv::ToStr(srv->ItsOutputToUdpQueueRcvCounter,pad) + "\n");

    PrintString(XdpConv::Pad("Too long messages",width) + 
                XdpConv::ToStr(srv->ItsOutputToUdpTooLongCounter,pad) + "\n");

    PrintString("\n");

    PrintString(XdpConv::Pad("Discard due to disabled state",width) + 
                XdpConv::ToStr(srv->ItsOutputToUdpDiscardDisableCounter,pad) + "\n");

    PrintString(XdpConv::Pad("Discard due to not started",width) + 
                XdpConv::ToStr(srv->ItsOutputToUdpDiscardNoStartCounter,pad) + "\n");

    PrintString(XdpConv::Pad("Discard due to queue errors",width) + 
                XdpConv::ToStr(srv->ItsOutputToUdpDiscardQueueErrorCounter,pad) + "\n");

    PrintString(XdpConv::Pad("Discard due to no free memory",width) + 
                XdpConv::ToStr(srv->ItsOutputToUdpDiscardNoMemCounter,pad) + "\n");

    PrintString(XdpConv::Pad("Discard due to UDP socket error",width) + 
                XdpConv::ToStr(srv->ItsOutputToUdpDiscardSocketErrorCounter,pad) + "\n");

    PrintString("\n");

}


// ============================================================================
// ====
// ==== Output to serial service
// ====
// ============================================================================

// --- OutputToSerialCommand -----------------------------------------------------
//
// Handles all the output to serial sub-commands
//

void XdpMngrCmdMngr::OutputToSerialCommand() {


    // sum commands numbres
    const UINT32 COMMAND_ENABLE   = 0;
    const UINT32 COMMAND_DISABLE  = 1;
    const UINT32 COMMAND_STATUS   = 2;
    const UINT32 COMMAND_COUNTERS = 3;
    const UINT32 COMMAND_TEST     = 4;

    // sub-ommands table
    static const XdpParse::XdpParseSubCommand  subCommands[] = {

      { "Enable",   "Enable output to UDP",            COMMAND_ENABLE,   0 },
      { "Disable",  "Disable output to UDP",           COMMAND_DISABLE,  0 },
      { "Status",   "Show output to UDP status",       COMMAND_STATUS,   0 },
      { "Countres", "Show output to UDP counters",     COMMAND_COUNTERS, 0 },
      { "Test",     "Send five test lines via UDP",   COMMAND_TEST,     0 },
      { NULL, NULL,0,0} } ; // Last

    
    // Create a parser

    XdpParse parser(this);

    // Get the sub commands
    UINT32 cmdNumber;

    bool rc;
    rc = parser.ParseSubCommands("OutputToSerial","Manage OutputToSerial service",0, 
                                            subCommands, 0, cmdNumber);

    // Check error
    if (rc == false) {
        return;
    }

    // Clear status string

    ThisServices->ClearStatusStrings();

    // Execute the selected sub command

    switch (cmdNumber) {

        // Enable
        case COMMAND_ENABLE:  {
            
            // Do it

            rc = ThisServices->OutputToSerialEnable(true);

            // No need to check return code

            break;
        }


        // Disable
        case COMMAND_DISABLE:  {
            
            // Do it

            rc = ThisServices->OutputToSerialEnable(false);

            // No need to check return code

            break;
        }

        // Status
        case COMMAND_STATUS:  {
            
            // Do it

            OutputToSerialStatus();
            break;
        }

        // Counters
        case COMMAND_COUNTERS:  {
            
            // Do it

            OutputToSerialCounters();
            break;
        }


        // Test
        case COMMAND_TEST:  {
            
            UINT32 i;
            RWCString s;

            // Send five test lines
        
            for (i = 0 ; i < 5 ; i++) {

                s = "Output to serial from command manager test string number " 
                        + XdpConv::ToStr(i+1) + "\n" ;
                ThisServices->OutputToSerial(s);
            }

            break;
        }

        default: // Unknowen commnad number
            PrintString("Error: Invalid command number " + XdpConv::ToStr(cmdNumber) + "\n");
            return; 
    }


    // At this point the command has been executed 

    // Get and print status message
    
    RWCString statusStr = ThisServices->GetStatusStrings();
    PrintString(statusStr);

    // All done

}

// --- OutputToSerialStatus ------------------------------------------------------
//
// Show output to serial status
//
// An helper function to OutputToSerialCommand
//

void XdpMngrCmdMngr::OutputToSerialStatus() {

    // Short cut to XDP services
    XdpServices *srv = ThisServices;

    // Print it

    PrintString( "Start flag: " + XdpConv::ToStr(srv->ItsOutputToSerialStartFlag) + 
                 " Enable flag: " + XdpConv::ToStr(srv->ItsOutputToSerialEnableFlag) + 
                 "\n");

    PrintString( "Task priority: " + XdpConv::ToStr(srv->ItsOutoutToSerialTaskPriority) + 
                 " Queue size: " + XdpConv::ToStr(srv->ItsOutputToSerialQueueSize) + 
                 " Max message size: " + XdpConv::ToStr(srv->ItsOutputToSerialMaxMsgSize) + 
                 "\n");

}

// --- OutputToSerialCounters ----------------------------------------------------
//
// Show output to UDP counters
//
// An helper function to OutputToSerialCommand
//

void XdpMngrCmdMngr::OutputToSerialCounters() {

    // Description width
    INT32 width = -33;

    // Counter width
    UINT32 pad = 6;

    // Short cut to XDP services
    XdpServices *srv = ThisServices;

    // Print it

    PrintString("\n");

    PrintString(XdpConv::Pad("Send to queue",width) + 
                XdpConv::ToStr(srv->ItsOutputToSerialQueueSendCounter,pad) + "\n");

    PrintString(XdpConv::Pad("Receive from queue",width) + 
                XdpConv::ToStr(srv->ItsOutputToSerialQueueRcvCounter,pad) + "\n");

    PrintString(XdpConv::Pad("Too long messages",width) + 
                XdpConv::ToStr(srv->ItsOutputToSerialTooLongCounter,pad) + "\n");

    PrintString("\n");

    PrintString(XdpConv::Pad("Discard due to disabled state",width) + 
                XdpConv::ToStr(srv->ItsOutputToSerialDiscardDisableCounter,pad) + "\n");

    PrintString(XdpConv::Pad("Discard due to not started",width) + 
                XdpConv::ToStr(srv->ItsOutputToSerialDiscardNoStartCounter,pad) + "\n");

    PrintString(XdpConv::Pad("Discard due to queue errors",width) + 
                XdpConv::ToStr(srv->ItsOutputToSerialDiscardQueueErrorCounter,pad) + "\n");

    PrintString(XdpConv::Pad("Discard due to no free memory",width) + 
                XdpConv::ToStr(srv->ItsOutputToSerialDiscardNoMemCounter,pad) + "\n");

    PrintString("\n");

}



// ============================================================================
// ====
// ==== Time stamp service
// ====
// ============================================================================

// --- TimeStampCommand -------------------------------------------------------
//
// Handles all the time stamp commands
//

void XdpMngrCmdMngr::TimeStampCommand() {


    // sub commands numbres
    const UINT32 COMMAND_STATUS   = 0;

    // sub-ommands table
    static const XdpParse::XdpParseSubCommand  subCommands[] = {

      { "Status",   "Show time stamp status",       COMMAND_STATUS,   0 },
      { NULL, NULL,0,0} } ; // Last
    
    // Create a parser

    XdpParse parser(this);

    // Get the sub commands
    UINT32 cmdNumber;

    bool rc;
    rc = parser.ParseSubCommands("TimeStamp","Manage TimeStamp service",0, 
                                            subCommands, 0, cmdNumber);

    // Check error
    if (rc == false) {
        return;
    }

    // Clear status string

    ThisServices->ClearStatusStrings();

    // Execute the selected sub command

    switch (cmdNumber) {

        // Status
        case COMMAND_STATUS:  {
            
            // Do it

            TimeStampStatus();
            break;
        }

        default: // Unknowen commnad number
            PrintString("Error: Invalid command number " + XdpConv::ToStr(cmdNumber) + "\n");
            return; 
    }


    // At this point the command has been executed 

    // Get and print status message
    
    RWCString statusStr = ThisServices->GetStatusStrings();
    PrintString(statusStr);

    // All done

}

// --- TimeStampStatus ------------------------------------------------------
//
// Show timeStamp status
//
// An helper function to OutputToSerialCommand
//

void XdpMngrCmdMngr::TimeStampStatus() {

    // Short cut to XDP services
    XdpServices *srv = ThisServices;

    // Print it

    PrintString( "Time stamp counter frequency: " + XdpConv::ToUnitsStr(srv->ItsTimeStampFrequency) + 
                 "Hz\n");

}




void XdpMngrCmdMngr::PrintState()
{
    XdpServices *srv = ThisServices;
    srv->PrintParams();
}


//--- SaveConfig --------------------------------------------------------------
//
// Save global XDP configuration
//
//

void XdpMngrCmdMngr::SaveConfig()
{
    XdpServices *srv = ThisServices;
    srv->SaveConfig();
}


//--- PrintGlobals --------------------------------------------------------------
//
// Prints all parameters in all XdpGlobalConfig - used for tests
//
//


void XdpMngrCmdMngr::PrintGlobals()
{
    #ifdef RN_SFC_PPE
    XdpInSfcConfigMngr::GetIt()->PrintGlobals();
    #endif
    #ifndef RN_SFC_PPE
    printf("Please type this command only in XdpMngr on SFC\n");
    #endif

}


//--- PrintFiles --------------------------------------------------------------
//
// Prints all the configuration files - used for tests
//
//

void XdpMngrCmdMngr::PrintFiles()
{
    #ifdef RN_SFC_PPE
    XdpInSfcConfigMngr::GetIt()->PrintFiles();
    #endif
    #ifndef RN_SFC_PPE
    printf("Please type this command only in XdpMngr on SFC\n");
    #endif
}


//--- PrintFiles --------------------------------------------------------------
//
// Delete all the Xdp configuration files - used for tests
//
//

void XdpMngrCmdMngr::DeleteFiles()
{
    #ifdef RN_SFC_PPE
    XdpInSfcConfigMngr::GetIt()->DeleteFiles();
    #endif
    #ifndef RN_SFC_PPE
    printf("Please type this command only in XdpMngr on SFC\n");
    #endif
}


