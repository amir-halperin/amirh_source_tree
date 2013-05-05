/* XdpLocalConfigService.cc - <module's short description>  */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpLocalConfigService
      This class provides XDP local configuration services.
      This class is responsible to download restored configuration to
      local XDP actual objects (XdpServices, XdpTrace), which means
      that it copies SFC XDP restored configuration
      (from XdpGlobalConfig, XdpTraceConfig) to SFC XDP actual objects
      (XdpServices, XdpTrace).
      In case of LIC it copies LIC XDP restored configuration and
      recived via Acs to LIC XDP actual objects.
      This class is singelton.

AUTHOR
   Tanya Yakov, Seabridge


**************************************************************************/

#include "XdpLocalConfigService.h"

#include "casts.h"

#include "XdpServices.h"
#include "XdpTraceContainer.h"
#include "XdpTrace.h"


extern const MaskFlagId MASK_XDP;

XdpLocalConfigService* XdpLocalConfigService::TheirSelfP = 0;

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
XdpLocalConfigService*  XdpLocalConfigService::GetIt()
{
   	// Check if XdpLocalConfigService instance is created.
	if (NULL == TheirSelfP)
	{
       REC_FATAL(17515);
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
void XdpLocalConfigService::Create(XdpTraceContainer* theContainerP)
{

	if (NULL == TheirSelfP)
	{
		TheirSelfP = new XdpLocalConfigService(theContainerP);
		if (NULL == TheirSelfP)
		{
            //failed to allocate memory
			REC_FATAL(17516);
		}
	}
	else
	{
        //Create() was already called
		REC_FATAL(17517);
	}
}


/**************************************************************************
*
* DownloadGlobalConfig
*
* DESCRIPTION
*    Download (set) previously restored from file XDP package configuration
*    to the XdpServices object that holds actual XDP package configuration.
*
* SCOPE
*    not- static
*
* RETURNS
*    void
*
**************************************************************************/	
void XdpLocalConfigService::DownloadGlobalConfig(
                    const CardIndex&  theCardIndexR,
                    const IpAddress&  theOutputToUdpHostIPR,
                    const UINT16      theOutputToUdpHostPort,
                    const bool        theOutputToUdpEnableFlag,
                    const bool        theOutputToSerialEnableFlag) const
 {

     RN_TRACE(MASK_XDP,"::DownloadGlobalConfig");

     // In case of local service card index isn't used
     // therefore it value is also CardIndex::NotValid.

     // Get XDP services
     XdpServices *xdpServicesP = XdpServices::GetIt();

     bool rc;

     //Set restored output to UDP host IP
     rc = xdpServicesP->OutputToUdpSetIP(theOutputToUdpHostIPR);
     if (false == rc) {
        REC_FATAL(17512);
     }

     //Set restored output to UDP host port
     rc = xdpServicesP->OutputToUdpSetPort(theOutputToUdpHostPort);
     if (false == rc) {
        REC_FATAL(17512);
     }

     //Set restored output to UDP enable flag
     rc = xdpServicesP->OutputToUdpEnable(theOutputToUdpEnableFlag);
     if (false == rc) {
        REC_FATAL(17512);
     }

     //Set restored output to serial enable flag
     rc = XdpServices::GetIt()->OutputToSerialEnable(theOutputToSerialEnableFlag);
     if (rc == false) {
        REC_FATAL(17512);
     }

 }



/**************************************************************************
*
* DownloadTraceConfig
*
* DESCRIPTION
*    Download (set) previously restored from file specified XDP trace
*    configuration to the XdpTrace object that holds actual
*    XDP trace configuration
*
* SCOPE
*    not- static
*
* RETURNS
*    void
*
**************************************************************************/	
void XdpLocalConfigService::DownloadTraceConfig(
                    const CardIndex&   theCardIndexR,
                    const RWCString&   theTraceNameR,
                    const UINT32       theEnabledAppTags,
                    const UINT32*      theEnabledColorsP,
                    const UINT32       thePrintActiveAppTags,
                    const UINT32*      thePrintActiveColorsP,
                    const UINT32       thePrintOutputChannel) const
{
     RN_TRACE(MASK_XDP,"::DownloadTraceConfig("<<theTraceNameR<<")");

     // In case of local service card index isn't used
     // therefore it value is also CardIndex::NotValid.

     //get XDP trace object using the container
     XdpTrace* traceP = ItsTraceContainerP->GetTrace(theTraceNameR);
     if (NULL == traceP)
     {
         REC_FATAL(17513);
     }

     //set restored XDP trace configuration to actual XdpTrace object
     if ((traceP->RestoreConfig(
                      theEnabledAppTags,
                      theEnabledColorsP,
                      thePrintActiveAppTags,
                      thePrintActiveColorsP,
                      rn_static_cast(XdpTrace::XdpTraceOutput, thePrintOutputChannel))) == false) {
         REC_FATAL(17514);
     }
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
* XdpLocalConfigService
*
* DESCRIPTION
*  Constractor
*
* SCOPE
*    not-static
*
* RETURNS
*    N/A
*
**************************************************************************/
XdpLocalConfigService::XdpLocalConfigService(XdpTraceContainer* theContainerP)
{

    RN_TRACE(MASK_XDP,"::XdpLocalConfigService ctor begin");
    RN_ASSERT ((NULL != theContainerP), "XdpTraceContainer pointer is NULL");

    ItsTraceContainerP = theContainerP;
}

// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

// Operators

// Conv Operators

// Data members

// Friend Declarations

