/**************************************************************/
// Class: RpcRxXdpRemoteConfigProxy
// File: RpcRxXdpRemoteConfigProxy.h
// Author: ACS API Generator
// Creation Date: Wed Feb  8 07:52:07 2006
//
// Purpose: ACS Rx API for XdpRemoteConfigProxy
//     Rpc_Phy - Card to Mcp
//
// Class Invariant: None.
//
// Copy Semantics: 
//		The ACS API can not be created or copied.
//
// Instantiation semantics:
//		The ACS API can not be created or copied.
/**************************************************************/

#ifndef RpcRxXdpRemoteConfigProxy_h
#define RpcRxXdpRemoteConfigProxy_h

#include "basictypes.h"
#include "RWHdr/cstring.h"
#include "UINT32Array.h"
#include "UINT8Buffer.h"
#include "IpAddress.h"
#include "CardIndex.h"
#include "AcsPacket.h"
#include "AcsAppId.h"
#include "AcsAppServiceId.h"

class AcsAppInfo;


class RpcRxXdpRemoteConfigProxy {

public:

    static void RpcPhyRelRqstInMain (const CardIndex, AcsPacket *&);

    static void RpcPhyRqstWithRspInMain (const CardIndex,
                                      AcsPacket *&,    // Rx Packet
                                      AcsPacket *&    // Reply Packet
                                     );

    static void Init ();

    static const RWCString ClassName() {return "RpcRxXdpRemoteConfigProxy";}

static AcsAppInfo *GetAcsAppInfoP() { return TheirAppInfoP; };



private:

    static AcsAppServiceId UnpackFuncType (AcsPacket *);

    static void SaveCardGlobalConfig1 (const CardIndex, AcsPacket *);

    static void SaveCardTraceConfig2 (const CardIndex, AcsPacket *);

    static AcsAppId appId;

    static AcsAppInfo *TheirAppInfoP;

};

#endif // RpcRxXdpRemoteConfigProxy_h
