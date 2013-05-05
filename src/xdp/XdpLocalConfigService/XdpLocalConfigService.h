/* XdpLocalConfigService.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpLocalConfigService
      This class provides XDP local configuration services.
      This class is responsible to download restored configuration to
      local XDP actual objects (XdpServices, XdpTrace), which means
      that it copies SFC XDP restored configuration
      (from XdpGlobalConfig, XdpTraceConfig) to SFC XDP actual objects
      (XdpServices, XdpTrace).
      In case of LIC it copies LIC XDP restored configuration and
      recived via Acs to LIC XDP actual objects.
      This class is singelton.

AUTHOR
   Tanya Yakov, Seabridge

		
*************************************************************************/

#ifndef XdpLocalConfigService_h
#define XdpLocalConfigService_h

#include "XdpConfigService.h"


//----------------------------
// Forward Declarations
//----------------------------
class XdpTraceContainer;

class XdpLocalConfigService: public XdpConfigService
{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------

// Typedef & Enum

// Constructor & Destructor

// Copy constructor & Assign op

// Get methods

    //singelton support - return pointer to itself
    static XdpLocalConfigService*  GetIt();

// Set methods

// Miscellaneous Operations

    //singelton support - create itself
    static void Create(XdpTraceContainer* theContainerP);


    //download (set) XDP package configuration to local XdpServices object
    virtual void DownloadGlobalConfig(
                    const CardIndex&  theCardIndexR,               //card index
                    const IpAddress&  theOutputToUdpHostIPR,       // Host IP
                    const UINT16      theOutputToUdpHostPort,      // Host port
                    const bool        theOutputToUdpEnableFlag,    // Enable output to UDP flag
                    const bool        theOutputToSerialEnableFlag  // Enable output to serial flag
        ) const;

    //download (set) XDP trace configuration to local XdpTrace object
    virtual void DownloadTraceConfig(
                    const CardIndex&   theCardIndexR,         //card index
                    const RWCString&   theTraceNameR,          //XDP trace name
                    const UINT32       theEnabledAppTags,     //bit map of the app tags enabled by the user
                    const UINT32*      theEnabledColors,      // bit map of the color enalbed by the user for each app tag
                    const UINT32       thePrintActiveAppTags, //bit map of the app tags enabled by the user
                    const UINT32*      thePrintActiveColors,  // bit map of the active color to print for each app tag
                    const UINT32       thePrintOutputChannel  //output channel
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

   XdpLocalConfigService(XdpTraceContainer* theContainerP);

// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

// Operators

// Conv Op

// Data members

    static XdpLocalConfigService*  TheirSelfP; //singelton support

           XdpTraceContainer*      ItsTraceContainerP; //points to XDP trace objects container

};

#endif // XdpLocalConfigService_h

