/**************************************************************/
// Class: RpcRxXdpInLicConfigMngr
// File: RpcRxXdpInLicConfigMngr.cc
// Author: ACS API Generator
// Creation Date: Wed Feb  8 10:09:24 2006
//
// Purpose: ACS Rx API for XdpInLicConfigMngr
//     Rpc_Phy - Mcp to Card
/**************************************************************/

#include "UINT32Array.h"
#include "UINT8Buffer.h"
#include "IpAddress.h"
#include "RpcCmnApiRxInCard.h"
#include "AcsAppNameToId.h"
#include "Error.h"
#include "AcsAppInfo.h"
#include "XdpInLicConfigMngr.h"
#include "RpcRxXdpInLicConfigMngr.h"

extern const MaskFlagId MASK_RPC_PRN;



AcsAppInfo *RpcRxXdpInLicConfigMngr::TheirAppInfoP = NULL;

AcsAppId RpcRxXdpInLicConfigMngr::appId;

void RpcRxXdpInLicConfigMngr::RpcRqstWithRspInCard (AcsPacket *&theRxPacketP,
                AcsPacket *&theTxPacketP)
{
    AcsAppServiceId funcIndex = UnpackFuncType(theRxPacketP);

    switch (funcIndex) {
    default:
        REC_FATAL (11601);
    }
    delete theRxPacketP;
    theRxPacketP=0;
}

void RpcRxXdpInLicConfigMngr::RpcRelRqstInCard (AcsPacket *&theRxPacketP)
{
    AcsAppServiceId funcIndex = UnpackFuncType(theRxPacketP);

    switch (funcIndex) {
    case 1:
        DownloadGlobalConfig1 (theRxPacketP);
        break;

    case 2:
        DownloadTraceConfig2 (theRxPacketP);
        break;

    default:
        REC_FATAL (11601);
    }
    delete theRxPacketP;
    theRxPacketP=0;
}

AcsAppServiceId RpcRxXdpInLicConfigMngr::UnpackFuncType (AcsPacket *theRxPacketP)
{
    AcsAppServiceId FuncIndex = theRxPacketP->GetAppServiceId();

    return FuncIndex;
}

void RpcRxXdpInLicConfigMngr::Init ()
{
    appId = AcsAppNameToId::TheNameToId()->GetId("RPC_XdpInLicConfigMngr");


    if (TheirAppInfoP == NULL) {
        TheirAppInfoP = AcsAppInfo::Create( "XdpInLicConfigMngr" , "RPC" , false , MASK_RPC_PRN );

    }

    RpcCmnApiRxInCard::TheRegularRpc()->RegisterRelRqst(appId ,
            RpcRxXdpInLicConfigMngr::RpcRelRqstInCard);
    RpcCmnApiRxInCard::TheHighRpc()->RegisterRelRqst(appId ,
            RpcRxXdpInLicConfigMngr::RpcRelRqstInCard);
}

void RpcRxXdpInLicConfigMngr::DownloadGlobalConfig1 (AcsPacket *packetP)
{
    IpAddress theParamIn1(*packetP);
    UINT16 theParamIn2;
    *packetP>>theParamIn2;
    BOOL32 theParamIn3;
    *packetP>>theParamIn3;
    BOOL32 theParamIn4;
    *packetP>>theParamIn4;


    if ( TheirAppInfoP->IsTraceEnable(1) ) {

        RN_TRACE (MASK_RPC_PRN, "------------------------------");
        RN_TRACE (MASK_RPC_PRN, "RpcRxXdpInLicConfigMngr::DownloadGlobalConfig called. In Param:");
        RN_TRACE (MASK_RPC_PRN, "IpAddress: " << theParamIn1);
        RN_TRACE (MASK_RPC_PRN, "UINT16: " << theParamIn2);
        RN_TRACE (MASK_RPC_PRN, "BOOL32: " << theParamIn3);
        RN_TRACE (MASK_RPC_PRN, "BOOL32: " << theParamIn4);
        RN_TRACE (MASK_RPC_PRN, "------------------------------");
    }

    XdpInLicConfigMngr::DownloadGlobalConfig(
         theParamIn1, theParamIn2, theParamIn3, theParamIn4);

}

void RpcRxXdpInLicConfigMngr::DownloadTraceConfig2 (AcsPacket *packetP)
{
    UINT8Buffer theParamIn1(*packetP);
    UINT32 theParamIn2;
    *packetP>>theParamIn2;
    UINT32Array theParamIn3(*packetP);
    UINT32 theParamIn4;
    *packetP>>theParamIn4;
    UINT32Array theParamIn5(*packetP);
    UINT32 theParamIn6;
    *packetP>>theParamIn6;


    if ( TheirAppInfoP->IsTraceEnable(2) ) {

        RN_TRACE (MASK_RPC_PRN, "------------------------------");
        RN_TRACE (MASK_RPC_PRN, "RpcRxXdpInLicConfigMngr::DownloadTraceConfig called. In Param:");
        RN_TRACE (MASK_RPC_PRN, "UINT8Buffer: " << theParamIn1);
        RN_TRACE (MASK_RPC_PRN, "UINT32: " << theParamIn2);
        RN_TRACE (MASK_RPC_PRN, "UINT32Array: " << theParamIn3);
        RN_TRACE (MASK_RPC_PRN, "UINT32: " << theParamIn4);
        RN_TRACE (MASK_RPC_PRN, "UINT32Array: " << theParamIn5);
        RN_TRACE (MASK_RPC_PRN, "UINT32: " << theParamIn6);
        RN_TRACE (MASK_RPC_PRN, "------------------------------");
    }

    XdpInLicConfigMngr::DownloadTraceConfig(
         theParamIn1, theParamIn2, theParamIn3, theParamIn4, theParamIn5, theParamIn6);

}

