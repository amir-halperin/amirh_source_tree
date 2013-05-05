/* XdpInLicConfigMngr.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpInLicConfigMngr
      This class is responsible to send  XDP updated configuration to SFC
      in order to save it to file on flash, and to update XDP objects
      with XDP restored configuration received from the SFC via ACS.

AUTHOR
   Tanya Yakov, Seabridge

		
*************************************************************************/

#ifndef XdpInLicConfigMngr_h
#define XdpInLicConfigMngr_h

#include "basictypes.h"
#include "UINT32Array.h"
#include "UINT8Buffer.h"

#include "XdpConfigMngr.h"

//----------------------------
// Forward Declarations
//----------------------------

// RPC_Phy - Main to Card

class XdpInLicConfigMngr: public XdpConfigMngr {

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------

// Typedef & Enum

// Constructor & Destructor

// Copy constructor & Assign op

// Get methods

    //singelton support - return pointer to itself
    static XdpInLicConfigMngr*  GetIt();

// Set methods

// Miscellaneous Operations

    //singelton support - create itself
    static void Create();

    // RPC_Phy-REG
    static void DownloadGlobalConfig(
                    const IpAddress&,  // Host IP
                    const UINT16,      // Host port
                    const BOOL32,      // Enable output to UDP flag
                    const BOOL32       // Enable output to serial flag
        );

    // RPC_Phy-REG
    static void DownloadTraceConfig(
                    const UINT8Buffer&,    //XDP trace name
                    const UINT32,         //bit map of the app tags enabled by the user
                    const UINT32Array&,   // bit map of the color enalbed by the user for each app tag
                    const UINT32,         //bit map of the app tags enabled by the user
                    const UINT32Array&,   // bit map of the active color to print for each app tag
                    const UINT32          //output channel
       );


    //send updated XDP global configuration to SFC via Acs
    //in order to save it to file on flash
    virtual void SaveGlobalConfig(
                    const IpAddress&  theOutputToUdpHostIpR,       // Host IP
                    const UINT16      theOutputToUdpHostPort,      // Host port
                    const bool        theOutputToUdpEnableFlag,    // Enable output to UDP flag
                    const bool        theOutputToSerialEnableFlag  // Enable output to serial flag
        );

    //send updated XDP trace configuration to SFC via Acs
    //in order to save it to file on flash
    virtual void SaveTraceConfig(
                    const RWCString&   theTraceNameR,         //XDP trace name
                    const UINT32       theEnabledAppTags,     //bit map of the app tags enabled by the user
                    UINT32* const      theEnabledColorsP,      // bit map of the color enalbed by the user for each app tag
                    const UINT32       thePrintActiveAppTags, //bit map of the app tags enabled by the user
                    UINT32* const      thePrintActiveColorsP,  // bit map of the active color to print for each app tag
                    const UINT32       thePrintOutputChannel  //output channel
       );

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
    XdpInLicConfigMngr();

// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

// Operators

// Conv Op

// Data members
    static XdpInLicConfigMngr* TheirSelfP; //singelton support

};

#endif // XdpInLicConfigMngr_h

