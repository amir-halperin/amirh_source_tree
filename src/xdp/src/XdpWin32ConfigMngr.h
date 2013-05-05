/* XdpWin32ConfigMngr.h   */

/*************************************************************************
*  2006  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpWin32ConfigMngr
   Derive from XdpConfigMngr to implements provided abstractions
   for Windows environment.

AUTHOR
   Amir Halperin, Seabridge

		
*************************************************************************/

#ifndef XdpWin32ConfigMngr_h
#define XdpWin32ConfigMngr_h

#include "basictypes.h"
#include "RWHdr/cstring.h"
#include "XdpConfigMngr.h"

//----------------------------
// Forward Declarations
//----------------------------
class IpAddress;

class XdpWin32ConfigMngr : public XdpConfigMngr
{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------

	static XdpWin32ConfigMngr& Instance() {static XdpWin32ConfigMngr _instance; return _instance;}

    /**
    * save XDP global configuration request (called by XdpServices)
	* Do nothing under Win32 environment
	* @param theOutputToUdpHostIpR Host IP
	* @param theOutputToUdpHostPort Host port
	* @param theOutputToUdpEnableFlag UDP output enable/disable flag
	* @param theOutputToSerialEnableFlag Serial output enable/disable flag
	*/
    virtual void SaveGlobalConfig(
                    const IpAddress&  theOutputToUdpHostIpR,       // Host IP
                    const UINT16      theOutputToUdpHostPort,      // Host port
                    const bool        theOutputToUdpEnableFlag,    // Enable output to UDP flag
                    const bool        theOutputToSerialEnableFlag  // Enable output to serial flag
					){/*Do nothing*/}

    //save XDP trace configuration request (called by XdpTrace)
    virtual void SaveTraceConfig(
                    const RWCString&   theTraceNameR,         //XDP trace name
                    const UINT32       theEnabledAppTags,     //bit map of the app tags enabled by the user
                    UINT32* const     theEnabledColorsP,     // bit map of the color enalbed by the user for each app tag
                    const UINT32       thePrintActiveAppTags, //bit map of the app tags enabled by the user
                    UINT32* const     thePrintActiveColorsP, // bit map of the active color to print for each app tag
                    const UINT32       thePrintOutputChannel  //output channel
					) {/*Do nothing*/};

//-------------------------------------------------------------------------
private:
//-------------------------------------------------------------------------

    /**
    * Constructor
    */
    XdpWin32ConfigMngr() : XdpConfigMngr() {TheirSelfP = this;}  
};

#endif // XdpWin32ConfigMngr_h

