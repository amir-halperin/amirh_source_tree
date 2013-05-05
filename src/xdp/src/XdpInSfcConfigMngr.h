/* XdpInSfcConfigMngr.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpInSfcConfigMngr
     This class manages configuration saving and restoration process.
     It holds SFC and LICs XDP package and trace configuration using
     XdpGlobalConfig objects.

AUTHOR
  Tanya Yakov, Seabridge

		
*************************************************************************/

#ifndef XdpInSfcConfigMngr_h
#define XdpInSfcConfigMngr_h


#include "XdpConfigMngr.h"

#include "SfcPPESystemDefs.h"
#include "PTaskWithID.h"
#include "PQueue.h"

//----------------------------
// Forward Declarations
//----------------------------
class XdpGlobalConfig;
class XdpTrace;

class XdpInSfcConfigMngr: public XdpConfigMngr
{

    friend class XdpRemoteConfigProxy;

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------

// Typedef & Enum

// Constructor & Destructor

// Copy constructor & Assign op

// Get methods

    //singelton support - return pointer to itself
    static XdpInSfcConfigMngr*  GetIt();

// Set methods

// Miscellaneous Operations

    //singelton support - create itself
    static void Create();

    //restore XDP configuration from file to storage
    void RestoreConfig();

    //download XDP configuration from the storage to actual objects
    void DownloadConfig(const CardIndex&  theCardIndexR);

    //create XDP save configuration queue and task, start the task
    void SaveConfigStart();

    //get SFC CardIndex and call SaveCardGlobalConfig method in order to save
    //XDP package updated configuration to flash
    virtual void SaveGlobalConfig(
                    const IpAddress&  theOutputToUdpHostIpR,       // Host IP
                    const UINT16      theOutputToUdpHostPort,      // Host port
                    const bool        theOutputToUdpEnableFlag,    // Enable output to UDP flag
                    const bool        theOutputToSerialEnableFlag  // Enable output to serial flag
        );

    //get SFC CardIndex and call SaveCardGlobalConfig method in order to save
    //XDP trace updated configuration to flash
    virtual void SaveTraceConfig(
                    const RWCString&   theTraceNameR,         //XDP trace name
                    const UINT32       theEnabledAppTags,     //bit map of the app tags enabled by the user
                    UINT32* const     theEnabledColorsP,     // bit map of the color enalbed by the user for each app tag
                    const UINT32       thePrintActiveAppTags, //bit map of the app tags enabled by the user
                    UINT32* const      thePrintActiveColorsP, // bit map of the active color to print for each app tag
                    const UINT32       thePrintOutputChannel  //output channel
       );

// Operators

// Conv Op

// RTTI

//test amit

    void PrintFiles();

    void PrintGlobals();

    void DeleteFiles();


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
    const UINT32 XDP_NUM_OF_CARDS = ATMUX_NUM_OF_IO_PHY_SLOTS + 1; //14 LICs + SFC

   enum XdpOperationType {
      SAVE_XDP_GLOBAL_CONFIG,
      SAVE_XDP_TRACE_CONFIG,
   };


// Constructor & Destructor
    XdpInSfcConfigMngr();

// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

    static void SaveConfigTaskHandler();

    void SaveConfigTaskBody();

    //update XdpGlobalConfig with updated configuration and
    //send request to the "XDPS" task in order to save configuration to file on flash
    void SaveCardGlobalConfig(
                    const CardIndex&  theCardIndexR,               //card index
                    const IpAddress&  theOutputToUdpHostIpR,       // Host IP
                    const UINT16      theOutputToUdpHostPort,      // Host port
                    const bool        theOutputToUdpEnableFlag,    // Enable output to UDP flag
                    const bool        theOutputToSerialEnableFlag  // Enable output to serial flag
        );

    //update XdpTraceConfig with updated configuration and
    //send request to the "XDPS" task in order to save configuration to file on flash
    void SaveCardTraceConfig(
                    const CardIndex&   theCardIndexR,         //card index
                    const RWCString&   theTraceNameR,         //XDP trace name
                    const UINT32       theEnabledAppTags,     //bit map of the app tags enabled by the user
                    UINT32* const     theEnabledColors,      // bit map of the color enalbed by the user for each app tag
                    const UINT32       thePrintActiveAppTags, //bit map of the app tags enabled by the user
                    UINT32* const     thePrintActiveColors,  // bit map of the active color to print for each app tag
                    const UINT32       thePrintOutputChannel  //output channel
       );

// Operators

// Conv Op

// Data members

    static XdpInSfcConfigMngr* TheirSelfP; //singelton support

    //XDP save configuration task related data members
    PTaskWithID  ItsSaveConfigTask;          // XDP save config task
    PQueue       ItsSaveConfigQueue;         // XDP save config message queue


    //XDP configuration storage
    XdpGlobalConfig*  ItsConfigArr[XDP_NUM_OF_CARDS];

};

#endif // XdpInSfcConfigMngr_h

