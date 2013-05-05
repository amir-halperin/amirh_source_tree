/* XdpInLicConfigMngr.cc - <module's short description>  */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpInLicConfigMngr
      This class is responsible to send  XDP updated configuration to SFC
      in order to save it to file on flash, and to update XDP objects
      with XDP restored configuration received from the SFC via ACS.

AUTHOR
   Tanya Yakov, Seabridge


**************************************************************************/

#include "XdpInLicConfigMngr.h"

#include "UINT32Array.h"
#include "UINT8Buffer.h"

#include "XdpLocalConfigService.h"
#include "RpcTxXdpRemoteConfigProxy.h"
#include "XdpTrace.h"



extern const MaskFlagId MASK_XDP;

XdpInLicConfigMngr* XdpInLicConfigMngr::TheirSelfP = 0;

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
*    pointer to XdpInLicConfigMngr
*
**************************************************************************/
XdpInLicConfigMngr*  XdpInLicConfigMngr::GetIt()
{
   	// Check if XdpInLicConfigMngr instance is created.
	if (NULL == TheirSelfP)
	{
       REC_FATAL(17540);
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
void XdpInLicConfigMngr::Create()
{
    // Make sure the object is created exactly once
    //in LIC only XdpInLicConfigMngr is created
	if ((NULL == TheirSelfP) && (NULL == XdpConfigMngr::TheirSelfP))
	{
		TheirSelfP = new XdpInLicConfigMngr();
		if (NULL == TheirSelfP)
		{
            //failed to allocate memory
			REC_FATAL(17536);
		}
	}
	else
	{
        //Specific XDP manager was already created
        REC_FATAL(17537);
	}

    //update static pointer from type base
    //to support base GetIt
    XdpConfigMngr::TheirSelfP = TheirSelfP;

}

/**************************************************************************
*
* DownloadGlobalConfig
*
* DESCRIPTION
*    Download (set) previously restored from file XDP package configuration
*    to the XdpServices object that holds actual XDP package configuration
*    using XdpLocalConfigService
*
* SCOPE
*    static
*
* RETURNS
*    void
*
**************************************************************************/	
void XdpInLicConfigMngr::DownloadGlobalConfig(
                    const IpAddress&  theOutputToUdpHostIpR,
                    const UINT16      theOutputToUdpHostPort,
                    const BOOL32      theOutputToUdpEnableFlag,
                    const BOOL32      theOutputToSerialEnableFlag)
 {

     RN_TRACE(MASK_XDP,"::DownloadGlobalConfig");

     //call local service in order to download XDP configuration
     // in case of local service CardIndex is not in use
     (XdpLocalConfigService::GetIt())->DownloadGlobalConfig(
                                           CardIndex(CardIndex::NotValid),
                                           theOutputToUdpHostIpR,
                                           theOutputToUdpHostPort,
                                           theOutputToUdpEnableFlag,
                                           theOutputToSerialEnableFlag);

}


/**************************************************************************
*
* DownloadTraceConfig
*
* DESCRIPTION
*    Download (set) previously restored from file specified XDP trace
*    configuration to the XdpTrace object that holds actual
*    XDP trace configuration using XdpLocalConfigService
*
* SCOPE
*    static
*
* RETURNS
*    void
*
**************************************************************************/	
void XdpInLicConfigMngr::DownloadTraceConfig(
                    const UINT8Buffer& theTraceNameR,
                    const UINT32       theEnabledAppTags,
                    const UINT32Array& theEnabledColorsR,
                    const UINT32       thePrintActiveAppTags,
                    const UINT32Array& thePrintActiveColorsR,
                    const UINT32       thePrintOutputChannel)
{



     RN_TRACE(MASK_XDP,"::DownloadTraceConfig("<<theTraceNameR<<")");

     //converting theTraceNameR into RWCString
     RWCString traceName = RWCString(rn_static_cast(const char *,theTraceNameR.GetData()), theTraceNameR.GetSize());

     //call local service in order to download XDP configuration
     // in case of local service CardIndex is not in use
     (XdpLocalConfigService::GetIt())->DownloadTraceConfig(
                                         CardIndex(CardIndex::NotValid),
                                         traceName,
                                         theEnabledAppTags,
                                         theEnabledColorsR.GetData(),
                                         thePrintActiveAppTags,
                                         thePrintActiveColorsR.GetData(),
                                         thePrintOutputChannel);
}


/**************************************************************************
*
* SaveGlobalConfig
*
* DESCRIPTION
*  Send message to the Sfc via Acs in order to save updated
*  XDP package configuration
*
*
* SCOPE
*    non-static
*
* RETURNS
*    void
*
**************************************************************************/
void XdpInLicConfigMngr::SaveGlobalConfig(
                    const IpAddress&  theOutputToUdpHostIpR,
                    const UINT16      theOutputToUdpHostPort,
                    const bool        theOutputToUdpEnableFlag,
                    const bool        theOutputToSerialEnableFlag)
{
     RN_TRACE(MASK_XDP,"::SaveGlobalConfig");

    //Send Acs
    RpcTxXdpRemoteConfigProxy::SaveCardGlobalConfig(
                                     theOutputToUdpHostIpR,
							         theOutputToUdpHostPort,
							         theOutputToUdpEnableFlag,
							         theOutputToSerialEnableFlag);

}

/**************************************************************************
*
* SaveTraceConfig
*
* DESCRIPTION
*  Send message to the Sfc via Acs in order to save updated
*  XDP trace configuration
*
* SCOPE
*    non-static
*
* RETURNS
*    void
*
**************************************************************************/
void XdpInLicConfigMngr::SaveTraceConfig(
                    const RWCString&   theTraceNameR,
                    const UINT32       theEnabledAppTags,
                    UINT32* const      theEnabledColorsP,
                    const UINT32       thePrintActiveAppTags,
                    UINT32* const      thePrintActiveColorsP,
                    const UINT32       thePrintOutputChannel)
{
    RN_TRACE(MASK_XDP,"::SaveTraceConfig("<<theTraceNameR<<")");

    size_t colorsArrSize = rn_static_cast(size_t, XDP_TRACE_APP_LAST);

    //Send Acs
    RpcTxXdpRemoteConfigProxy::SaveCardTraceConfig(
                                   UINT8Buffer(rn_static_cast(UINT8*, theTraceNameR.data()),
                                               theTraceNameR.length()),
                                   theEnabledAppTags,
                                   UINT32Array(theEnabledColorsP, colorsArrSize),
                                   thePrintActiveAppTags,
                                   UINT32Array(thePrintActiveColorsP, colorsArrSize),
                                   thePrintOutputChannel);
}

//-------------------------------------------------------------------------
// private
//-------------------------------------------------------------------------

// Constructor & Destructor
XdpInLicConfigMngr::XdpInLicConfigMngr() {
    RN_TRACE(MASK_XDP,"::Ctor");

}


