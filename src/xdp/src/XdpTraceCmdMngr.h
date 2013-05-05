/* XdpTraceCmdMngr.h */

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

#ifndef XdpTraceCmdMngr_h
#define XdpTraceCmdMngr_h

#include "CmdMngr.h"

class XdpTraceCmdMngr: public CmdMngr {

public:

	void Trace();

    static CmdMngr *Instance();

//-----------------------------------------------------------------------------
protected:
//-----------------------------------------------------------------------------

  	XdpTraceCmdMngr(); // protected becauae it is single-ton

//-----------------------------------------------------------------------------
private:
//-----------------------------------------------------------------------------

	static const CommandEntry CommandTable[];

    static CmdMngr *TheirP;

// --- copy ctor, operator = --------------------------------------------------
//
// copy ctor & operator = are declared private and has no implemenation
// this procted from calling them
    
    // Copy ctor
    XdpTraceCmdMngr(const XdpTraceCmdMngr &);

    // Operator =
    XdpTraceCmdMngr & operator=( const XdpTraceCmdMngr) ;

};


#endif // XdpTraceCmdMngr_h

