/**************************************************************/
// Class: RpcTxXdpRemoteConfigProxy
// File: RpcTxXdpRemoteConfigProxy.h
// Author: ACS API Generator
// Creation Date: Wed Feb  8 07:52:07 2006
//
// Purpose: ACS Tx API for XdpRemoteConfigProxy
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

#ifndef RpcTxXdpRemoteConfigProxy_h
#define RpcTxXdpRemoteConfigProxy_h

#include "basictypes.h"
#include "RWHdr/cstring.h"
#include "UINT32Array.h"
#include "UINT8Buffer.h"
#include "IpAddress.h"
#include "AcsEventPacket.h"
#include "AcsPacket.h"
#include "AcsAppId.h"

class AcsAppInfo;


class RpcTxXdpRemoteConfigProxy {

public:

    static void SaveCardGlobalConfig (
                                      const IpAddress &,
                                      const UINT16,
                                      const BOOL32,
                                      const BOOL32);

    static void SaveCardTraceConfig (
                                     const UINT8Buffer &,
                                     const UINT32,
                                     const UINT32Array &,
                                     const UINT32,
                                     const UINT32Array &,
                                     const UINT32);

    static void Init ();

    static const RWCString ClassName() {return "RpcTxXdpRemoteConfigProxy";}

static AcsAppInfo *GetAcsAppInfoP() { return TheirAppInfoP; };

private:

    static AcsAppId appId;

    static AcsAppInfo *TheirAppInfoP;

    static AcsPacket *PackSaveCardGlobalConfig1 (
                                                 const IpAddress &,
                                                 const UINT16,
                                                 const BOOL32,
                                                 const BOOL32);

    static AcsPacket *PackSaveCardTraceConfig2 (
                                                const UINT8Buffer &,
                                                const UINT32,
                                                const UINT32Array &,
                                                const UINT32,
                                                const UINT32Array &,
                                                const UINT32);

};

#endif // RpcTxXdpRemoteConfigProxy_h
