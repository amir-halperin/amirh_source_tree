/* XdpRemoteConfigService.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpRemoteConfigService
      This class provides XDP local configuration services.
      This class is responsible to download restored configuration to
      remote XDP actual objects (XdpServices, XdpTrace), which means
      that it sends LIC XDP restored configuration from SFC to LIC using Acs.
      This class is singelton.

AUTHOR
   Tanya Yakov, Seabridge

		
*************************************************************************/

#ifndef XdpRemoteConfigService_h
#define XdpRemoteConfigService_h

#include "XdpConfigService.h"

//----------------------------
// Forward Declarations
//----------------------------

class XdpRemoteConfigService: public XdpConfigService
{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------

// Typedef & Enum

// Constructor & Destructor

// Copy constructor & Assign op

// Get methods

    //singelton support - return pointer to itself
    static XdpRemoteConfigService*  GetIt();

    //singelton support - create itself
    static void Create();


// Set methods

// Miscellaneous Operations

    virtual void DownloadGlobalConfig(
                    const CardIndex&  theCardIndexR,               //card index
                    const IpAddress&  theOutputToUdpHostIPR,       // Host IP
                    const UINT16      theOutputToUdpHostPort,      // Host port
                    const bool        theOutputToUdpEnableFlag,    // Enable output to UDP flag
                    const bool        theOutputToSerialEnableFlag // Enable output to serial flag
        ) const;

    virtual void DownloadTraceConfig(
                    const CardIndex&   theCardIndexR,         //card index
                    const RWCString&   theTraceNameR,          //XDP trace name
                    const UINT32       theEnabledAppTags,     //bit map of the app tags enabled by the user
                    const UINT32*      theEnabledColors,      // bit map of the color enalbed by the user for each app tag
                    const UINT32       thePrintActiveAppTags, //bit map of the app tags enabled by the user
                    const UINT32*      thePrintActiveColors,  // bit map of the active color to print for each app tag
                    const UINT32       thePrintOutputChannel //output channel
       ) const;

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
    XdpRemoteConfigService();

// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

// Operators

// Conv Op

// Data members
     static XdpRemoteConfigService*  TheirSelfP; //singelton support

};

#endif // XdpRemoteConfigService_h

