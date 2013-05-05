/* XdpRemoteConfigService.cc - <module's short description>  */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpRemoteConfigService
      This class provides XDP local configuration services.
      This class is responsible to download restored configuration to
      remote XDP actual objects (XdpServices, XdpTrace), which means
      that it sends LIC XDP restored configuration from SFC to LIC using Acs.
      This class is singelton.

AUTHOR
   Tanya Yakov, Seabridge


**************************************************************************/

#include "XdpRemoteConfigService.h"

#include "XdpTrace.h"
#include "RpcTxXdpInLicConfigMngr.h"
#include "SerialPrint.h"

extern const MaskFlagId MASK_XDP;

XdpRemoteConfigService* XdpRemoteConfigService::TheirSelfP = 0;

//-------------------------------------------------------------------------
// public
//-------------------------------------------------------------------------

// Constructor & Destructor

// Copy constructor & Assign op

// Get methods

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
XdpRemoteConfigService*  XdpRemoteConfigService::GetIt()
{
   	// Check if XdpLocalConfigService instance is created.
	if (NULL == TheirSelfP)
	{
       REC_FATAL(17518);
	}

	return TheirSelfP;
}

// Set methods

// Miscellaneous Operations

/**************************************************************************
*
* Create
*
* DESCRIPTION
*    Singelton support - create itself.
*    Make sure the object is created exactly once.
*
* SCOPE
*    static
*
* RETURNS
*    void
*
**************************************************************************/
void XdpRemoteConfigService::Create()
{

	if (NULL == TheirSelfP)
	{
		TheirSelfP = new XdpRemoteConfigService();
		if (NULL == TheirSelfP)
		{
            //failed to allocate memory
			REC_FATAL(17519);
		}
	}
	else
	{
        //Create() was already called
		REC_FATAL(17520);
	}
}




/**************************************************************************
*
* DownloadGlobalConfig
*
* DESCRIPTION
*    Download (send) previously restored from file XDP package configuration
*    to the XDP in specified LIC using Acs
*
* SCOPE
*    not-static
*
* RETURNS
*    void
*
**************************************************************************/	
void XdpRemoteConfigService::DownloadGlobalConfig(
                    const CardIndex&  theCardIndexR,
                    const IpAddress&  theOutputToUdpHostIPR,
                    const UINT16      theOutputToUdpHostPort,
                    const bool        theOutputToUdpEnableFlag,
                    const bool        theOutputToSerialEnableFlag) const
{

      RN_TRACE(MASK_XDP,"::DownloadGlobalConfig("<<theCardIndexR);
      RN_ASSERT(theCardIndexR.IsSCIndexValid(),RN_cat << "Non valid LIC index = " << theCardIndexR)

     //send Rpc
     RpcTxXdpInLicConfigMngr::DownloadGlobalConfig(theCardIndexR,
                                                    theOutputToUdpHostIPR,
                                                    theOutputToUdpHostPort,
                                                    theOutputToUdpEnableFlag,
                                                    theOutputToSerialEnableFlag);
}


/**************************************************************************
*
* DownloadTraceConfig
*
* DESCRIPTION
*    Download (send) previously restored from file specified XDP trace
*    configuration to the XDP in specified LIC using Acs
*
* SCOPE
*    not- static
*
* RETURNS
*    void
*
**************************************************************************/	
void XdpRemoteConfigService::DownloadTraceConfig(
                    const CardIndex&   theCardIndexR,
                    const RWCString&   theTraceNameR,
                    const UINT32       theEnabledAppTags,
                    const UINT32*      theEnabledColorsP,
                    const UINT32       thePrintActiveAppTags,
                    const UINT32*      thePrintActiveColorsP,
                    const UINT32       thePrintOutputChannel) const
{

      RN_TRACE(MASK_XDP,"::DownloadTraceConfig("<<theCardIndexR<<", "<<theTraceNameR);
      RN_ASSERT(theCardIndexR.IsSCIndexValid(),RN_cat << "Non valid LIC index = " << theCardIndexR)

      size_t colorsArrSize = rn_static_cast(size_t, XDP_TRACE_APP_LAST);

      //send Acs
      RpcTxXdpInLicConfigMngr::DownloadTraceConfig(
                                    theCardIndexR,
                                    UINT8Buffer(rn_static_cast(UINT8*, theTraceNameR.data()), theTraceNameR.length()),
                                    theEnabledAppTags,
                                    UINT32Array(theEnabledColorsP, colorsArrSize),
                                    thePrintActiveAppTags,
                                    UINT32Array(thePrintActiveColorsP, colorsArrSize),
                                    thePrintOutputChannel
                                                    );
}

// Operators

// Conv Operators

//-------------------------------------------------------------------------
// protected
//-------------------------------------------------------------------------

// Constructor & Destructor

// Get methods

// Set methods

// Miscellaneous Operations

// Operators

// Conv Operators

//-------------------------------------------------------------------------
// private
//-------------------------------------------------------------------------

// Constructor & Destructor


/**************************************************************************
*
* XdpRemoteConfigService
*
* DESCRIPTION
*  Constructor
*
* SCOPE
*    not-static
*
* RETURNS
*    N/A
*
**************************************************************************/
XdpRemoteConfigService::XdpRemoteConfigService()
{
    RN_TRACE(MASK_XDP,"::XdpRemoteConfigService ctor begin");
}

// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

// Operators

// Conv Operators

// Data members

// Friend Declarations

