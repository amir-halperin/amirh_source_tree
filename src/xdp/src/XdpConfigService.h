/* XdpConfigService.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpConfigService
     This is XDP configuration service abstract base class.
     This class is responsible to download XDP restored
     configuration to local or remote XDP objects.

AUTHOR
   Tanya Yakov, Seabridge

		
*************************************************************************/

#ifndef XdpConfigService_h
#define XdpConfigService_h

#include "basictypes.h"
#include "CardIndex.h"
#include "IpAddress.h"
#include "UINT32Array.h"


//----------------------------
// Forward Declarations
//----------------------------

class XdpConfigService
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

    virtual void DownloadGlobalConfig(
                    const CardIndex&  theCardIndexR,               //card index
                    const IpAddress&  theOutputToUdpHostIPR,       // Host IP
                    const UINT16      theOutputToUdpHostPort,      // Host port
                    const bool        theOutputToUdpEnableFlag,    // Enable output to UDP flag
                    const bool        theOutputToSerialEnableFlag  // Enable output to serial flag
        ) const = 0;

    virtual void DownloadTraceConfig(
                    const CardIndex&   theCardIndexR,         //card index
                    const RWCString&   theTraceNameR,         //XDP trace name
                    const UINT32       theEnabledAppTags,     //bit map of the app tags enabled by the user
                    const UINT32*      theEnabledColorsP,     // bit map of the color enalbed by the user for each app tag
                    const UINT32       thePrintActiveAppTags, //bit map of the app tags enabled by the user
                    const UINT32*      thePrintActiveColorsP, // bit map of the active color to print for each app tag
                    const UINT32       thePrintOutputChannel  //output channel
       ) const = 0;


// Operators

// Conv Op

// RTTI


//-------------------------------------------------------------------------
protected:
//-------------------------------------------------------------------------

// Typedef & Enum

// Constructor & Destructor

   XdpConfigService() {};

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

// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

// Operators

// Conv Op

// Data members

};

#endif // XdpConfigService_h

