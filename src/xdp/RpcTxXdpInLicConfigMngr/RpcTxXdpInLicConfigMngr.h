/**************************************************************/
// Class: RpcTxXdpInLicConfigMngr
// File: RpcTxXdpInLicConfigMngr.h
// Author: ACS API Generator
// Creation Date: Wed Feb  8 10:09:24 2006
//
// Purpose: ACS Tx API for XdpInLicConfigMngr
//     Rpc_Phy - Mcp to Card
//
// Class Invariant: None.
//
// Copy Semantics: 
//		The ACS API can not be created or copied.
//
// Instantiation semantics:
//		The ACS API can not be created or copied.
/**************************************************************/

#ifndef RpcTxXdpInLicConfigMngr_h
#define RpcTxXdpInLicConfigMngr_h

#include "basictypes.h"
#include "RWHdr/cstring.h"
#include "UINT32Array.h"
#include "UINT8Buffer.h"
#include "IpAddress.h"
#include "CardIndex.h"
#include "AcsPacket.h"
#include "AcsAppId.h"

class AcsAppInfo;


class RpcTxXdpInLicConfigMngr {

public:

    static void DownloadGlobalConfig (const CardIndex,
                                      const IpAddress &,
                                      const UINT16,
                                      const BOOL32,
                                      const BOOL32);

    static void DownloadTraceConfig (const CardIndex,
                                     const UINT8Buffer &,
                                     const UINT32,
                                     const UINT32Array &,
                                     const UINT32,
                                     const UINT32Array &,
                                     const UINT32);

    static void Init ();

    static const RWCString ClassName() {return "RpcTxXdpInLicConfigMngr";}

static AcsAppInfo *GetAcsAppInfoP() { return TheirAppInfoP; };

private:

    static AcsAppId appId;

    static AcsAppInfo *TheirAppInfoP;

    static AcsPacket *PackDownloadGlobalConfig1 (
                                                 const IpAddress &,
                                                 const UINT16,
                                                 const BOOL32,
                                                 const BOOL32);

    static AcsPacket *PackDownloadTraceConfig2 (
                                                const UINT8Buffer &,
                                                const UINT32,
                                                const UINT32Array &,
                                                const UINT32,
                                                const UINT32Array &,
                                                const UINT32);

};

#endif // RpcTxXdpInLicConfigMngr_h
