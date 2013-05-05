/**************************************************************/
// Class: RpcTxXdpRemoteConfigProxy
// File: RpcTxXdpRemoteConfigProxy.cc
// Author: ACS API Generator
// Creation Date: Wed Feb  8 07:52:07 2006
//
// Purpose: ACS Tx API for XdpRemoteConfigProxy
//     Rpc_Phy - Card to Mcp
/**************************************************************/

#include "RpcCmnApiTxInCard.h"
#include "AcsAppNameToId.h"
#include "Error.h"
#include "AcsAppInfo.h"
#include "RpcTxXdpRemoteConfigProxy.h"

extern const MaskFlagId MASK_RPC_PRN;



AcsAppInfo *RpcTxXdpRemoteConfigProxy::TheirAppInfoP = NULL;

AcsAppId RpcTxXdpRemoteConfigProxy::appId;

void RpcTxXdpRemoteConfigProxy::SaveCardGlobalConfig (
                                                      const IpAddress & theParamIn1,
                                                      const UINT16 theParamIn2,
                                                      const BOOL32 theParamIn3,
                                                      const BOOL32 theParamIn4)
{


    if ( TheirAppInfoP->IsTraceEnable(1) ) {

        RN_TRACE (MASK_RPC_PRN, "------------------------------");
        RN_TRACE (MASK_RPC_PRN, "RpcTxXdpRemoteConfigProxy::SaveCardGlobalConfig called. In Param:");
        RN_TRACE (MASK_RPC_PRN, "IpAddress: " << theParamIn1);
        RN_TRACE (MASK_RPC_PRN, "UINT16: " << theParamIn2);
        RN_TRACE (MASK_RPC_PRN, "BOOL32: " << theParamIn3);
        RN_TRACE (MASK_RPC_PRN, "BOOL32: " << theParamIn4);
        RN_TRACE (MASK_RPC_PRN, "------------------------------");
    }

    AcsPacket *txPacketP = PackSaveCardGlobalConfig1 (
        theParamIn1,
        theParamIn2,
        theParamIn3,
        theParamIn4);

    RpcCmnApiTxInCard::TheRpc()->ReliableRequest(txPacketP);
}

void RpcTxXdpRemoteConfigProxy::SaveCardTraceConfig (
                                                     const UINT8Buffer & theParamIn1,
                                                     const UINT32 theParamIn2,
                                                     const UINT32Array & theParamIn3,
                                                     const UINT32 theParamIn4,
                                                     const UINT32Array & theParamIn5,
                                                     const UINT32 theParamIn6)
{


    if ( TheirAppInfoP->IsTraceEnable(2) ) {

        RN_TRACE (MASK_RPC_PRN, "------------------------------");
        RN_TRACE (MASK_RPC_PRN, "RpcTxXdpRemoteConfigProxy::SaveCardTraceConfig called. In Param:");
        RN_TRACE (MASK_RPC_PRN, "UINT8Buffer: " << theParamIn1);
        RN_TRACE (MASK_RPC_PRN, "UINT32: " << theParamIn2);
        RN_TRACE (MASK_RPC_PRN, "UINT32Array: " << theParamIn3);
        RN_TRACE (MASK_RPC_PRN, "UINT32: " << theParamIn4);
        RN_TRACE (MASK_RPC_PRN, "UINT32Array: " << theParamIn5);
        RN_TRACE (MASK_RPC_PRN, "UINT32: " << theParamIn6);
        RN_TRACE (MASK_RPC_PRN, "------------------------------");
    }

    AcsPacket *txPacketP = PackSaveCardTraceConfig2 (
        theParamIn1,
        theParamIn2,
        theParamIn3,
        theParamIn4,
        theParamIn5,
        theParamIn6);

    RpcCmnApiTxInCard::TheRpc()->ReliableRequest(txPacketP);
}

void RpcTxXdpRemoteConfigProxy::Init() {
    appId = AcsAppNameToId::TheNameToId()->GetId("RPC_XdpRemoteConfigProxy");

    if (TheirAppInfoP == NULL) {
        TheirAppInfoP = AcsAppInfo::Create( "XdpRemoteConfigProxy" , "RPC" , true , MASK_RPC_PRN );

    }

}

AcsPacket *RpcTxXdpRemoteConfigProxy::PackSaveCardGlobalConfig1 (
                                                                 const IpAddress & theParamIn1,
                                                                 const UINT16 theParamIn2,
                                                                 const BOOL32 theParamIn3,
                                                                 const BOOL32 theParamIn4)
{
    UINT32 packetSize = 
        theParamIn1.GetAcsSize()+ 
        sizeof(UINT16)+ 
        sizeof(BOOL32)+ 
        sizeof(BOOL32);
    if (packetSize > AcsPacket::GetMaxUserDataLen()) {
        REC_FATAL (11600);
    }
    
    AcsPacket *packetP = new AcsPacket ((size_t)packetSize);

    packetP->SetPrimaryId (appId);
    packetP->SetAppServiceId (1);
    
    theParamIn1.WriteToAcs(*packetP);
    *packetP<<theParamIn2;
    *packetP<<theParamIn3;
    *packetP<<theParamIn4;

    return packetP;
}

AcsPacket *RpcTxXdpRemoteConfigProxy::PackSaveCardTraceConfig2 (
                                                                const UINT8Buffer & theParamIn1,
                                                                const UINT32 theParamIn2,
                                                                const UINT32Array & theParamIn3,
                                                                const UINT32 theParamIn4,
                                                                const UINT32Array & theParamIn5,
                                                                const UINT32 theParamIn6)
{
    UINT32 packetSize = 
        theParamIn1.GetAcsSize()+ 
        sizeof(UINT32)+ 
        theParamIn3.GetAcsSize()+ 
        sizeof(UINT32)+ 
        theParamIn5.GetAcsSize()+ 
        sizeof(UINT32);
    if (packetSize > AcsPacket::GetMaxUserDataLen()) {
        REC_FATAL (11600);
    }
    
    AcsPacket *packetP = new AcsPacket ((size_t)packetSize);

    packetP->SetPrimaryId (appId);
    packetP->SetAppServiceId (2);
    
    theParamIn1.WriteToAcs(*packetP);
    *packetP<<theParamIn2;
    theParamIn3.WriteToAcs(*packetP);
    *packetP<<theParamIn4;
    theParamIn5.WriteToAcs(*packetP);
    *packetP<<theParamIn6;

    return packetP;
}

