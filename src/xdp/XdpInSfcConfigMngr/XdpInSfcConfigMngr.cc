/* XdpInSfcConfigMngr.cc - <module's short description>  */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpInSfcConfigMngr
     This class manages configuration saving and restoration process.
     It holds SFC and LICs XDP package and trace configuration using
     XdpGlobalConfig objects.

AUTHOR
   Tanya Yakov, Seabridge


**************************************************************************/

#include "XdpInSfcConfigMngr.h"
#include "XdpTrace.h"

#include "MCPTasks.h"
#include "MCPQueues.h"

#include "XdpGlobalConfig.h"
#include "XdpTraceConfig.h"
#include "XdpRemoteConfigService.h"
#include "XdpLocalConfigService.h"
#include "XdpRemoteConfigProxy.h"

extern const MaskFlagId MASK_XDP;

XdpInSfcConfigMngr* XdpInSfcConfigMngr::TheirSelfP = 0;

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
*    pointer to XdpInSfcConfigMngr
*
**************************************************************************/
XdpInSfcConfigMngr*  XdpInSfcConfigMngr::GetIt()
{
   	// Check if XdpInSfcConfigMngr instance is created.
	if (NULL == TheirSelfP)
	{
       REC_FATAL(17525);
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
void XdpInSfcConfigMngr::Create()
{

    // Make sure the object is created exactly once
    //in SFC only XdpInSfcConfigMngr is created
	if ((NULL == TheirSelfP) && (NULL == XdpConfigMngr::TheirSelfP))
	{
		TheirSelfP = new XdpInSfcConfigMngr();
		if (NULL == TheirSelfP)
		{
            //failed to allocate memory
			 REC_FATAL(17526);
		}
	}
	else
	{
        //Specific XDP manager was already created
        REC_FATAL(17527);
	}

    //update static pointer from type base
    //to support base GetIt
    XdpConfigMngr::TheirSelfP = TheirSelfP;

}


/**************************************************************************
*
* RestoreConfig
*
* DESCRIPTION
*  For each card (SFC, LICs) restore it XDP configuration from file
*  and save it to storage objects (XdpGlobalConfig, XdpTraceConfig)
*
* SCOPE
*    non-static
*
* RETURNS
*    void
*
**************************************************************************/
void XdpInSfcConfigMngr::RestoreConfig()
{
     RN_TRACE(MASK_XDP,"::RestoreConfig - begin");
     for (UINT32 i = CardIndex::FirstCardIndex; i <= CardIndex::FirstSFCCardIndex; i++)
     {
        ItsConfigArr[i]->RestoreConfig();
     }

     RN_TRACE(MASK_XDP,"::RestoreConfig - end");
}

/**************************************************************************
*
* DownloadConfig
*
* DESCRIPTION
*  Download specified card (SFC or LIC) XDP configuration from storage
*  (XdpGlobalConfig, XdpTraceConfig) objects to actual XDP objects
*  (XdpServices, XdpTrace). In case of LIC use Acs.
*
* SCOPE
*    non-static
*
* RETURNS
*    void
*
**************************************************************************/
void XdpInSfcConfigMngr::DownloadConfig(const CardIndex&  theCardIndexR)
{
      RN_TRACE(MASK_XDP,"::DownloadConfig of card " << theCardIndexR);
      ItsConfigArr[theCardIndexR.AsUINT32()]->DownloadConfig();
}



/**************************************************************************
*
* SaveConfigStart
*
* DESCRIPTION
*     Create XDP save configuration queue
*     Create and start save configuration task
*
* SCOPE
*    non-static
*
* RETURNS
*    void
*
**************************************************************************/

void XdpInSfcConfigMngr::SaveConfigStart()
{

    RN_TRACE(MASK_XDP,"::SaveConfigStart - begin");
    RN_ASSERT(this, "XdpInSfcConfigMngr object isn't created");

    BOOL retVal;


    // Create the save configuration queue
    retVal = ItsSaveConfigQueue.Create("XDPQ",Q_CREATE_FLAGS, MCPQueues::XdpSaveConfigQueueSize);
    if (0 != retVal) {
        //failed to create queue
        REC_FATAL_1P(17528, retVal);
    }

    // Create the task
    retVal = ItsSaveConfigTask.Create("XDPT",MCPTasks::XdpSaveConfigTaskPriority);
    if (0 != retVal) {
        //failed to create task
        REC_FATAL_1P(17529, retVal);
    }

    // Run the task
    retVal = ItsSaveConfigTask.Start(TASK_FUNCT(XdpInSfcConfigMngr::SaveConfigTaskHandler),NO_ARGS);
    if (retVal != 0) {
        //failed to start task
        REC_FATAL_1P(17530, retVal);
    }

    RN_TRACE(MASK_XDP,"::SaveConfigStart - end");

}

/**************************************************************************
*
* SaveGlobalConfig
*
* DESCRIPTION
*     Save XDP global configuration request (called by XdpServices)
*
* SCOPE
*    non-static
*
* RETURNS
*    void
*
**************************************************************************/
void XdpInSfcConfigMngr::SaveGlobalConfig(
                    const IpAddress&  theOutputToUdpHostIpR,
                    const UINT16      theOutputToUdpHostPort,
                    const bool        theOutputToUdpEnableFlag,
                    const bool        theOutputToSerialEnableFlag)
{
     RN_TRACE(MASK_XDP,"::SaveGlobalConfig");


    //call to private method
    SaveCardGlobalConfig(CardIndex::FirstSFCCardIndex, //sfc card index???
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
*   Save XDP trace configuration request (called by XdpTrace)
*
* SCOPE
*    non-static
*
* RETURNS
*    void
*
**************************************************************************/
void XdpInSfcConfigMngr::SaveTraceConfig(
                    const RWCString&   theTraceNameR,
                    const UINT32       theEnabledAppTags,
                    UINT32* const     theEnabledColorsP,
                    const UINT32       thePrintActiveAppTags,
                    UINT32* const     thePrintActiveColorsP,
                    const UINT32       thePrintOutputChannel)
{
    RN_TRACE(MASK_XDP,"::SaveTraceConfig("<<theTraceNameR<<")");
    UINT32 appTag;
    SaveCardTraceConfig(CardIndex::FirstSFCCardIndex,
                        theTraceNameR,
                        theEnabledAppTags,
                        theEnabledColorsP,
                        thePrintActiveAppTags,
                        thePrintActiveColorsP,
                        thePrintOutputChannel);
}

// Operators

// Conv Operators

//-------------------------------------------------------------------------
// private
//-------------------------------------------------------------------------

// Constructor & Destructor

/**************************************************************************
*
* XdpInSfcConfigMngr
*
* DESCRIPTION
*   Constractor
*   For each card (SFC, LICs) allocates XdpGlobalConfig object with
*   suitable service object (local in case of SFC, remote in case of LIC)
*
*
* SCOPE
*    non-static
*
* RETURNS
*    N/A
*
**************************************************************************/
XdpInSfcConfigMngr::XdpInSfcConfigMngr()
{

    //create and get XdpRemoteConfigService object
    XdpRemoteConfigService::Create();
    XdpRemoteConfigService* remoteServiceP = XdpRemoteConfigService::GetIt();


    //for each LIC create XdpGlobalConfig object
    for (UINT32 i = CardIndex::FirstCardIndex; i <= CardIndex::LastIOCardIndex; i++)
    {
       ItsConfigArr[i] = new XdpGlobalConfig(i, remoteServiceP);
       if (NULL == ItsConfigArr[i])
       {
           //failed to allocate XdpGlobalConfig object
           REC_FATAL_1P(17531,i);
       }
    }

    //get XdpLocalConfigService object (created in XdpConfigMngr )
    XdpLocalConfigService* localServiceP = XdpLocalConfigService::GetIt();

    //create XdpGlobalConfig object for SFC
    ItsConfigArr[CardIndex::FirstSFCCardIndex] = new XdpGlobalConfig(CardIndex::FirstSFCCardIndex, localServiceP);
    if (NULL == ItsConfigArr[CardIndex::FirstSFCCardIndex])
    {
        //failed to allocate XdpGlobalConfig object
        REC_FATAL_1P(17531,CardIndex::FirstSFCCardIndex);
    }

    //create remote configuration proxy object
    XdpRemoteConfigProxy::Create(this);
}

// Miscellaneous Operations

/**************************************************************************
*
* SaveConfigTaskHandler
*
* DESCRIPTION
*    This method calls the object task body method
*
* SCOPE
*    static
*
* RETURNS
*    void
*
**************************************************************************/
void XdpInSfcConfigMngr::SaveConfigTaskHandler()
{
    RN_TRACE(MASK_XDP,"::SaveConfigTaskHandler - begin");
    RN_ASSERT(NULL != TheirSelfP, "XdpInSfcConfigMngr isn't created");

    TheirSelfP->SaveConfigTaskBody();
}


/**************************************************************************
*
* SaveConfigTaskHandler
*
* DESCRIPTION
*    Implements XDP configuration task body.
*    Wait on its queue for XDP global or trace configuration save message.
*    After message is received call received storage object
*    (XdpGlobalConfig, XdpTraceConfig) to save its configuration to file.
*
* SCOPE
*    non-static
*
* RETURNS
*    void
*
**************************************************************************/
void XdpInSfcConfigMngr::SaveConfigTaskBody()
{

  RN_TRACE(MASK_XDP,"::SaveConfigTaskBody()");

  UINT32 retVal = 0;
  UINT32 msg[4];

  while(true)
  {

    retVal =  ItsSaveConfigQueue.Receive(msg);

    if (0 != retVal)
    {
      // failed to receive message from queue
      REC_FATAL_1P(17532,retVal)
    }

    switch (rn_static_cast(XdpInSfcConfigMngr::XdpOperationType, msg[0]))
    {
        case XdpInSfcConfigMngr::SAVE_XDP_GLOBAL_CONFIG:
            RN_TRACE(MASK_XDP,"::Task - save card " << msg[1] << " global config");
            ItsConfigArr[msg[1]]->SaveConfig();
            //((XdpGlobalConfig*)msg[2])->SaveConfig();
            break;
        case XdpInSfcConfigMngr::SAVE_XDP_TRACE_CONFIG:
            RN_TRACE(MASK_XDP,"::Task - save card " << msg[1] << " trace config");
            ItsConfigArr[msg[1]]->SaveConfig();
            //((XdpTraceConfig*)msg[2])->SaveConfig();
            break;
        default:
            //undefined XDP operation
            REC_WARNING_1P(17533,msg[0]);
            break;
    }

  } //end of while(true)

}

/**************************************************************************
*
* SaveCardGlobalConfig
*
* DESCRIPTION
*    Save XDP global configuration to XdpGlobalConfig object of the specified
*    card (SFC or LIC) and send message to the "XDPT" task in order to save
*    the updated configuration to the file
*
* SCOPE
*    non-static
*
* RETURNS
*    void
*
**************************************************************************/
void XdpInSfcConfigMngr::SaveCardGlobalConfig(
                    const CardIndex&  theCardIndexR,
                    const IpAddress&  theOutputToUdpHostIpR,
                    const UINT16      theOutputToUdpHostPort,
                    const bool        theOutputToUdpEnableFlag,
                    const bool        theOutputToSerialEnableFlag)
{
     RN_TRACE(MASK_XDP,"::SaveGlobalConfig");

     //get XdpConfig object according to the specified card index
     XdpGlobalConfig* xdpGlobalConfigP = ItsConfigArr[theCardIndexR.AsUINT32()];

     // and update its parameters
     xdpGlobalConfigP->UpdateConfig(theOutputToUdpHostIpR,
                                    theOutputToUdpHostPort,
                                    theOutputToUdpEnableFlag,
                                    theOutputToSerialEnableFlag);

    //prepare and send save XDP configuration to file message
     UINT32 message[4];
     message[0] = XdpInSfcConfigMngr::SAVE_XDP_GLOBAL_CONFIG;
     message[1] = theCardIndexR.AsUINT32();
     message[2] = (UINT32)xdpGlobalConfigP;
     message[3] = 0; //not used
	 UINT32 retVal = ItsSaveConfigQueue.Send(message);
     if (retVal != 0)
     {
         //failed to send message to ItsSaveConfigQueue
         REC_FATAL_1P(17534, retVal)
     }

}


/**************************************************************************
*
* SaveCardTraceConfig
*
* DESCRIPTION
*    Save XDP trace configuration to XdpTraceConfig object of the specified
*    card (SFC or LIC) and send message to the "XDPT" task in order to save
*    the updated configuration to the file
*
* SCOPE
*    non-static
*
* RETURNS
*    void
*
**************************************************************************/
void XdpInSfcConfigMngr::SaveCardTraceConfig(
                    const CardIndex&   theCardIndexR,
                    const RWCString&   theTraceNameR,
                    const UINT32       theEnabledAppTags,
                    UINT32* const      theEnabledColorsP,
                    const UINT32       thePrintActiveAppTags,
                    UINT32* const      thePrintActiveColorsP,
                    const UINT32       thePrintOutputChannel)
{
    RN_TRACE(MASK_XDP,"::SaveTraceConfig("<<theTraceNameR<<")");

    UINT32 appTag;
    //get XdpConfig object according to the specified card index
    // and update its parameters
     XdpGlobalConfig* xdpGlobalConfigP = ItsConfigArr[theCardIndexR.AsUINT32()];

     XdpTraceConfig* traceConfigP = xdpGlobalConfigP->GetTraceConfig(theTraceNameR);

     if (NULL == traceConfigP)
     {
        // Trace Config does not exist yet
        traceConfigP = xdpGlobalConfigP->NewTraceConfig(theTraceNameR);
     }

     //Update XdpTraceConfig parameters
     traceConfigP->UpdateConfig(theEnabledAppTags,
                                theEnabledColorsP,
                                thePrintActiveAppTags,
                                thePrintActiveColorsP,
                                thePrintOutputChannel);

     //prepare and send save XDP configuration to file message
     UINT32 message[4];
     message[0] = XdpInSfcConfigMngr::SAVE_XDP_TRACE_CONFIG;
     message[1] = theCardIndexR.AsUINT32();
     message[2] = (UINT32)traceConfigP;
     message[3] = 0; //not used
	 UINT32 retVal = ItsSaveConfigQueue.Send(message);
     if (retVal != 0)
     {
         //failed to send message to ItsSaveConfigQueue
         REC_FATAL_1P(17534, retVal)
     }
}


//Print all XdpConfiguration files, Used for tests
void XdpInSfcConfigMngr::PrintFiles()
{
    for (UINT32 i = CardIndex::FirstCardIndex; i <= CardIndex::FirstSFCCardIndex; i++)
    {
        ItsConfigArr[i]->PrintFile();
    }
}


//Print all XdpGlobalConfig objects,  Used for tests
void XdpInSfcConfigMngr::PrintGlobals()
{
    for (UINT32 i = CardIndex::FirstCardIndex; i <= CardIndex::FirstSFCCardIndex; i++)
    {
        ItsConfigArr[i]->PrintGlobal();
    }
}

//Delete all XdpConfiguration files from flash,  Used for tests
void XdpInSfcConfigMngr::DeleteFiles()
{
    for (UINT32 i = CardIndex::FirstCardIndex; i <= CardIndex::FirstSFCCardIndex; i++)
    {
        ItsConfigArr[i]->DeleteFile();
    }
}
