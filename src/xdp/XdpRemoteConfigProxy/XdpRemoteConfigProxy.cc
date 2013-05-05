/* XdpRemoteConfigProxy.cc - <module's short description>  */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpRemoteConfigProxy
      This class is responsible to receive XDP configuration from Acs
      as part of XDP configuration save scenario and call the XdpInSfcConfigMngr
      in order to set it to XdpGlobalConfig, XdpTraceConfig
      objects and save it to file.

AUTHOR
   Tanya Yakov, Seabridge


**************************************************************************/

#include "XdpRemoteConfigProxy.h"

#include "XdpInSfcConfigMngr.h"
#include "SerialPrint.h"


extern const MaskFlagId MASK_XDP;

XdpRemoteConfigProxy* XdpRemoteConfigProxy::TheirSelfP = 0;
XdpInSfcConfigMngr*   XdpRemoteConfigProxy::TheirConfigMngrP = 0;

//-------------------------------------------------------------------------
// public
//-------------------------------------------------------------------------

// Constructor & Destructor

// Copy constructor & Assign op

// Get methods

// Set methods


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
void XdpRemoteConfigProxy::Create(XdpInSfcConfigMngr* theConfigMngrP)
{

    RN_TRACE(MASK_XDP,"::Create - start");
    RN_ASSERT(theConfigMngrP != NULL ,"XdpInSfcConfigMngr pointer is NULL")


	if (NULL == TheirSelfP)
	{
		TheirSelfP = new XdpRemoteConfigProxy(theConfigMngrP);
		if (NULL == TheirSelfP)
		{
            //failed to allocate memory
			REC_FATAL(17521);
		}
	}
	else
	{
        //Create() was already called
		 REC_FATAL(17522);
	}
}


/**************************************************************************
*
* SaveCardGlobalConfig
*
* DESCRIPTION
*    Call XdpConfigMngr in order to save updated XDP global configuration
*    received from the LIC via Acs.
*
* SCOPE
*    static
*
* RETURNS
*    void
*
**************************************************************************/
void XdpRemoteConfigProxy::SaveCardGlobalConfig(
                                     const CardIndex  theCardIndex,
							         const IpAddress& theOutputToUdpHostIpR,
							         const UINT16     theOutputToUdpHostPort,
							         const BOOL32     theOutputToUdpEnableFlag,
							         const BOOL32     theOutputToSerialEnableFlag)
{

    RN_TRACE(MASK_XDP,"::UpdateCardGlobalConfig("<<theCardIndex);
    RN_ASSERT(theCardIndex.IsSCIndexValid(),RN_cat << "Non valid LIC index = " << theCardIndex)

    TheirConfigMngrP->SaveCardGlobalConfig(theCardIndex,
                                         theOutputToUdpHostIpR,
                                         theOutputToUdpHostPort,
                                         theOutputToUdpEnableFlag,
                                         theOutputToSerialEnableFlag);

}


/**************************************************************************
*
* SaveCardTraceConfig
*
* DESCRIPTION
*    Call XdpConfigMngr in order to save updated XDP trace configuration
*    received from the LIC via Acs.
*
* SCOPE
*    static
*
* RETURNS
*    void
*
**************************************************************************/
void XdpRemoteConfigProxy::SaveCardTraceConfig(
                                    const CardIndex  theCardIndex,
                                    const UINT8Buffer& theTraceNameR,
                                    const UINT32       theEnabledAppTags,
                                    const UINT32Array& theEnabledColorsR,
                                    const UINT32       thePrintActiveAppTags,
                                    const UINT32Array&  thePrintActiveColorsR,
                                    const UINT32        thePrintOutputChannel)
{

    RN_TRACE(MASK_XDP,"::UpdateCardTraceConfig("<<theCardIndex);
    RN_ASSERT(theCardIndex.IsSCIndexValid(),RN_cat << "Non valid LIC index = " << theCardIndex)

    //converting theTraceNameR into RWCString
    RWCString traceName = RWCString(rn_static_cast(const char *,theTraceNameR.GetData()), theTraceNameR.GetSize());


    TheirConfigMngrP->SaveCardTraceConfig(theCardIndex,
                                          traceName,
                                          theEnabledAppTags,
                                          theEnabledColorsR.GetData(),
                                          thePrintActiveAppTags,
                                          thePrintActiveColorsR.GetData(),
                                          thePrintOutputChannel);

}


// Miscellaneous Operations

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
* XdpRemoteConfigProxy
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
XdpRemoteConfigProxy::XdpRemoteConfigProxy(XdpInSfcConfigMngr* theConfigMngrP)
{
    TheirConfigMngrP = theConfigMngrP;
}

// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

// Operators

// Conv Operators

// Data members

// Friend Declarations

