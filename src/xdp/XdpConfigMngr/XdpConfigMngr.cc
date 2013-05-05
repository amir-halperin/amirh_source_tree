/* XdpConfigMngr.cc - <module's short description>  */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpConfigMngr
      This class holds local XdpTrace objects container.
      It is abstract base class.
      This class is responsible to register local XdpTrace objects.

AUTHOR
   Tanya Yakov, Seabridge


**************************************************************************/

#include "XdpConfigMngr.h"

#include "XdpTraceContainer.h"
#include "XdpLocalConfigService.h"
#include "XdpConv.h"
#include "XdpTrace.h"

extern const MaskFlagId MASK_XDP;

XdpConfigMngr* XdpConfigMngr::TheirSelfP = 0;

//-------------------------------------------------------------------------
// public
//-------------------------------------------------------------------------

/**************************************************************************
*
* GetIt
*
* DESCRIPTION
*    Singelton support - returns pointer to itself.
*
* SCOPE
*    static
*
* RETURNS
*    pointer to XdpLocalConfigService
*
**************************************************************************/
XdpConfigMngr*  XdpConfigMngr::GetIt()
{
   	// Check if XdpConfigMngr derived class instance is created.
	if (NULL == TheirSelfP)
	{
        REC_FATAL(17523);
	}

	return TheirSelfP;
}

// Set methods

// Miscellaneous Operations

// Operators

// Conv Operators

//-------------------------------------------------------------------------
// protected
//-------------------------------------------------------------------------

// Constructor & Destructor

/**************************************************************************
*
* XdpConfigMngr
*
* DESCRIPTION
*    Conatractor
*
* SCOPE
*    non - static
*
* RETURNS
*    N/A
*
**************************************************************************/
XdpConfigMngr::XdpConfigMngr(): ItsTraceContainerP(NULL) {

    RN_TRACE(MASK_XDP,"::XdpConfigMngr (" + XdpConv::ToHexStr(this) + ") - ctor begin");

    //allocate XdpTraceContainer
    ItsTraceContainerP = new XdpTraceContainer();
    if (NULL == ItsTraceContainerP)
    {
        //failed to allocate memory
	     REC_FATAL(17524);
    }

    //create local config service there because both
    //XdpInSfcConfigMngr and XdpInLicConfigMngr uses it
    XdpLocalConfigService::Create(ItsTraceContainerP);
}



void XdpConfigMngr::RegisterTrace(XdpTrace *theTraceP)
{
    ItsTraceContainerP->Insert(theTraceP);
}
// Get methods

// Set methods

// Miscellaneous Operations


//-------------------------------------------------------------------------
// private
//-------------------------------------------------------------------------

