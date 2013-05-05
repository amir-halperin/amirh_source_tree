/* XdpTraceCmdMngr.cc - Command manager for XDP trace  */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpTraceCmdMngr
   
   Command manager for XDP trace

AUTHOR
   Gaash, SB


**************************************************************************/

#include "RWHdr/cstring.h"
#include "XdpTraceCmdMngr.h"
#include "Error.h"
#include "XdpTrace.h"
#include "XdpConv.h"

//#include "XdpParse.h"
//#include "XdpParseNumber.h"
//#include "XdpParseString.h"
//#include "XdpParseBool.h"
//#include "XdpParseEnum.h"

extern const MaskFlagId MASK_XDP;

#define This ((XdpTrace *)ItsObjP)


// --- static/globals ---------------------------------------------------------

CmdMngr * XdpTraceCmdMngr::TheirP = NULL;

const CommandEntry XdpTraceCmdMngr::CommandTable[] =
						 {{"trace",
								(CmdMngrP)&XdpTraceCmdMngr::Trace},
						  {"",
								NULL} };

//-------------------------------------------------------------------------
// public 
//-------------------------------------------------------------------------

// Constructor & Destructor

// --- Instance -----------------------------------------------------------
//
// standard single tone
//
CmdMngr * XdpTraceCmdMngr::Instance() 
{

    // Check not  created
    if (TheirP == NULL) {

        // Not created, create it

        TheirP = new XdpTraceCmdMngr;

    }

    // Verify object exists
    if (TheirP == NULL) {
        REC_FATAL(17500);
    }

    // return it

    return TheirP;

}

// --- Trace --------------------------------------------------------------

void XdpTraceCmdMngr::Trace(){
  
    if (This == NULL) {
        PrintString("Object has not been set\n");
        return;
    }

    // Call the XdpParse to do the work

    This->ProcessUI(this);

}

//-------------------------------------------------------------------------
// protected
//-------------------------------------------------------------------------

// --- ctor ---------------------------------------------------------------
XdpTraceCmdMngr::XdpTraceCmdMngr(): CmdMngr("Trace",  CommandTable) {

    RN_TRACE(MASK_XDP, "At ctor, this=" + XdpConv::ToHexStr(this) );

}



