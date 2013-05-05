/* XdpMngr.cc */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpMngr
   
    This class is responsible to init and configure all the XDP classes.

    The application should call the init functions when the function 
    preconditions are meat.

    Applications that require to change the configuration should derived
    from this class and make insert their changes in the specific functions.

    Note that most of the configuration can not be changed after init is done

    All calls should be made from the same task, or atleast without preemption

AUTHOR
   Gaash, SB


**************************************************************************/

#include "XdpMngr.h"
#include "XdpServices.h"
#include "XdpMngrCmdMngr.h"

#include "stdio.h"
#include "Error.h"
#include "XdpConv.h"

// Specific includes

#ifdef RN_WIN32
#include <time.h>
#endif

extern const MaskFlagId MASK_XDP;


// ============================================================================
// ===
// === Statics
// ===
// ============================================================================

XdpMngr *XdpMngr::TheirP = NULL;

// ============================================================================
// ===
// === Common functions
// ===
// ============================================================================

// --- ctor -------------------------------------------------------------------
//
// Init all the variable to their defaults
//
// To change a value, use the xxxSetxxx functions, don't modify this code
//
XdpMngr::XdpMngr() {

    RN_TRACE(MASK_XDP,"::XdpMngr (" + XdpConv::ToHexStr(this) + ") - ctor begin");

    // Make sure the object is created exactly once
    if (TheirP != NULL) {
        REC_FATAL_1P(111,(UINT32)TheirP);
    }

    // Save the object pointer
    TheirP = this;


}

// --- GetIt ------------------------------------------------------------------
//
// Return pointer to the XdpService object. Create it if it is not exist.
//
// Standard single-tone implemetaion
//
// Never returns NULL
//
XdpMngr *XdpMngr::GetIt() {
    
    // Check object already exists
    if (TheirP == NULL) {
        
        // Not exists, error
        REC_FATAL(111);
    }


    // Object exists, return it
    return TheirP;

}

// ============================================================================
// ===
// === Create & Start functions
// ===
// ============================================================================

// --- CreateIndependent ------------------------------------------------------
//
// Create independent XDP classes. Independent class are classes that does not
// required any other non-XDP class (application class) to be init-ed
//
// StartIndependent starts the configured classes. Note that most of the 
// configutaion can not be changed after it has been started.
//
// Call those function as soon as possible
//
// Application configutaion might be done between the create and the start 
// functions.
//
// The virutal specific creation function is called at the end of the creation 
// function to enabled application specific configurations
//
void XdpMngr::CreateIndependent(){

    RN_TRACE(MASK_XDP,"::CreateIndependent");

    // Create XDP services 

    XdpServices *xdpServices = new XdpServices;

    // Config the XDP Output to UDP service

    OutputToUdpSetIP(IpAddress("172.30.1.66")); // baracuda
	 
    IpPortNumber portNumber = 5999;

    OutputToUdpSetPort(portNumber);

    OutputToUdpSetTaskPriority(203);
    OutputToUdpSetQueueSize(50);
    OutputToUdpSetMaxMsgSize(1500);

    // Enable output to UDP 
	//iko: disable by default since if need default gateway & inband ip is used
	//then loop of messages is created and inband queue is full
	//the user should enable manually if want udp traces. 
    OutputToUdpEnable(false);


    // Config the XDP Output to serial service

    OutputToSerialSetTaskPriority(204);
    OutputToSerialSetQueueSize(100);
    OutputToSerialSetMaxMsgSize(300);

    // Enable output to serial
    OutputToSerialEnable(true);

    // Config time stamp service
#ifdef RN_WIN32
    TimeStampSetFrequency(CLOCKS_PER_SEC);
#endif

#ifdef RN_CPU_I960
    TimeStampSetFrequency(1000); // With 960, Using card time, the counter is of mSec
#endif

    // Call the specific creation function
    SpecificCreateIndependent();

    // Create XDP command manager

    XdpMngrCmdMngr *xdpMngrCmdMngr = new XdpMngrCmdMngr;

    // All done
}
// --- StartIndependent -------------------------------------------------------
//
// Set CreateIndependent
//

void XdpMngr::StartIndependent() {

    RN_TRACE(MASK_XDP,"::StartIndependent");

    // Get XDP services 

    XdpServices *xdpServices = XdpServices::GetIt();

    // Start the XDP Output to UDP service

    xdpServices->OutputToUdpStart();

    // Start the XDP Output to serial service

    xdpServices->OutputToSerialStart();
}

// ============================================================================
// ===
// === Output to UDP service functions
// ===
// ============================================================================

// --- OutputToUdpSetIP -------------------------------------------------------
//
// Set to IP address of Output to UDP service
//
// In case of failure an fatal error is generated
//
void XdpMngr::OutputToUdpSetIP(const IpAddress &theIP) {

    IpAddress tmpIP = theIP; // Get ride of const reference

    RN_TRACE(MASK_XDP,"XdpMngr::OutputToUdpSetIP - Set IP " + tmpIP.ToStr() );

    bool rc;

    rc = XdpServices::GetIt()->OutputToUdpSetIP(theIP);
    if (rc == false) {
        REC_FATAL(17506);
    }

}

// --- OutputToUdpSetPort -----------------------------------------------------
//
// Set to IP port of Output to UDP service
//
// In case of failure an fatal error is generated
//
void XdpMngr::OutputToUdpSetPort(const IpPortNumber &thePort) {

    RN_TRACE(MASK_XDP,"XdpMngr::OutputToUdpSetPort - Set port to " + XdpConv::ToStr(thePort) );

    bool rc;

    rc = XdpServices::GetIt()->OutputToUdpSetPort(thePort);
    if (rc == false) {
        REC_FATAL(17506);
    }

}

// --- OutputToUdpSetTaskPriority ---------------------------------------------
//
// Set to task prioity of Output to UDP service
//
// In case of failure an fatal error is generated
//
void XdpMngr::OutputToUdpSetTaskPriority(UINT32 theTaskPriority) {

    RN_TRACE(MASK_XDP,"XdpMngr::OutputToUdpTaskPriority - Set task priority to " + 
                      XdpConv::ToStr(theTaskPriority) );

    bool rc;

    rc = XdpServices::GetIt()->OutputToUdpSetTaskPriority(theTaskPriority);
    if (rc == false) {
        REC_FATAL(17506);
    }

}
// --- OutputToUdpSetQueueSize ------------------------------------------------
//
// Set to queue size of Output to UDP service
//
// In case of failure an fatal error is generated
//
void XdpMngr::OutputToUdpSetQueueSize(UINT32 theQueueSize) {

    RN_TRACE(MASK_XDP,"XdpMngr::OutputToUdpQueueSize - Set queue size to " + XdpConv::ToStr(theQueueSize) );

    bool rc;

    rc = XdpServices::GetIt()->OutputToUdpSetQueueSize(theQueueSize);
    if (rc == false) {
        REC_FATAL(17506);
    }

}

// --- OutputToUdpMaxMsgSize --------------------------------------------------
//
// Set max message size of Output to UDP service
//
// In case of failure an fatal error is generated
//
void XdpMngr::OutputToUdpSetMaxMsgSize(UINT32 MaxMsgSize) {

    RN_TRACE(MASK_XDP,"XdpMngr::OutputToUdpMaxMsgSize - Set max message size to " 
                       + XdpConv::ToStr(MaxMsgSize) );

    bool rc;

    rc = XdpServices::GetIt()->OutputToUdpSetMaxMsgSize(MaxMsgSize);
    if (rc == false) {
        REC_FATAL(17506);
    }

}
// --- OutputToUdpEnable ------------------------------------------------------
//
// Set enable (true) / disable (false) Output to UDP service
//
// In case of failure an fatal error is generated
//
void XdpMngr::OutputToUdpEnable(bool theEnable) {

    RN_TRACE(MASK_XDP,"XdpMngr::OutputToUdpEnable - Set enable to " + XdpConv::ToStr(theEnable) );

    bool rc;

    rc = XdpServices::GetIt()->OutputToUdpEnable(theEnable);
    if (rc == false) {
        REC_FATAL(17506);
    }

}


// ============================================================================
// ===
// === Output to serial service functions
// ===
// ============================================================================

// --- OutputToSerialSetTaskPriority ---------------------------------------------
//
// Set to task prioity of Output to serial service
//
// In case of failure an fatal error is generated
//
void XdpMngr::OutputToSerialSetTaskPriority(UINT32 theTaskPriority) {

    RN_TRACE(MASK_XDP,"XdpMngr::OutputToSerialTaskPriority - Set task priority to " + 
                      XdpConv::ToStr(theTaskPriority) );

    bool rc;

    rc = XdpServices::GetIt()->OutputToSerialSetTaskPriority(theTaskPriority);
    if (rc == false) {
        REC_FATAL(17509);
    }

}

// --- OutputToSerialSetQueueSize ------------------------------------------------
//
// Set to queue size of Output to serial service
//
// In case of failure an fatal error is generated
//
void XdpMngr::OutputToSerialSetQueueSize(UINT32 theQueueSize) {

    RN_TRACE(MASK_XDP,"XdpMngr::OutputToSerialQueueSize - Set queue size to " + XdpConv::ToStr(theQueueSize) );

    bool rc;

    rc = XdpServices::GetIt()->OutputToSerialSetQueueSize(theQueueSize);
    if (rc == false) {
        REC_FATAL(17509);
    }

}

// --- OutputToSerialMaxMsgSize --------------------------------------------------
//
// Set max message size of Output to serial service
//
// In case of failure an fatal error is generated
//
void XdpMngr::OutputToSerialSetMaxMsgSize(UINT32 MaxMsgSize) {

    RN_TRACE(MASK_XDP,"XdpMngr::OutputToSerialMaxMsgSize - Set max message size to " 
                       + XdpConv::ToStr(MaxMsgSize) );

    bool rc;

    rc = XdpServices::GetIt()->OutputToSerialSetMaxMsgSize(MaxMsgSize);
    if (rc == false) {
        REC_FATAL(17509);
    }

}
// --- OutputToSerialEnable ------------------------------------------------------
//
// Set enable (true) / disable (false) Output to serial service
//
// In case of failure an fatal error is generated
//
void XdpMngr::OutputToSerialEnable(bool theEnable) {

    RN_TRACE(MASK_XDP,"XdpMngr::OutputToSerialEnable - Set enable to " + XdpConv::ToStr(theEnable) );

    bool rc;

    rc = XdpServices::GetIt()->OutputToSerialEnable(theEnable);
    if (rc == false) {
        REC_FATAL(17509);
    }

}

// ============================================================================
// ===
// === Time stamp service functions
// ===
// ============================================================================

// --- TimeStampSetFrequency --------------------------------------------------
//
// Set time stamp counter frequency in Hz
//
// In case of failure an fatal error is generated
//
void XdpMngr::TimeStampSetFrequency(double theFrequency) {

    RN_TRACE(MASK_XDP,"XdpMngr::TimeStampSetFrequency - Set frequency to " + 
                      XdpConv::ToUnitsStr(theFrequency) + "Hz" );

    bool rc;

    rc = XdpServices::GetIt()->TimeStampSetFrequency(theFrequency);
    if (rc == false) {
        REC_FATAL(17511);
    }

}


// ============================================================================
// ===
// === WIN32 support functions functions
// ===
// ============================================================================

#ifdef RN_WIN32



#endif // RN_WIN32
