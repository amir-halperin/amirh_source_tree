/**************************************************************/
// Class: RpcRxXdpRemoteConfigProxy
// File: RpcRxXdpRemoteConfigProxy.cc
// Author: ACS API Generator
// Creation Date: Wed Feb  8 07:52:07 2006
//
// Purpose: ACS Rx API for XdpRemoteConfigProxy
//     Rpc_Phy - Card to Mcp
/**************************************************************/

#include "UINT32Array.h"
#include "UINT8Buffer.h"
#include "IpAddress.h"
#include "RpcCmnApiRxInMain.h"
#include "AcsAppNameToId.h"
#include "Error.h"
#include "AcsAppInfo.h"
#include "XdpRemoteConfigProxy.h"
#include "RpcRxXdpRemoteConfigProxy.h"

extern const MaskFlagId MASK_RPC_PRN;



AcsAppInfo *RpcRxXdpRemoteConfigProxy::TheirAppInfoP = NULL;

AcsAppId RpcRxXdpRemoteConfigProxy::appId;

void RpcRxXdpRemoteConfigProxy::RpcPhyRqstWithRspInMain (const CardIndex theCard,
                AcsPacket *&theRxPacketP,
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

void RpcRxXdpRemoteConfigProxy::RpcPhyRelRqstInMain (const CardIndex theCard, 
                AcsPacket *&theRxPacketP)
{
    AcsAppServiceId funcIndex = UnpackFuncType(theRxPacketP);

    switch (funcIndex) {
    case 1:
        SaveCardGlobalConfig1 (theCard, theRxPacketP);
        break;

    case 2:
        SaveCardTraceConfig2 (theCard, theRxPacketP);
        break;

    default:
        REC_FATAL (11601);
    }
    delete theRxPacketP;
    theRxPacketP=0;
}

AcsAppServiceId RpcRxXdpRemoteConfigProxy::UnpackFuncType (AcsPacket *theRxPacketP)
{
    AcsAppServiceId FuncIndex = theRxPacketP->GetAppServiceId();

    return FuncIndex;
}

void RpcRxXdpRemoteConfigProxy::Init ()
{
    appId = AcsAppNameToId::TheNameToId()->GetId("RPC_XdpRemoteConfigProxy");


    if (TheirAppInfoP == NULL) {
        TheirAppInfoP = AcsAppInfo::Create( "XdpRemoteConfigProxy" , "RPC" , false , MASK_RPC_PRN );

    }

    RpcCmnApiRxInMain::TheRpc()->RegisterRelRqst(appId ,
            RpcRxXdpRemoteConfigProxy::RpcPhyRelRqstInMain);
}

void RpcRxXdpRemoteConfigProxy::SaveCardGlobalConfig1 (const CardIndex theCard, AcsPacket *packetP)
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
        RN_TRACE (MASK_RPC_PRN, "RpcRxXdpRemoteConfigProxy::SaveCardGlobalConfig called. In Param:");
        RN_TRACE (MASK_RPC_PRN, "CardIndex: " << theCard);
        RN_TRACE (MASK_RPC_PRN, "IpAddress: " << theParamIn1);
        RN_TRACE (MASK_RPC_PRN, "UINT16: " << theParamIn2);
        RN_TRACE (MASK_RPC_PRN, "BOOL32: " << theParamIn3);
        RN_TRACE (MASK_RPC_PRN, "BOOL32: " << theParamIn4);
        RN_TRACE (MASK_RPC_PRN, "------------------------------");
    }

    XdpRemoteConfigProxy::SaveCardGlobalConfig(theCard,
         theParamIn1, theParamIn2, theParamIn3, theParamIn4);

}

void RpcRxXdpRemoteConfigProxy::SaveCardTraceConfig2 (const CardIndex theCard, AcsPacket *packetP)
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
        RN_TRACE (MASK_RPC_PRN, "RpcRxXdpRemoteConfigProxy::SaveCardTraceConfig called. In Param:");
        RN_TRACE (MASK_RPC_PRN, "CardIndex: " << theCard);
        RN_TRACE (MASK_RPC_PRN, "UINT8Buffer: " << theParamIn1);
        RN_TRACE (MASK_RPC_PRN, "UINT32: " << theParamIn2);
        RN_TRACE (MASK_RPC_PRN, "UINT32Array: " << theParamIn3);
        RN_TRACE (MASK_RPC_PRN, "UINT32: " << theParamIn4);
        RN_TRACE (MASK_RPC_PRN, "UINT32Array: " << theParamIn5);
        RN_TRACE (MASK_RPC_PRN, "UINT32: " << theParamIn6);
        RN_TRACE (MASK_RPC_PRN, "------------------------------");
    }

    XdpRemoteConfigProxy::SaveCardTraceConfig(theCard,
         theParamIn1, theParamIn2, theParamIn3, theParamIn4, theParamIn5, theParamIn6);

}

