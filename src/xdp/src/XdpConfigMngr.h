/* XdpConfigMngr.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpConfigMngr
   This class holds local XdpTrace objects container.
   It is abstract base class.
   This class is responsible to register local XdpTrace objects.

AUTHOR
   Tanya Yakov, Seabridge

		
*************************************************************************/

#ifndef XdpConfigMngr_h
#define XdpConfigMngr_h

#include "basictypes.h"
#include "CardIndex.h"
#include "IpAddress.h"
#include "RWHdr/cstring.h"



//----------------------------
// Forward Declarations
//----------------------------
class XdpTraceContainer;
class XdpTrace;

class XdpConfigMngr
{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------

// Typedef & Enum

// Constructor & Destructor

// Copy constructor & Assign op

// Get methods

    //singelton support - return pointer to itself
    static XdpConfigMngr*  GetIt();

// Set methods

// Miscellaneous Operations


    //save XDP global configuration request (called by XdpServices)
    virtual void SaveGlobalConfig(
                    const IpAddress&  theOutputToUdpHostIpR,       // Host IP
                    const UINT16      theOutputToUdpHostPort,      // Host port
                    const bool        theOutputToUdpEnableFlag,    // Enable output to UDP flag
                    const bool        theOutputToSerialEnableFlag  // Enable output to serial flag
        ) = 0;

    //save XDP trace configuration request (called by XdpTrace)
    virtual void SaveTraceConfig(
                    const RWCString&   theTraceNameR,         //XDP trace name
                    const UINT32       theEnabledAppTags,     //bit map of the app tags enabled by the user
                    UINT32* const     theEnabledColorsP,     // bit map of the color enalbed by the user for each app tag
                    const UINT32       thePrintActiveAppTags, //bit map of the app tags enabled by the user
                    UINT32* const     thePrintActiveColorsP, // bit map of the active color to print for each app tag
                    const UINT32       thePrintOutputChannel  //output channel
       ) = 0;

    void RegisterTrace(XdpTrace *theTraceP);
// Operators

// Conv Op

// RTTI
    //virtual void PrintGlobals();
    //virtual void PrintFiles();
    //virtual void DeleteFiles();
//-------------------------------------------------------------------------
protected:
//-------------------------------------------------------------------------

// Typedef & Enum

// Constructor & Destructor

    XdpConfigMngr();

// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

// Operators

// Conv Op

// Data members
      static XdpConfigMngr*      TheirSelfP; //singelton support

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

           XdpTraceContainer *ItsTraceContainerP;

};

#endif // XdpConfigMngr_h

