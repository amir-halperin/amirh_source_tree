/* XdpMngrCmdMngr.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   This class is the command manager for all the XDP services and 
   the XDP manager.

AUTHOR
   Gaash, SB

		
*************************************************************************/

#ifndef XdpMngrCmdMngr_h
#define XdpMngrCmdMngr_h

#include "basictypes.h"
#include "CmdMngr.h"

#include "CmdMngr.h"

class XdpMngrCmdMngr: public CmdMngr {

public:

    // ctor
    XdpMngrCmdMngr();


//-----------------------------------------------------------------------------
protected:
//-----------------------------------------------------------------------------

    // Output to UDP
    void OutputToUdpCommand();
    void OutputToUdpStatus();
    void OutputToUdpCounters();

    // Output to serial
    void OutputToSerialCommand();
    void OutputToSerialStatus();
    void OutputToSerialCounters();

    // Time stamp
    void TimeStampCommand();
    void TimeStampStatus();

    //save configuration
    void SaveConfig();
    void PrintGlobals();
    void PrintFiles();
    void DeleteFiles();
    void PrintState();
//-----------------------------------------------------------------------------
private:
//-----------------------------------------------------------------------------

	static const CommandEntry CommandTable[];

// --- copy ctor, operator = --------------------------------------------------
//
// copy ctor & operator = are declared private and has no implemenation
// this procted from calling them
    
    // Copy ctor
    XdpMngrCmdMngr(const XdpMngrCmdMngr &);

    // Operator =
    XdpMngrCmdMngr & operator=( const XdpMngrCmdMngr) ;


};

#endif // XdpMngrCmdMngr_h

