/* XdpRemoteConfigProxy.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpRemoteConfigProxy
      This class is responsible to receive XDP configuration from Acs
      as part of XDP configuration save scenario and call the XdpInSfcConfigMngr
      in order to set it to XdpGlobalConfig, XdpTraceConfig
      objects and save it to file.

AUTHOR
  Tanya Yakov, Seabridge

		
*************************************************************************/

#ifndef XdpRemoteConfigProxy_h
#define XdpRemoteConfigProxy_h

#include "basictypes.h"

#include "CardIndex.h"
#include "IpAddress.h"
#include "UINT32Array.h"
#include "UINT8Buffer.h"

// RPC_Phy - Card to Main

//----------------------------
// Forward Declarations
//----------------------------
class XdpInSfcConfigMngr;


class XdpRemoteConfigProxy
{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------

// Typedef & Enum

// Constructor & Destructor

// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

  static void Create(XdpInSfcConfigMngr* theConfigMngrP); //singelton support

  // RPC_Phy
  static void SaveCardGlobalConfig(const CardIndex,
       						       const IpAddress&, //output to UDP host IP
       						       const UINT16,     //output to UDP host port
       						       const BOOL32,     //output to UDP enable flag
       						       const BOOL32);    //output to serial enable flag
  // RPC_Phy
  static void SaveCardTraceConfig(const CardIndex,
                                  const UINT8Buffer&,  //XDP trace name
                                  const UINT32,       //enabled app tags bit map
                                  const UINT32Array&, //enabled colors bit map per app tag
                                  const UINT32,       //print enabled app tags bit map
                                  const UINT32Array&, //print enabled colors bit map per app tag
                                  const UINT32);      //output channel




// Operators

// Conv Op

// RTTI


//-------------------------------------------------------------------------
protected:
//-------------------------------------------------------------------------

// Typedef & Enum

// Constructor & Destructor

// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

// Operators

// Conv Op

// Data members

//-------------------------------------------------------------------------
private:
//-------------------------------------------------------------------------

// Typedef & Enum

// Constructor & Destructor
    XdpRemoteConfigProxy(XdpInSfcConfigMngr* theConfigMngrP);

// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

// Operators

// Conv Op

// Data members

    static XdpRemoteConfigProxy* TheirSelfP;

    static XdpInSfcConfigMngr*   TheirConfigMngrP;

};

#endif // XdpRemoteConfigProxy_h

