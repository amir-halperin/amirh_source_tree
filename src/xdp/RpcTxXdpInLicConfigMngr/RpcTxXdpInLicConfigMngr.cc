/**************************************************************/
// Class: RpcTxXdpInLicConfigMngr
// File: RpcTxXdpInLicConfigMngr.cc
// Author: ACS API Generator
// Creation Date: Wed Feb  8 10:09:24 2006
//
// Purpose: ACS Tx API for XdpInLicConfigMngr
//     Rpc_Phy - Mcp to Card
/**************************************************************/

#include "RpcCmnApiTxInMain.h"
#include "AcsAppNameToId.h"
#include "Error.h"
#include "AcsAppInfo.h"
#include "RpcTxXdpInLicConfigMngr.h"

extern const MaskFlagId MASK_RPC_PRN;



AcsAppInfo *RpcTxXdpInLicConfigMngr::TheirAppInfoP = NULL;

AcsAppId RpcTxXdpInLicConfigMngr::appId;

void RpcTxXdpInLicConfigMngr::DownloadGlobalConfig (const CardIndex theCard,
                                                    const IpAddress & theParamIn1,
                                                    const UINT16 theParamIn2,
                                                    const BOOL32 theParamIn3,
                                                    const BOOL32 theParamIn4)
{


    if ( TheirAppInfoP->IsTraceEnable(1) ) {

        RN_TRACE (MASK_RPC_PRN, "------------------------------");
        RN_TRACE (MASK_RPC_PRN, "RpcTxXdpInLicConfigMngr::DownloadGlobalConfig called. In Param:");
        RN_TRACE (MASK_RPC_PRN, "CardIndex: " << theCard);
        RN_TRACE (MASK_RPC_PRN, "IpAddress: " << theParamIn1);
        RN_TRACE (MASK_RPC_PRN, "UINT16: " << theParamIn2);
        RN_TRACE (MASK_RPC_PRN, "BOOL32: " << theParamIn3);
        RN_TRACE (MASK_RPC_PRN, "BOOL32: " << theParamIn4);
        RN_TRACE (MASK_RPC_PRN, "------------------------------");
    }

    AcsPacket *txPacketP = PackDownloadGlobalConfig1 (
        theParamIn1,
        theParamIn2,
        theParamIn3,
        theParamIn4);

    RpcCmnApiTxInMain::TheRegularRpc()->ReliableRequest(theCard, txPacketP);
}

void RpcTxXdpInLicConfigMngr::DownloadTraceConfig (const CardIndex theCard,
                                                   const UINT8Buffer & theParamIn1,
                                                   const UINT32 theParamIn2,
                                                   const UINT32Array & theParamIn3,
                                                   const UINT32 theParamIn4,
                                                   const UINT32Array & theParamIn5,
                                                   const UINT32 theParamIn6)
{


    if ( TheirAppInfoP->IsTraceEnable(2) ) {

        RN_TRACE (MASK_RPC_PRN, "------------------------------");
        RN_TRACE (MASK_RPC_PRN, "RpcTxXdpInLicConfigMngr::DownloadTraceConfig called. In Param:");
        RN_TRACE (MASK_RPC_PRN, "CardIndex: " << theCard);
        RN_TRACE (MASK_RPC_PRN, "UINT8Buffer: " << theParamIn1);
        RN_TRACE (MASK_RPC_PRN, "UINT32: " << theParamIn2);
        RN_TRACE (MASK_RPC_PRN, "UINT32Array: " << theParamIn3);
        RN_TRACE (MASK_RPC_PRN, "UINT32: " << theParamIn4);
        RN_TRACE (MASK_RPC_PRN, "UINT32Array: " << theParamIn5);
        RN_TRACE (MASK_RPC_PRN, "UINT32: " << theParamIn6);
        RN_TRACE (MASK_RPC_PRN, "------------------------------");
    }

    AcsPacket *txPacketP = PackDownloadTraceConfig2 (
        theParamIn1,
        theParamIn2,
        theParamIn3,
        theParamIn4,
        theParamIn5,
        theParamIn6);

    RpcCmnApiTxInMain::TheRegularRpc()->ReliableRequest(theCard, txPacketP);
}

void RpcTxXdpInLicConfigMngr::Init() {
    appId = AcsAppNameToId::TheNameToId()->GetId("RPC_XdpInLicConfigMngr");

    if (TheirAppInfoP == NULL) {
        TheirAppInfoP = AcsAppInfo::Create( "XdpInLicConfigMngr" , "RPC" , true , MASK_RPC_PRN );

    }

}

AcsPacket *RpcTxXdpInLicConfigMngr::PackDownloadGlobalConfig1 (
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

AcsPacket *RpcTxXdpInLicConfigMngr::PackDownloadTraceConfig2 (
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

