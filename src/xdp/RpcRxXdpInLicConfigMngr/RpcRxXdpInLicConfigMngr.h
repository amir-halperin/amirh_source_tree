/**************************************************************/
// Class: RpcRxXdpInLicConfigMngr
// File: RpcRxXdpInLicConfigMngr.h
// Author: ACS API Generator
// Creation Date: Wed Feb  8 10:09:24 2006
//
// Purpose: ACS Rx API for XdpInLicConfigMngr
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

#ifndef RpcRxXdpInLicConfigMngr_h
#define RpcRxXdpInLicConfigMngr_h

#include "basictypes.h"
#include "RWHdr/cstring.h"
#include "UINT32Array.h"
#include "UINT8Buffer.h"
#include "IpAddress.h"
#include "AcsPacket.h"
#include "AcsAppId.h"
#include "AcsAppServiceId.h"

class AcsAppInfo;


class RpcRxXdpInLicConfigMngr {

public:

    static void RpcRelRqstInCard (AcsPacket *&);

    static void RpcRqstWithRspInCard (AcsPacket *&,    // Rx Packet
                                      AcsPacket *&    // Reply Packet
                                     );

    static void Init ();

    static const RWCString ClassName() {return "RpcRxXdpInLicConfigMngr";}

static AcsAppInfo *GetAcsAppInfoP() { return TheirAppInfoP; };



private:

    static AcsAppServiceId UnpackFuncType (AcsPacket *);

    static void DownloadGlobalConfig1 (AcsPacket *);

    static void DownloadTraceConfig2 (AcsPacket *);

    static AcsAppId appId;

    static AcsAppInfo *TheirAppInfoP;

};

#endif // RpcRxXdpInLicConfigMngr_h
