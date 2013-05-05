/* XdpTraceConfig.cc - <module's short description>  */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpTraceConfig
   Storage element that holds the configuration for an XdpTrace object

AUTHOR
   Amit Rapaport, Seabridge

**************************************************************************/

#include "XdpTraceConfig.h"
#include "XdpTrace.h"

//-------------------------------------------------------------------------
// public
//-------------------------------------------------------------------------

// Constructor & Destructor

XdpTraceConfig::XdpTraceConfig(UINT32 theCardIndex,
                               const RWCString &theTraceNameR,
                               XdpConfigService* theConfServiceP)
{

    ItsCardIndex = CardIndex(theCardIndex);
    ItsConfServiceP = theConfServiceP;
    ItsName = theTraceNameR;
    UINT32 appTag;
    for (appTag = XDP_TRACE_APP_FIRST-1; appTag <= XDP_TRACE_APP_LAST-1; appTag++)
    {
        ItsEnableColorsBuffP[appTag] = 0;
        ItsPrintActiveColorsBuffP[appTag] = 0;
    }
    ItsEnableAppTags = 1 << XDP_TRACE_APP_DEFAULT;
    ItsPrintActiveAppTags = 1 << XDP_TRACE_APP_DEFAULT;
    // to change -- XDP_TRACE_OUTPUT_UDP
    ItsPrintOutputChannel = 1;

}
// Copy constructor & Assign op

XdpTraceConfig::~XdpTraceConfig(){}


// Miscellaneous Operations
/**************************************************************************
*
* UpdateConfig
*
* DESCRIPTION
*    Update members
*
**************************************************************************/
void XdpTraceConfig::UpdateConfig(const UINT32 theEnableAppTags,
                                  UINT32 const *theEnableColorsBufferP,
                                  const UINT32 thePrintActiveAppTags,
                                  UINT32 const *thePrintActiveColorsBufferP,
                                  const UINT32 thePrintOutputChannel)
{

    ItsEnableAppTags = theEnableAppTags;
    UINT32 appTag;
    for (appTag = XDP_TRACE_APP_FIRST-1; appTag <= XDP_TRACE_APP_LAST-1; appTag++)
    {
        ItsEnableColorsBuffP[appTag] = theEnableColorsBufferP[appTag];
        ItsPrintActiveColorsBuffP[appTag] = thePrintActiveColorsBufferP[appTag];
    }
    ItsPrintActiveAppTags = thePrintActiveAppTags;
    ItsPrintOutputChannel = thePrintOutputChannel;
}

/**************************************************************************
*
* SaveConfig
*
* DESCRIPTION
*    Write the configuration from object to the file on flash.
*
**************************************************************************/
void XdpTraceConfig::SaveConfig(FILE *theFileP)
{
    RN_ASSERT ((theFileP!=NULL), "File Handler pointer is NULL");
    fprintf(theFileP, "%s %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u \n",
            ItsName.operator const char*(), ItsEnableAppTags, ItsEnableColorsBuffP[0],
            ItsEnableColorsBuffP[1], ItsEnableColorsBuffP[2], ItsEnableColorsBuffP[3],
            ItsEnableColorsBuffP[4], ItsEnableColorsBuffP[5], ItsEnableColorsBuffP[6],
            ItsEnableColorsBuffP[7], ItsEnableColorsBuffP[8], ItsEnableColorsBuffP[9],
            ItsEnableColorsBuffP[10], ItsEnableColorsBuffP[11], ItsEnableColorsBuffP[12],
            ItsEnableColorsBuffP[13], ItsEnableColorsBuffP[14], ItsPrintActiveAppTags,
            ItsPrintActiveColorsBuffP[0], ItsPrintActiveColorsBuffP[1], ItsPrintActiveColorsBuffP[2],
            ItsPrintActiveColorsBuffP[3], ItsPrintActiveColorsBuffP[4], ItsPrintActiveColorsBuffP[5],
            ItsPrintActiveColorsBuffP[6], ItsPrintActiveColorsBuffP[7], ItsPrintActiveColorsBuffP[8],
            ItsPrintActiveColorsBuffP[9], ItsPrintActiveColorsBuffP[10], ItsPrintActiveColorsBuffP[11],
            ItsPrintActiveColorsBuffP[12], ItsPrintActiveColorsBuffP[13], ItsPrintActiveColorsBuffP[14],
            ItsPrintOutputChannel);
}


/**************************************************************************
*
* DownloadConfig
*
* DESCRIPTION
*       Copy configuration from object to the XdpTrace real object
*
**************************************************************************/
void XdpTraceConfig::DownloadConfig()
{
    ItsConfServiceP->DownloadTraceConfig(ItsCardIndex,
                                         ItsName,
                                         ItsEnableAppTags,
                                         ItsEnableColorsBuffP,
                                         ItsPrintActiveAppTags,
                                         ItsPrintActiveColorsBuffP,
                                         ItsPrintOutputChannel);
}


// Get methods
/**************************************************************************
*
* GetName
*
* DESCRIPTION
*    returns the XdpTrace name
*
**************************************************************************/
   RWCString XdpTraceConfig::GetName(){
        return ItsName;
    }



/**************************************************************************
*
* PrintTraceConfig
*
* DESCRIPTION
*    Print current values of the XdpTraceConfig
*   Uesd for tests!!
*
**************************************************************************/

void XdpTraceConfig::PrintTraceConfig()
{
    printf("### print trace config ###\n");
    printf("Trace name = %s\n",ItsName.operator const char*());
    printf("EnableAppTags = %d\n",ItsEnableAppTags);
    UINT32 appTag;
    printf("ItsEnableColorsBuffP[appTag] ::\n");
    for(appTag = 0; appTag <= 13; appTag++)
    {
        printf(" [%d] = %d ,",appTag,ItsEnableColorsBuffP[appTag]);
    }
    printf(" [14] = %d \n",ItsEnableColorsBuffP[14]);
    printf("ItsPrintActiveAppTags = %d\n",ItsPrintActiveAppTags);
    printf("ItsPrintActiveColorsBuffP[appTag] ::\n");
    for (appTag = 0; appTag <= 13; appTag++)
    {
        printf("  [%d] = %d  ",appTag ,ItsPrintActiveColorsBuffP[appTag]);
    }
    printf(" [14] = %d \n",ItsPrintActiveColorsBuffP[14]);
    printf("ItsPrintOutputChannel = %d\n",ItsPrintOutputChannel);
}



