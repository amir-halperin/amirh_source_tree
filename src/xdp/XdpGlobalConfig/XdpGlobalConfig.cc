/* XdpGlobalConfig.cc */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpGlobalConfig
   Storage element which holds the XDP configuration per card (LICs and SFC):
    - Global configuration (IP address, port etc.)
    - linked list of pointers to XdpTraceConfig objects, each one of those holds
         the configuration of one XdpTrace object

AUTHOR
   Amit Rapaport, Seabridge

**************************************************************************/
#include <stdio.h>

#include "XdpGlobalConfig.h"

#include "SLinkList.h"
#include "RWHdr/ctoken.h"
#include "casts.h"
#include "XdpTraceConfig.h"

extern const MaskFlagId MASK_XDP;

#define NUMBER_OF_TRACE_PARAMS 34
#define NUMBER_OF_GLOBAL_PARAMS 4
#define GLOBAL_HEADER "IP Address | IP Port Number | Output To UDP | Output To Serial\n"
#define TRACE_HEADER_1 " Trace Name | Enable App Tags | Enabled Colors - App Tag | Print Active App Tags | Print Output Channel - App Tag \n"

//-------------------------------------------------------------------------
// public
//-------------------------------------------------------------------------

// Constructor & Destructor


XdpGlobalConfig::XdpGlobalConfig(UINT32 theCardIndex, XdpConfigService *theConfigServicesP)
{
    RN_ASSERT ((NULL != theConfigServicesP), "Xdp Config Service pointer is NULL");

    ItsCardIndex = CardIndex(theCardIndex);
    ItsConfServiceP = theConfigServicesP;
    sprintf(ItsFileName,"\\MNGMNT\\XDP_%d",ItsCardIndex.AsUINT32());
    ItsConfigFileFlag = false;
    ItsOutputToUdpHostIp = IpAddress("172.30.1.66");;
    ItsOutputToUdpHostPort = 5999;
    ItsOutputToUdpEnableFlag = false;
    ItsOutputToSerialEnableFlag = false;

}


/**************************************************************************
*
* UpdateFileFlag
*
* DESCRIPTION
*   In order to see if configuration file exist,
*   try to open it in "read only" mode
*
**************************************************************************/

void XdpGlobalConfig::UpdateFileFlag()
{
    FILE *fileP = fopen(ItsFileName, "r");
    if (NULL != fileP)
    {
        ItsConfigFileFlag = true;
    }
    else
    {
        ItsConfigFileFlag = false;
    }
    fclose(fileP);
}

// Get methods
/**************************************************************************
*
* GetTraceConfig
*
* DESCRIPTION
*   walk over the LinkedList of the XdpTraceConfig objects and returns a
*   pointer to the XdpTraceConfig with name theTraceNameR, or NULL if such
*   does not exist
*
**************************************************************************/
XdpTraceConfig* XdpGlobalConfig::GetTraceConfig(const RWCString& theTraceNameR)
{
    RN_ASSERT ((NULL != theTraceNameR), "Xdp trace name reference is NULL");
    RN_TRACE(MASK_XDP,"XdpTraceContainer::GetTraceConfig" + theTraceNameR);

    SLinkList::SLinkListIter iter;
    iter = ItsTraceConfigList.Begin();
    XdpTraceConfig *traceConfigP = rn_static_cast(XdpTraceConfig*, iter.Get());
    while (NULL != traceConfigP)
    {
        RWCString traceConfigPName = traceConfigP->GetName();
		if (0 == theTraceNameR.compareTo(traceConfigPName))
        {
			break;
		}
		
		//get next
        ++iter;
        traceConfigP = rn_static_cast(XdpTraceConfig*, iter.Get());
    }
    return traceConfigP;
}


/**************************************************************************
*
* NewTraceConfig
*
* DESCRIPTION
*   Create a new XdpTraceConfig object and add it to the list
*
**************************************************************************/

XdpTraceConfig* XdpGlobalConfig::NewTraceConfig(const RWCString& theTraceNameR)
{
    RN_ASSERT ((NULL != theTraceNameR), "Trace name is not valid");

    XdpTraceConfig *traceConfigP = new XdpTraceConfig(ItsCardIndex.AsUINT32(),theTraceNameR, ItsConfServiceP);
    if (NULL == traceConfigP)
    {
        REC_FATAL(17542);
    }
    AddToTraceConfigList(traceConfigP);
    return traceConfigP;
}


/**************************************************************************
*
* AddToTraceConfigList
*
* DESCRIPTION
*   gets a pointer to an XdpTraceConfig object and adds it to its list
*   of pointers in the wright position (sorted by name)
*
**************************************************************************/
void XdpGlobalConfig::AddToTraceConfigList(XdpTraceConfig* theTraceConfigP)
{


    RN_ASSERT ((NULL != theTraceConfigP), "XdpTraceConfig pointer is NULL");
    RN_TRACE(MASK_XDP,"XdpGlobalConfig::AddToTraceConfigList "+ theTraceConfigP->GetName());


    if (NULL != theTraceConfigP)
    {
        RWCString traceToInsertName = theTraceConfigP->GetName();

        if (ItsTraceConfigList.IsEmpty())
        {
            ItsTraceConfigList.InsertFirst(theTraceConfigP);
        }
        else if(NULL != GetTraceConfig(traceToInsertName))
        {
            REC_FATAL(17541);
        }
        else
        {
            SLinkList::SLinkListIter iterCurrent;
            iterCurrent = ItsTraceConfigList.Begin();

            //current trace  & its name
            XdpTraceConfig *currentTraceP = rn_static_cast(XdpTraceConfig*, iterCurrent.Get());
            RWCString currentTraceName = currentTraceP->GetName();

            if(traceToInsertName.compareTo(currentTraceName) < 0)
            {
                ItsTraceConfigList.InsertFirst(theTraceConfigP);
            }

            else
            {
                //next trace iterator
                SLinkList::SLinkListIter iterNext;
                iterNext = ItsTraceConfigList.Begin();
                ++iterNext;
                XdpTraceConfig *nextTraceP = rn_static_cast(XdpTraceConfig*, iterNext.Get());

                while (NULL != currentTraceP && NULL != nextTraceP)
                {
                    //curent trace name
                    currentTraceName = currentTraceP->GetName();
                    //next trace name
                    RWCString nextTraceName = nextTraceP->GetName();
                    if((traceToInsertName.compareTo(currentTraceName) > 0))
                    {
                        if(traceToInsertName.compareTo(nextTraceName)<0)
                        {
                            break;
                        }
                    }

                    //get next
                    ++iterCurrent;
                    currentTraceP = rn_static_cast(XdpTraceConfig*, iterCurrent.Get());

                    ++iterNext;
                    nextTraceP = rn_static_cast(XdpTraceConfig*, iterNext.Get());
                }
                ItsTraceConfigList.Insert(iterCurrent,theTraceConfigP);
            }
        }
        //printTraceList();
    }

}


/**************************************************************************
*
* RestoreConfig
*
* DESCRIPTION
*   Print the list of XdpTraceConfig
*
***************************************************************************/
void XdpGlobalConfig::PrintList()
{
    SLinkList::SLinkListIter it;
    it = ItsTraceConfigList.Begin();
    XdpTraceConfig *trace = rn_static_cast(XdpTraceConfig*, it.Get());
    while(NULL != trace)
    {
        printf("%s\n",trace->GetName().operator const char*());
        ++it;
        trace = rn_static_cast(XdpTraceConfig*, it.Get());

    }
}


/**************************************************************************
*
* RestoreConfig
*
* DESCRIPTION
*   Read the configuration file from flash, line by line:
*   restore the global configuration (IP address, port etc.)
*   by calling RestoreGlobalConfigFromString.
*   create the XdpTraceConfig objects and restore their configuration
*   by calling RestoreTraceConfigFromString for each one of them,
*   then add each to the sorted LinkedList
*
**************************************************************************/
void XdpGlobalConfig::RestoreConfig()
{
    UpdateFileFlag();
    if (true == ItsConfigFileFlag)
    {
        RN_TRACE(MASK_XDP,"::Restoring Configuration for card " << ItsCardIndex);
        FILE *fileP = fopen(ItsFileName,"r");
        if (NULL == fileP)
        {
            perror ("Open Xdp configuration file ");
        }
        char buffer[MAX_CONFIG_LEN];
        UINT32 lineNum = 1;
        while( fgets(buffer,MAX_CONFIG_LEN, fileP) != NULL)
        {
            char *header;
            if (1 == lineNum)
            {

                if (0 != memcmp(buffer, GLOBAL_HEADER, strlen(GLOBAL_HEADER)))
                {
                    REC_FATAL(17545);
                }
            }
            if (2 == lineNum)
            {
                RestoreGlobalConfigFromString(buffer);
            }
            else if (4 <= lineNum )
            {
                RestoreTraceConfigFromString(buffer);
            }
            lineNum++;
        }
        fclose (fileP);
    }
    else
    {
        RN_TRACE(MASK_XDP,"::No Configuration to restore for card " << ItsCardIndex);
    }
}


/**************************************************************************
*
* RestoreGlobalConfigFromString
*
* DESCRIPTION
*    Parse theLine (read from the flash file) and restore the
*    configuration into XdpGlobalConfig parameters
*
**************************************************************************/
void XdpGlobalConfig::RestoreGlobalConfigFromString(const char *theLine)
{

    RN_ASSERT ((NULL != theLine), ":: The configuration line is not valid");

     char ipAddress[20];
     memset(ipAddress,0,20);
     UINT32 ipPort;
     bool outputToUdpEnableFlag;
     bool outputToSerialEnableFlag;
     UINT32 rc;
     rc = sscanf(theLine, "%s %u %u %u",ipAddress, &ipPort, &outputToUdpEnableFlag, &outputToSerialEnableFlag);
     if(NUMBER_OF_GLOBAL_PARAMS != rc)
     {
         REC_FATAL(17543);
     }
     rn_static_cast(UINT16, ipPort);
     IpAddress ip = IpAddress(ipAddress);

     UpdateConfig(ip, ipPort, (bool)outputToUdpEnableFlag, (bool)outputToSerialEnableFlag);
}

/**************************************************************************
*
* RestoreTraceConfigFromString
*
* DESCRIPTION
*    Parse theLine (read from the flash file)
*    create a new XdpTraceConfig object, update its params
*    and add it to the list - ItsTraceConfigListP
*
**************************************************************************/
void XdpGlobalConfig::RestoreTraceConfigFromString(const char *theLine)
{
    RN_ASSERT ((NULL != theLine), ":: The configuration line is not valid");
    char traceName[XDP_TRACE_NAME_MAX_LEN];
    memset(traceName,0,XDP_TRACE_NAME_MAX_LEN);
    UINT32 enableAppTags;
    UINT32 enableColorsBuffP[XDP_TRACE_APP_LAST];
    UINT32 printActiveAppTags;
    UINT32 printActiveColorsBuffP[XDP_TRACE_APP_LAST];
    UINT32 printOutputChannel;

    UINT32 rc;
    rc = sscanf(theLine, "%s %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n",
            traceName, &enableAppTags, &enableColorsBuffP[0], &enableColorsBuffP[1], &enableColorsBuffP[2],
            &enableColorsBuffP[3], &enableColorsBuffP[4], &enableColorsBuffP[5], &enableColorsBuffP[6],
            &enableColorsBuffP[7], &enableColorsBuffP[8], &enableColorsBuffP[9], &enableColorsBuffP[10],
            &enableColorsBuffP[11], &enableColorsBuffP[12], &enableColorsBuffP[13], &enableColorsBuffP[14],
            &printActiveAppTags, &printActiveColorsBuffP[0], &printActiveColorsBuffP[1],
            &printActiveColorsBuffP[2], &printActiveColorsBuffP[3], &printActiveColorsBuffP[4],
            &printActiveColorsBuffP[5], &printActiveColorsBuffP[6], &printActiveColorsBuffP[7],
            &printActiveColorsBuffP[8], &printActiveColorsBuffP[9], &printActiveColorsBuffP[10],
            &printActiveColorsBuffP[11], &printActiveColorsBuffP[12], &printActiveColorsBuffP[13],
            &printActiveColorsBuffP[14],
            &printOutputChannel);

    printf("number of params read = %d\n", rc);
    //if(NUMBER_OF_TRACE_PARAMS != rc)
    //{
      //  REC_FATAL(17543);
    //}
    RWCString theName = RWCString(traceName);
    //create new XdpTraceConfig
    XdpTraceConfig *traceConfigP = new XdpTraceConfig(ItsCardIndex.AsUINT32(), theName, ItsConfServiceP);
    if(NULL == traceConfigP)
    {
        REC_FATAL(17542);
    }
    //Update its params
    traceConfigP->UpdateConfig(enableAppTags, enableColorsBuffP, printActiveAppTags,
                               printActiveColorsBuffP, printOutputChannel);
    //add it into the list of XdpTraceConfig
    AddToTraceConfigList(traceConfigP);
}

/**************************************************************************
*
* DownloadConfig
*
* DESCRIPTION
*    Download all configuration to the real objects:
*    from XdpGlobalConfig to XdpServices using theConfigServicesP ,
*    and from each XdpTraceConfig to the coresponding XdpTrace real object
*    by calling DownloadConfig
*
**************************************************************************/

void XdpGlobalConfig::DownloadConfig()
{
   if (true == ItsConfigFileFlag)
    {

        ItsConfServiceP->DownloadGlobalConfig(ItsCardIndex,
                                              ItsOutputToUdpHostIp,
                                              ItsOutputToUdpHostPort,
                                              ItsOutputToUdpEnableFlag,
                                              ItsOutputToSerialEnableFlag);

        SLinkList::SLinkListIter iter;
        iter = ItsTraceConfigList.Begin();
        XdpTraceConfig *traceConfigP = rn_static_cast(XdpTraceConfig*, iter.Get());
        while(NULL != traceConfigP)
        {
            traceConfigP->DownloadConfig();
            //get next
		    ++iter;
            traceConfigP = rn_static_cast(XdpTraceConfig*, iter.Get());
        }
    }
    else
    {
        RN_TRACE(MASK_XDP,"::No Configuration to download from card " << ItsCardIndex);
    }
}


/**************************************************************************
*
* UpdateConfig
*
* DESCRIPTION
*    Set the new values to all global parameters in XdpGlobalConfig
*
**************************************************************************/
void XdpGlobalConfig::UpdateConfig(IpAddress theOutputToUdpHostIp,
                                   IpPortNumber theOutputToUdpHostPort,
                                   bool theOutputToUdpEnableFlag,
                                   bool theOutputToSerialEnableFlag)
{
    ItsOutputToUdpHostIp = theOutputToUdpHostIp;
    ItsOutputToUdpHostPort = theOutputToUdpHostPort;
    ItsOutputToUdpEnableFlag = theOutputToUdpEnableFlag;
    ItsOutputToSerialEnableFlag = theOutputToSerialEnableFlag;

}

/**************************************************************************
*
* SaveConfig
*
* DESCRIPTION
*    Save current values of the global parameters to the flash file,
*    for each XdpTraceConfig, call SaveTraceConfig(FILE*)
*
**************************************************************************/
void XdpGlobalConfig::SaveConfig()
{
    FILE *fileP = fopen(ItsFileName,"w+");
    if (NULL == fileP)
    {
      REC_FATAL(17538);
    }
    //add the header

    fwrite(GLOBAL_HEADER, 1, strlen(GLOBAL_HEADER), fileP);
    // convert IpAddress to RWCString
    RWCString ip = ItsOutputToUdpHostIp.ToStr();
    fprintf(fileP, "%s %d %d %d\n", ip.operator const char*(),ItsOutputToUdpHostPort,
            ItsOutputToUdpEnableFlag,ItsOutputToSerialEnableFlag);


    fwrite(TRACE_HEADER_1, 1, strlen(TRACE_HEADER_1), fileP);
    //go over the traces list and write thier config to the file

    SLinkList::SLinkListIter iter;
    iter = ItsTraceConfigList.Begin();
    XdpTraceConfig *traceConfigP = rn_static_cast(XdpTraceConfig*, iter.Get());
    while(NULL != traceConfigP)
    {
        traceConfigP->SaveConfig(fileP);
        //get next
        ++iter;
        traceConfigP = rn_static_cast(XdpTraceConfig*, iter.Get());
    }

    fclose(fileP);
    UpdateFileFlag();
}



/**************************************************************************
*
* PrintGlobal
*
* DESCRIPTION
*    Print current values of XdpGlobalConfig, and for each XdpTraceConfig
*    call PrintTraceConfig() to print its parameters.
*    Used for tests!!
*
**************************************************************************/
void XdpGlobalConfig::PrintGlobal()
{
    printf("************* print global config ***************\n");
    printf("file name = %s\n",ItsFileName);
    if(false != ItsConfigFileFlag)
    {
        printf("IP address = %s\n", ItsOutputToUdpHostIp.ToStr().operator const char*());
        printf("IP Port = %d\n", ItsOutputToUdpHostPort);
        printf("OutputToUdpEnableFlag = %d\n", ItsOutputToUdpEnableFlag);
        printf("OutputToSerialEnableFlag = %d\n", ItsOutputToSerialEnableFlag);

        SLinkList::SLinkListIter iter;
        iter = ItsTraceConfigList.Begin();
        XdpTraceConfig *traceConfigP = rn_static_cast(XdpTraceConfig*, iter.Get());
        while(NULL != traceConfigP)
        {
            traceConfigP->PrintTraceConfig();
            ++iter;
            traceConfigP = rn_static_cast(XdpTraceConfig*, iter.Get());
        }

    }
}



/**************************************************************************
*
* DeleteFile
*
* DESCRIPTION
*    Delete configuration file from flash
*    Used for tests!!
*
**************************************************************************/
void XdpGlobalConfig::DeleteFile()
{
    UINT32 rc = remove(ItsFileName);
    if(rc != 0)
    {
        printf("Did NOT remove the file %s\n",ItsFileName);
    }
}


/**************************************************************************
*
* PrintFile
*
* DESCRIPTION
*    Print to screen the configuration file from flash
*    Used for tests!!
*
**************************************************************************/
void XdpGlobalConfig::PrintFile()
{
    printf("PRINTING FILE ********* %s\n", ItsFileName);
    FILE *fileP = fopen(ItsFileName,"r");
    char buffer[250];
    if (NULL != fileP)
    {
        while( fgets(buffer,250, fileP) != NULL)
        {
            printf(buffer);
        }
        fclose (fileP);
    }
    else
    {
        printf("No file to print!!!\n");
    }

}
