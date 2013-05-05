/* XdpConfig.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpConfig
   Base abstruct class for configuration storage

AUTHOR
   Amit Rapaport, Seabridge
		
*************************************************************************/

#ifndef XdpConfig_h
#define XdpConfig_h

#include <stdio.h>

#include "basictypes.h"
#include "CardIndex.h"


//----------------------------
// Forward Declarations
//----------------------------
class XdpConfigService;

class XdpConfig
{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------
// Constructor & Destructor
XdpConfig(UINT32 theCarsIndex, XdpConfigService *theConfServiceP);

XdpConfig(){}

virtual ~XdpConfig(){}

// Miscellaneous Operations

virtual void DownloadConfig() = 0;

//-------------------------------------------------------------------------
protected:
//-------------------------------------------------------------------------
// Data members
CardIndex ItsCardIndex;
XdpConfigService *ItsConfServiceP;

};

#endif // XdpConfig_h

