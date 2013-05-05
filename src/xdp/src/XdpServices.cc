/* XdpServices.cc  */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpServices

    This class provide system services like printing to other XDP classes.
    It is intened to be use primarialy by XDP classes.

    This class hides from the XDP classes the differenet OS-es and CPU-s.
    Therefore it has quite a lot of code under #ifdefs

    The provided services are:

    1. Output to UDP (monserver)
    2. Output to serial port
    3. Interrrupt services (enable/restore/isInside)
    4. Get stamp time service
    5. Get task service

AUTHOR
   Gaash, SB


**************************************************************************/

#include "XdpServices.h"
#include "prepc.h"
#include "Error.h"
#include "XdpConv.h"
#include "SerialPrint.h"

#include "XdpConfigMngr.h"

// --- specific includes ------------------------------------------------------

// --- specific includes - WIN32
#ifdef RN_WIN32

#include <time.h>

#endif // RN_WIN32

// --- specific includes - PPC

// --- specific includes - 960
#ifdef RN_CPU_I960
#include "CardTime.h"
#endif

#define TIME_STAMP_PARAM_FOR_MICROSEC 8
#define TIME_STAMP_PARAM_FOR_MILLISEC 18

extern const MaskFlagId MASK_XDP;


// ============================================================================
// ===
// === Statics
// ===
// ============================================================================

XdpServices *XdpServices::TheirP = NULL;

#ifdef RN_CPU_I960
HW_UINT32 *XdpServices::TheirTimeStampCounterP = NULL;
#endif

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
XdpServices::XdpServices() {

    RN_TRACE(MASK_XDP,"::XdpServices (" + XdpConv::ToHexStr(this) + ") - ctor begin");

    // Check object already exists
    if (TheirP != NULL) {
        // Object already created
        REC_FATAL(17503);
    }

    // Output to UDP

    ItsOutputToUdpHostIP       = IpAddress("172.30.1.66"); // Host IP - baracuda

    ItsOutputToUdpHostPort     = 5999;    // Host port
    ItsOutputToUdpSocket       = -1;      // Socket number

    ItsOutoutToUdpTaskPriority = 203;     // Task priority

    ItsOutputToUdpQueueSize    = 50;      // Message queue size (# of msgs)
    ItsOutputToUdpMaxMsgSize   = 1500;    // Max message size

    ItsOutputToUdpEnableFlag   = false;   // Enable output to UDP flag
    ItsOutputToUdpStartFlag    = false;   // Start flag, also disables cfg changes

    ItsOutputToUdpQueueSendCounter          = 0; // Send to queue counter
    ItsOutputToUdpQueueRcvCounter           = 0; // Receive from queue counter
    ItsOutputToUdpTooLongCounter            = 0; // String length > MaxMsgSize
    ItsOutputToUdpDiscardDisableCounter     = 0; // Discard due to disabled
    ItsOutputToUdpDiscardNoStartCounter     = 0; // Discard due to not started
    ItsOutputToUdpDiscardQueueErrorCounter  = 0; // Discard due to queue error (e.g. full)
    ItsOutputToUdpDiscardNoMemCounter       = 0; // Discard due to no memory
    ItsOutputToUdpDiscardSocketErrorCounter = 0; // Discard due to socket error


    // perpare the socket destination address
    memset((char *)&ItsOutputToUdpDestAddr, 0, sizeof(struct sockaddr_in));

    ItsOutputToUdpDestAddr.sin_family      = AF_INET;
    ItsOutputToUdpDestAddr.sin_addr.s_addr = htonl (ItsOutputToUdpHostIP.GetIpAddress());
    ItsOutputToUdpDestAddr.sin_port        = htons (ItsOutputToUdpHostPort);

    // Output to serial

    ItsOutoutToSerialTaskPriority = 204;     // Task priority

    ItsOutputToSerialQueueSize    = 100;     // Message queue size (# of msgs)
    ItsOutputToSerialMaxMsgSize   = 300;     // Max message size

    ItsOutputToSerialEnableFlag   = false;   // Enable output to serial flag
    ItsOutputToSerialStartFlag    = false;   // Start flag, also disables cfg changes

    ItsOutputToSerialQueueSendCounter          = 0; // Send to queue counter
    ItsOutputToSerialQueueRcvCounter           = 0; // Receive from queue counter
    ItsOutputToSerialTooLongCounter            = 0; // String length > MaxMsgSize
    ItsOutputToSerialDiscardDisableCounter     = 0; // Discard due to disabled
    ItsOutputToSerialDiscardNoStartCounter     = 0; // Discard due to not started
    ItsOutputToSerialDiscardQueueErrorCounter  = 0; // Discard due to queue error (e.g. full)
    ItsOutputToSerialDiscardNoMemCounter       = 0; // Discard due to no memory


    // Time stamp
    ItsTimeStampFrequency  = 0; // Time stamp counter frequency - invalid
    ItsTimeResolutionForMicroSec = 0;
    ItsTimeResolutionForMilliSec = 0;


#ifdef RN_CPU_I960
	TheirTimeStampCounterP     = CardTime::GetMSecTimeP(); // Time stamp value pointer
	REC_COND_FATAL(TheirTimeStampCounterP,17503); // Check NULL
#endif	
    // All done, save the object pointer

    TheirP = this;

}

// --- GetIt ------------------------------------------------------------------
//
// Return pointer to the XdpService object.
//
// Never returns NULL
//
XdpServices *XdpServices::GetIt() {

    // Check object already exists
    if (TheirP == NULL) {
        // Not exists
        REC_FATAL(17503);
    }

    // Return the pointer
    return TheirP;

}

// ============================================================================
// ===
// === Output to UDP functions
// ===
// ============================================================================

// --- OutputToUdp(RWCString) -------------------------------------------------
//
// Sends a RWCString to the output UDP port by calling OutputToUdpSend
//
// true if all OK
//
// Static functions
//

bool XdpServices::OutputToUdp(const RWCString &theString) {

    const char *p =theString.data();

    bool rc = OutputToUdpSend(p);

    return rc;

}

// --- OutputToUdp(char *) ----------------------------------------------------
//
// Sends a char string to the output UDP port by calling OutputToUdpSend
//
// returns true if all OK
//
// Static functions
//

bool XdpServices::OutputToUdp(const char *theStringP) {

    bool rc = OutputToUdpSend(theStringP);

    return rc;

}

// --- OutputToUdpSend --------------------------------------------------------
//
// Sends a char string to the output UDP port. It copies the string into a buffer
// and places the buffer pointer in the queue to the OutputToUdp task.
//
// If the enable flag is one, returns immediately
//
// If not started, returns immediatly
//
// If the message it too long, truncate it.
//
// returns true if all OK
//
//

bool XdpServices::OutputToUdpSend(const char *theStringP) {

    // Check OutputToUdp has been started

    if (ItsOutputToUdpStartFlag == false) {
        // Not started
        ItsOutputToUdpDiscardNoStartCounter++;
        return false;
    }

    // Check OutputToUdp is enabled

    if (ItsOutputToUdpEnableFlag == false) {
        // Disabled
        ItsOutputToUdpDiscardDisableCounter++;
        return false;
    }

    // Find message size, check too long

    UINT32 len = strlen(theStringP);

    if (len > ItsOutputToUdpMaxMsgSize - 1) { // -1 for terminating '\0'
        // Too long
        len = ItsOutputToUdpMaxMsgSize - 1;

        ItsOutputToUdpTooLongCounter++;
    }

    // Allocate buffer for message & check for error

    UINT8 *buf = new UINT8[len + 1];

    if (buf == NULL) {

        // Allocation failed

        ItsOutputToUdpDiscardNoMemCounter++;

        return false;
    }

    // Copy string into buffer & add '\0'

    memcpy(buf,theStringP,(unsigned int)len);

    buf[len] = '\0';

    // Send buffer into queue

    ItsOutputToUdpQueueSendCounter++;

#ifdef RN_WIN32

    bool winRc = Win32OutputToUdpSend(buf);
    if (winRc == false) {
        ItsOutputToUdpDiscardQueueErrorCounter++;
        delete[] buf;
        return false;
    }

#else

    UINT32 message[4];
    message[0] = (long) buf;

	UINT32 retVal = ItsOutputToUdpQueue.Send(message);
    if (retVal != 0) {
        // Error
       ItsOutputToUdpDiscardQueueErrorCounter++;
       delete[] buf;
       return false;
    }
#endif // RN_WIN32

    // Send to queue OK, buf will be deleted by the receiving task

    // All done OK

    return true;
}

// --- OutputToUdpStart --------------------------------------------------------
//
// Setup the output to UDP queue and task. Run the task
//

void XdpServices::OutputToUdpStart()
{

    RN_TRACE(MASK_XDP,"::OutputToUdpStart(" + XdpConv::ToHexStr(this) + ") - begin");

    RN_ASSERT(this, "This is NULL");

    UINT32 retVal;

    // Check not already Start-ed
    if (ItsOutputToUdpStartFlag == true) {
        REC_FATAL_1P(17504, ItsOutputToUdpStartFlag);
    }

    // Create the queue and check for errors

    retVal = ItsOutputToUdpQueue.Create("dsQU",Q_CREATE_FLAGS, ItsOutputToUdpQueueSize);
    if (retVal != 0) {
       REC_FATAL_1P(17504, retVal);
    }

    // Create the task

    retVal = ItsOutputToUdpTask.Create("dsTU",ItsOutoutToUdpTaskPriority);
    if (retVal != 0) {
       REC_FATAL_1P(17504, retVal);
    }

    // Run the task

    retVal = ItsOutputToUdpTask.Start(TASK_FUNCT(XdpServices::S_OutputToUdpTxTask),NO_ARGS);
    if (retVal != 0) {
       REC_FATAL_1P(17504, retVal);
    }

    // Mark Start-ed

    ItsOutputToUdpStartFlag = true;

    // Check if some messages dropped due to not started
    if (ItsOutputToUdpDiscardNoStartCounter > 0) {
        // Lost message
        RN_TRACE(MASK_XDP,"::OutputToUdpStart(" + XdpConv::ToHexStr(this) +
                          ") " + XdpConv::ToStr(ItsOutputToUdpDiscardNoStartCounter) +
                          " Messeges lost before started");
    }

    // All done
    RN_TRACE(MASK_XDP,"::OutputTpUdpStart(" + XdpConv::ToHexStr(this) + ") - end");

}

// --- S_OutputToUdpTxTask ----------------------------------------------------
//
// Static function that calls the object task function
//
void XdpServices::S_OutputToUdpTxTask()
{
    RN_TRACE(MASK_XDP,"::S_OutputToUdpTxTask - begin");

    if (TheirP == NULL) {
        REC_FATAL(17503);
    }

    TheirP->OutputToUdpTxTask();
}
// --- OutputToUdpTxTask ------------------------------------------------------
//
// Output to UDP task functions. Init the socket and wait in a loop for messages
// from the queue. When a message is received, send it and free the buffer.
//
// Never returns
//

void XdpServices::OutputToUdpTxTask()
{

    RN_TRACE(MASK_XDP,"::OutputToUdpTxTask(" + XdpConv::ToHexStr(this) +
        ") - begin IP=" + ItsOutputToUdpHostIP.ToStr() +
        " Port= " + XdpConv::ToStr(ItsOutputToUdpHostPort));

    RN_ASSERT(this, "This is NULL");

    // Open the socket
    INT32 retVal;

    UINT32 bufferSize = 32000; // ??

    ItsOutputToUdpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (ItsOutputToUdpSocket == -1) {
	    REC_FATAL_1P(17505,ItsOutputToUdpSocket);
    }

    retVal = setsockopt(ItsOutputToUdpSocket,SOL_SOCKET, SO_SNDBUF,
					  (char *)&bufferSize, sizeof(bufferSize));
    if (retVal == -1) {
	    REC_FATAL_1P(17505,retVal);
    }

    retVal = setsockopt(ItsOutputToUdpSocket,SOL_SOCKET, SO_RCVBUF,
					  (char *)&bufferSize, sizeof(bufferSize));
    if (retVal == -1) {
	    REC_FATAL_1P(17505,retVal);
    }

#ifdef RN_DEBUG
    // Send 3 dummy messages to generate the ARPs
    char *initMsg = "OutputToUdp Init message\n";
    UINT32 i;

    for ( i = 0 ; i < 3 ; i++) {

#ifdef RN_UNDER_PSOS
         sendto(ItsOutputToUdpSocket,(char *)initMsg , strlen((char*)initMsg) , 0 ,
   	            &ItsOutputToUdpDestAddr, sizeof(struct sockaddr_in));
#endif
#if defined(RN_UNDER_VXWORKS) || defined(RN_WIN32)
         sendto(ItsOutputToUdpSocket,(char *)initMsg , strlen((char*)initMsg) , 0 ,
         		 (sockaddr *)&ItsOutputToUdpDestAddr,  sizeof(struct sockaddr_in));
#endif

    }
#endif // RN_DEBUG

    // We are ready for messages
    UINT32 message[4];
    UINT8  *buf;

    for(;;) {

        // Wait for a msg
	    retVal = ItsOutputToUdpQueue.Receive(message);
        if (retVal != 0) {
            REC_FATAL_1P(17505,retVal);
        }

        // Got a msg

        ItsOutputToUdpQueueRcvCounter++;

        // Get the buffer pointer

        buf = (UINT8 *) message[0];

		// Check buffer pointer is not NULL
        if (buf == NULL) {
            REC_FATAL(17503);
        }

        // Send to the UDP socket

#ifdef RN_UNDER_PSOS
        retVal = sendto(ItsOutputToUdpSocket,(char *)buf , strlen((char*)buf) , 0 ,
   	                     &ItsOutputToUdpDestAddr, sizeof(struct sockaddr_in));
#endif
#if defined(RN_UNDER_VXWORKS) || defined(RN_WIN32)
        retVal = sendto(ItsOutputToUdpSocket,(char *)buf , strlen((char*)buf) , 0 ,
     		             (sockaddr *)&ItsOutputToUdpDestAddr,  sizeof(struct sockaddr_in));
#endif

        if (retVal < 0) {
            // Just counter the error
            ItsOutputToUdpDiscardSocketErrorCounter++;
        }

        // Free the buffer
	    delete[] buf;
  }
}

// --- OutputToUdpSetIP -------------------------------------------------------
//
// Sets the destination IP address of output to UDP
//
// This function can be called before and after switch to start
//
bool XdpServices::OutputToUdpSetIP(const IpAddress &theIP){

    RN_ASSERT(this, "This is NULL");

    bool rc;

    // Check the IP address validity

    rc = theIP.IsValid();
    if (rc == false) {
        SetErrorString("Invalid IP address");
        return false;
    }

    // IP address is valid, set it

    ItsOutputToUdpHostIP = theIP;

    // Update the socket

    ItsOutputToUdpDestAddr.sin_addr.s_addr = htonl (ItsOutputToUdpHostIP.GetIpAddress());

    // All done OK

    SetInfoString("Destination address is " + ItsOutputToUdpHostIP.ToStr()
                   + " port " + XdpConv::ToStr(ItsOutputToUdpHostPort));

    return true;
}


// --- OutputToUdpSetPort -----------------------------------------------------
//
// Sets the destination port of output to UDP
//
// This function can be called before and after switch to start
//
bool XdpServices::OutputToUdpSetPort(const IpPortNumber &thePort) {

    RN_ASSERT(this, "This is NULL");

    // Check the port in range

    if ((thePort < 5000) || (thePort > 7000)) {
        SetErrorString("port " + XdpConv::ToStr(thePort) +  " is outside of range");
        return false;
    }

    // IP address is valid, set it

    ItsOutputToUdpHostPort = thePort;

    // Update the socket

    ItsOutputToUdpDestAddr.sin_port = htons (ItsOutputToUdpHostPort);

    // All done OK

    SetInfoString("Destination address is " + ItsOutputToUdpHostIP.ToStr()
                   + " port " + XdpConv::ToStr(ItsOutputToUdpHostPort));

    return true;
}

// --- OutputToUdpSetTaskPriority ---------------------------------------------
//
// Sets the output to UDP task priority
//
// This function must be called before switch to start

bool XdpServices::OutputToUdpSetTaskPriority(UINT32 theTaskPriority) {

    RN_ASSERT(this, "This is NULL");

    // Check not started
    if (ItsOutputToUdpStartFlag == true) {
        // error
        SetErrorString("Can't set task priority after it has been started");
        return false;
    }

    // Check the priority in range

    if (theTaskPriority > 254) {
        SetErrorString("Priority " + XdpConv::ToStr(theTaskPriority) + " is too big");
        return false;
    }

    // Set it

    ItsOutoutToUdpTaskPriority = theTaskPriority;

    // All done OK

    SetInfoString("Task priority is " +  XdpConv::ToStr(ItsOutoutToUdpTaskPriority) );

    return true;
}

// --- OutputToUdpSetQueueSize ------------------------------------------------
//
// Sets the output to UDP Queue size
//
// This function must be called before switch to start

bool XdpServices::OutputToUdpSetQueueSize(UINT32 theQueueSize) {

    RN_ASSERT(this, "This is NULL");

    // Check not started
    if (ItsOutputToUdpStartFlag == true) {
        // error
        SetErrorString("Can't set queue size after it has been started");
        return false;
    }

    // Check the size in range

    if (theQueueSize > 1000) {
        SetErrorString("Queue size " + XdpConv::ToStr(theQueueSize) + " is too big");
        return false;
    }

    // Set it

    ItsOutputToUdpQueueSize = theQueueSize;

    // All done OK

    SetInfoString("Queue size is " +  XdpConv::ToStr(ItsOutputToUdpQueueSize) );

    return true;
}

// --- OutputToUdpSetMaxMsgSize -----------------------------------------------
//
// Sets the output to UDP max message size
//
// This function must be called before switch to start

bool XdpServices::OutputToUdpSetMaxMsgSize(UINT32 theMaxMsgSize) {

     RN_ASSERT(this, "This is NULL");

    // Check not started
    if (ItsOutputToUdpStartFlag == true) {
        // error
        SetErrorString("Can't max message size after it has been started");
        return false;
    }

    // Check the max size in range

    if ((theMaxMsgSize < 100) || (theMaxMsgSize > 5000) ) {
        SetErrorString("Max message size " + XdpConv::ToStr(theMaxMsgSize) + " is invalid");
        return false;
    }

    // Set it

    ItsOutputToUdpMaxMsgSize = theMaxMsgSize;

    // All done OK

    SetInfoString("Max message size is " +  XdpConv::ToStr(ItsOutputToUdpMaxMsgSize) );

    return true;
}

// --- OutputToUdpEnable ------------------------------------------------------
//
// Enable/disable output to UDP
//
// theEnable==true => enable, else disable

bool XdpServices::OutputToUdpEnable(bool theEnable) {

     RN_ASSERT(this, "This is NULL");

    // Set it

    ItsOutputToUdpEnableFlag = theEnable;

    // Set to info string

    if (ItsOutputToUdpEnableFlag == true) {
        // Enabled
        SetInfoString("Output to UDP enabled");
    }
    else {
        // Disabled
        SetInfoString("Output to UDP disabled");
    }

    // All done OK
    return true;
}

// ============================================================================
// ===
// === Output to serial functions
// ===
// ============================================================================

// --- OutputToSerial(RWCString) ----------------------------------------------
//
// Sends a RWCString to the output the serial port by calling OutputToSerialSend
//
// true if all OK
//
// Static functions
//

bool XdpServices::OutputToSerial(const RWCString &theString) {

    const char *p =theString.data();

    bool rc = OutputToSerialSend(p);

    return rc;

}

// --- OutputToSerial(char *) -------------------------------------------------
//
// Sends a char string to the output the serial port by calling OutputToSerialSend
//
// returns true if all OK
//
// Static functions
//

bool XdpServices::OutputToSerial(const char *theStringP) {

    bool rc = OutputToSerialSend(theStringP);

    return rc;

}

// --- OutputToSerialSend -----------------------------------------------------
//
// Sends a char string to the output the serial port. It copies the string into a buffer
// and places the buffer pointer in the queue to the OutputToSerial task.
//
// If the enable flag is one, returns immediately
//
// If not started, returns immediatly
//
// If the message it too long, truncate it.
//
// returns true if all OK
//
//

bool XdpServices::OutputToSerialSend(const char *theStringP) {

    // Check OutputToSerial has been started

    if (ItsOutputToSerialStartFlag == false) {
        // Not started
        ItsOutputToSerialDiscardNoStartCounter++;
        return false;
    }

    // Check OutputToSerial is enabled

    if (ItsOutputToSerialEnableFlag == false) {
        // Disabled
        ItsOutputToSerialDiscardDisableCounter++;
        return false;
    }

    // Find message size, check too long

    UINT32 len = strlen(theStringP);

    if (len > ItsOutputToSerialMaxMsgSize - 1) { // -1 for terminating '\0'
        // Too long
        len = ItsOutputToSerialMaxMsgSize - 1;

        ItsOutputToSerialTooLongCounter++;
    }

    // Allocate buffer for message & check for error

    UINT8 *buf = new UINT8[len + 1];

    if (buf == NULL) {

        // Allocation failed

        ItsOutputToSerialDiscardNoMemCounter++;

        return false;
    }

    // Copy string into buffer & add '\0'

    memcpy(buf,theStringP, (unsigned int) len);

    buf[len] = '\0';

    // Send buffer into queue

    ItsOutputToSerialQueueSendCounter++;

#ifdef RN_WIN32

    bool winRc = Win32OutputToSerialSend(buf);
    if (winRc == false) {
        ItsOutputToSerialDiscardQueueErrorCounter++;
        delete[] buf;
        return false;
    }

#else

    UINT32 message[4];
    message[0] = (long) buf;

	UINT32 retVal = ItsOutputToSerialQueue.Send(message);
    if (retVal != 0) {
        // Error
       ItsOutputToSerialDiscardQueueErrorCounter++;
       delete[] buf;
       return false;
    }
#endif // RN_WIN32

    // Send to queue OK, buf will be deleted by the receiving task

    // All done OK

    return true;
}

// --- OutputToSerialStart ----------------------------------------------------
//
// Setup the output to serial queue and task. Run the task
//

void XdpServices::OutputToSerialStart()
{

    RN_TRACE(MASK_XDP,"::OutputToSerialStart(" + XdpConv::ToHexStr(this) + ") - begin");

    RN_ASSERT(this, "This is NULL");

    UINT32 retVal;

    // Check not already Start-ed
    if (ItsOutputToSerialStartFlag == true) {
        REC_FATAL_1P(17507, ItsOutputToSerialStartFlag);
    }

    // Create the queue and check for errors

    retVal = ItsOutputToSerialQueue.Create("dsQS",Q_CREATE_FLAGS, ItsOutputToSerialQueueSize);
    if (retVal != 0) {
       REC_FATAL_1P(17507, retVal);
    }

    // Create the task

    retVal = ItsOutputToSerialTask.Create("dsTS",ItsOutoutToSerialTaskPriority);
    if (retVal != 0) {
       REC_FATAL_1P(17507, retVal);
    }

    // Run the task

    retVal = ItsOutputToSerialTask.Start(TASK_FUNCT(XdpServices::S_OutputToSerialTxTask),NO_ARGS);
    if (retVal != 0) {
       REC_FATAL_1P(17507, retVal);
    }

    // Mark Start-ed

    ItsOutputToSerialStartFlag = true;

    // Check if some messages dropped due to not started
    if (ItsOutputToSerialDiscardNoStartCounter > 0) {
        // Lost message
        RN_TRACE(MASK_XDP,"::OutputTpSerialStart(" + XdpConv::ToHexStr(this) +
                          ") " + XdpConv::ToStr(ItsOutputToSerialDiscardNoStartCounter) +
                          " Messeges lost before started");
    }

    // All done
    RN_TRACE(MASK_XDP,"::OutputToSerialStart(" + XdpConv::ToHexStr(this) + ") - end");

}

// --- S_OutputToSerialTxTask -------------------------------------------------
//
// Static function that calls the object task function
//
void XdpServices::S_OutputToSerialTxTask()
{
    RN_TRACE(MASK_XDP,"::S_OutputToSerialTxTask - begin");

    if (TheirP == NULL) {
        REC_FATAL(17503);
    }

    TheirP->OutputToSerialTxTask();
}
// --- OutputToSerialTxTask ---------------------------------------------------
//
// Output to serial task functions. Wait in a loop for messages
// from the queue. When a message is received, send it and free the buffer.
//
// Never returns
//

void XdpServices::OutputToSerialTxTask()
{

    RN_TRACE(MASK_XDP,"::OutputToSerialTxTask(" + XdpConv::ToHexStr(this) );

    RN_ASSERT(this, "This is NULL");

#ifdef RN_DEBUG
    // Send 3 dummy messages
    char *initMsg = "OutputToSerial Init message\n";
    UINT32 i;

    for ( i = 0 ; i < 3 ; i++) {
        PRINT("%s",initMsg);
    }
#endif // RN_DEBUG

    // We are ready for messages
    UINT32 message[4];
    UINT8  *buf;
    INT32 retVal;

    for(;;) {

        // Wait for a msg
	    retVal = ItsOutputToSerialQueue.Receive(message);
        if (retVal != 0) {
            REC_FATAL_1P(17508,retVal);
        }

        // Got a msg

        ItsOutputToSerialQueueRcvCounter++;

        // Get the buffer pointer

        buf = (UINT8 *) message[0];

		// Check not a NULL buffer

        if (buf == NULL) {
            REC_FATAL(17503);
        }


        // Send to the Serial socket

        PRINT("%s",buf);

        // Free the buffer
	    delete[] buf;
  }
}

// --- OutputToSerialSetTaskPriority ------------------------------------------
//
// Sets the output to serial task priority
//
// This function must be called before switch to start

bool XdpServices::OutputToSerialSetTaskPriority(UINT32 theTaskPriority) {

    RN_ASSERT(this, "This is NULL");

    // Check not started
    if (ItsOutputToSerialStartFlag == true) {
        // error
        SetErrorString("Can't set task priority after it has been started");
        return false;
    }

    // Check the priority in range

    if (theTaskPriority > 254) {
        SetErrorString("Priority " + XdpConv::ToStr(theTaskPriority) + " is too big");
        return false;
    }

    // Set it

    ItsOutoutToSerialTaskPriority = theTaskPriority;

    // All done OK

    SetInfoString("Task priority is " +  XdpConv::ToStr(ItsOutoutToSerialTaskPriority) );

    return true;
}

// --- OutputToSerialSetQueueSize ---------------------------------------------
//
// Sets the output to serial queue size
//
// This function must be called before switch to start

bool XdpServices::OutputToSerialSetQueueSize(UINT32 theQueueSize) {

    RN_ASSERT(this, "This is NULL");

    // Check not started
    if (ItsOutputToSerialStartFlag == true) {
        // error
        SetErrorString("Can't set queue size after it has been started");
        return false;
    }

    // Check the size in range

    if (theQueueSize > 1000) {
        SetErrorString("Queue size " + XdpConv::ToStr(theQueueSize) + " is too big");
        return false;
    }

    // Set it

    ItsOutputToSerialQueueSize = theQueueSize;

    // All done OK

    SetInfoString("Queue size is " +  XdpConv::ToStr(ItsOutputToSerialQueueSize) );

    return true;
}

// --- OutputToSerialSetMaxMsgSize --------------------------------------------
//
// Sets the output to serial max message size
//
// This function must be called before switch to start

bool XdpServices::OutputToSerialSetMaxMsgSize(UINT32 theMaxMsgSize) {

    RN_ASSERT(this, "This is NULL");

    // Check not started
    if (ItsOutputToSerialStartFlag == true) {
        // error
        SetErrorString("Can't max message size after it has been started");
        return false;
    }

    // Check the max size in range

    if ((theMaxMsgSize < 100) || (theMaxMsgSize > 5000) ) {
        SetErrorString("Max message size " + XdpConv::ToStr(theMaxMsgSize) + " is invalid");
        return false;
    }

    // Set it

    ItsOutputToSerialMaxMsgSize = theMaxMsgSize;

    // All done OK

    SetInfoString("Max message size is " +  XdpConv::ToStr(ItsOutputToSerialMaxMsgSize) );

    return true;
}

// --- OutputToSerialEnable ---------------------------------------------------
//
// Enable/disable output to serial
//
// theEnable==true => enable, else disable

bool XdpServices::OutputToSerialEnable(bool theEnable) {

    RN_ASSERT(this, "This is NULL");

    // Set it

    ItsOutputToSerialEnableFlag = theEnable;

    // Set to info string

    if (ItsOutputToSerialEnableFlag == true) {
        // Enabled
        SetInfoString("Output to serial enabled");
    }
    else {
        // Disabled
        SetInfoString("Output to serial disabled");
    }

    // All done OK
    return true;
}


// --- SaveConfig --------------------------------------------------------------
//
// Save XDP package configuration (UDP IP, port, enable flag; serial enable flag) to file
//
void  XdpServices::SaveConfig()
{
      XdpConfigMngr::GetIt()->SaveGlobalConfig(ItsOutputToUdpHostIP,
                                               ItsOutputToUdpHostPort,
                                               ItsOutputToUdpEnableFlag,
                                               ItsOutputToSerialEnableFlag);

}


//  --- PrintParams()  ---------------------------------------------------------
//
// Print the global xdp parameters which are saved to configuratuin files -
// used for tests
//

void XdpServices::PrintParams()
{
    printf("-----   XdpServices::PrintParams   -----\n");
    printf("ItsOutputToUdpHostIP = %s\n", ItsOutputToUdpHostIP.ToStr().operator const char*());
    printf("ItsOutputToUdpHostPort = %d\n", ItsOutputToUdpHostPort);
    printf("ItsOutputToUdpEnableFlag = %d\n", ItsOutputToUdpEnableFlag);
    printf("ItsOutputToSerialEnableFlag = %d\n", ItsOutputToSerialEnableFlag);
}

// ============================================================================
// ===
// === Time stamp functions
// ===
// ============================================================================

// --- TimeStampSetFrequency --------------------------------------------------
//
// Set the frequency of the time stamp counter in Hz
//
// Fails if frequency is zero
//

bool XdpServices::TimeStampSetFrequency(double theFrequency) {

    RN_ASSERT(this, "This is NULL");

    // Check theFrequency is not zero

    if (theFrequency == 0) {
        return false;
    }

    // Set it
    ItsTimeStampFrequency = theFrequency;

    ItsTimeResolutionForMicroSec =  // Calculate In micro seconds per tik
        TimeStampParameterToResolution(TIME_STAMP_PARAM_FOR_MICROSEC) * 1000000;

    ItsTimeResolutionForMilliSec =  // Calculate In milli seconds per tik
        TimeStampParameterToResolution(TIME_STAMP_PARAM_FOR_MILLISEC) * 1000;

    // All done OK

    return true;
}

// --- TimeStampResultionToParameter ------------------------------------------
//
// Return a time stamp paramter that is used in TimeStampGet.
//
// The parameter represnts the required (or better) time resolution.
//

UINT32 XdpServices::TimeStampResolutionToParameter(double theResolution) {

    RN_ASSERT(this, "This is NULL");

    // Get the max shift
    UINT32 maxShift = TimeStampGetMaxShift();

    // Loop all possible shift counts, find the biggest shift count
    // that is equal or better to the required resolution

    INT32 shiftCount;
    double curRes;

    for(shiftCount = maxShift ; shiftCount >= 0  ; shiftCount--) {

        // GetCurrent resolution
        curRes = TimeStampParameterToResolution(shiftCount);

        // Is the current resolution good enough ?
        if ( theResolution >= curRes ) {
            // Yes, use it
            return shiftCount;
        }
    }

    // Not found, the finest resolution is 0, use it
    return 0;
}

// --- TimeStampParameterToResolution ------------------------------------------
//
// Converts a timeStampParameter into the time stamp resolution that it represents
//
double XdpServices::TimeStampParameterToResolution(UINT32 TimeStampParameter) {

    RN_ASSERT(this, "This is NULL");

    RN_ASSERT(TimeStampParameter <= TimeStampGetMaxShift() ,
              "XdpServices::TimeStampParameterToResolution TimeStampParameter invalid");

    // Verify frequnecy > 0  (=0 mean not inited)
    if (ItsTimeStampFrequency <= 0) {
        REC_FATAL(17510);
    }

    // Calc the resolution
    double resolution = (double) 1.0 / ItsTimeStampFrequency * (1u << TimeStampParameter);

    // Return it
    return resolution;

}

// --- TimeStampGet -----------------------------------------------------------
//
// Returns a time stamp in a UINT32. The time stamp resolution is determent
// by the parameter.
//
// To convert to UINT32 time stamp into a real time value, multiply it by the
// resultion.
//
// The implementation of this function is HW depenedet
//
// This function should be very fast and interrupt safe since it is beeing
// call from XdpTrace::WriteEntry functions
//


// --- PCC with GNU implementation ---

#if defined(RN_CPU_PPC) && defined(RN_GCC)
// Read the PPC time base register
// 0. The whole function is written direcly in assmebler becuase the optimization
//    corupts some registers
// 1. Read TBU & TBL until TBU is not change during the read
// 2. We got 64 bit value. Shift it to the right theTimeStampParameter
//    times and put it into 32 bit result vatiable

//UINT32 XdpServices::TimeStampGet( UINT32 theTimeStampParameter ) {

__asm("
	.globl TimeStampGet__11XdpServicesUi
	.type	 TimeStampGet__11XdpServicesUi,@function
TimeStampGet__11XdpServicesUi:


    mr      6, 3                /* R6 <= R3 = theTimeStampParameter                               */

TimeStampGetRead:				
	mftbu   4                   /* R4 <= Time base upper                                          */
    mftb    3                   /* R3 <= Time base lower                                          */
    mftbu   5                   /* R5 <= Time base upper (again)                                  */
	cmplw   4,5                 /* Is upper has been chnaged                                      */
    bne     TimeStampGetRead    /* Yes, read again                                                */
								/* Time base is at R4:R3                                          */

                                /* save shift count in R5                                         */

	srw     3,3,6               /* R3 <= R3 >> theTimeStampParameter                              */

	subfic  6,6,32              /* R6 <= 32 - R6 = 32 -  (shift count)                            */
	slw		4,4,6               /* R4 <= R4 << R6 = TBU << (32 - theTimeStampParameter)           */

	or      3,3,4               /* R3 <= R3 or R4 = TBU << (32 - shift) | TBL >> shift            */

	blr "                       /* return, return value is R3                                     */
    );

#endif // PPC with GNU

// --- I960 implementation
#ifdef RN_CPU_I960
UINT32 XdpServices::TimeStampGet( UINT32 /*theTimeStampParameter - not used */ ) {

    return (UINT32) *TheirTimeStampCounterP;

}
#endif

// --- WIN32 implementation ---

#ifdef RN_WIN32
UINT32 XdpServices::TimeStampGet( UINT32 theTimeStampParameter ) {
	// Win32
    return (UINT32) clock();
}
#endif // RNWIN32



// --- TimeStampGetMaxShift ---------------------------------------------------
//
// Returns a the max shift count of the time stamp resulotion.
//
// This function is use internaly to hide HW differeneces
//
// The implementation of this function is HW depenedet
//

UINT32 XdpServices::TimeStampGetMaxShift() {

    RN_ASSERT(this, "This is NULL");

#ifdef RN_CPU_PPC

	// With PPC the timer is 64bit long. So we can make up to 32 shifts
    // use only 31 shift to avoid problem with UINT32 overflows
    return 31;

#endif

#ifdef RN_CPU_I960
    return 0;
#endif

#ifdef RN_WIN32
    return 0;
#endif

}

// ============================================================================
// ===
// === Task stamp functions
// ===
// ============================================================================

// --- TimeStampTaskStampToString ---------------------------------------------
//
// Returns a string that of a task name for the task stamp
//
// The task stamp is retrived by the macro XDP_SERVICES_TASK_STAMP_GET
//
// The implemetation is OS specific
//

RWCString  XdpServices::TaskStampToString(UINT32 theTaskStamp) {


    RWCString tName;

// --- VxWorks ------------------------

#ifdef RN_UNDER_VXWORKS

    // theTaskStamp is the VxWorks task id

    // Get the task name
    char *taskNameCharP;

    taskNameCharP= taskName((int)theTaskStamp);

    // Check NULL pointer

    if (taskNameCharP == NULL) {
        // Null pointer, task is not valid, either the task has been deleted or we were inside ISR
        tName = "?????";
    }
    else {
        // Task name is valid, but it might be empty
        if (taskNameCharP[0] == '\0') {
            // Empty task name
            tName = "NoName";
        }
        else {
            // Vaild, non empty name, use it
            tName = taskNameCharP;
        }
    }

#endif // VxWorks


// --- pSOS ---------------------------

#ifdef RN_UNDER_PSOS

// Tmp: return th ID instead of the name

	 tName = XdpConv::ToHexStr(theTaskStamp);

#endif // pSOS

// --- Win32 --------------------------
#ifdef RN_WIN32

    tName = "WIN32";

#endif // Win32

    // All done
    return tName;
}


// ============================================================================
// ===
// === Error & info functions (support functions)
// ===
// ============================================================================

// --- ClearStatusStrings -----------------------------------------------------
//
// Clears the error & info strings
//

void XdpServices::ClearStatusStrings() {

    ItsErrorString = "";
    ItsInfoString  = "";

}

// --- GetStatusStrings -------------------------------------------------------
//
// Return the error and info strings
//

RWCString XdpServices::GetStatusStrings() {

    RWCString s = ItsErrorString + ItsInfoString;

    return s;

}


// --- SetErrorString ---------------------------------------------------------
//
// Appends the error string
//

void XdpServices::SetErrorString(RWCString theError) {

    ItsErrorString += "Error: " + theError + "\n";

    RN_TRACE(MASK_XDP, "XdpServices:: (" + XdpConv::ToHexStr(this) +
                       ") Error: " + theError);
 }

// --- SetInfoString ----------------------------------------------------------
//
// Appends the info string
//

void XdpServices::SetInfoString(RWCString theInfo) {

    ItsInfoString += "Info: " + theInfo + "\n";

    RN_TRACE(MASK_XDP, "XdpServices:: (" + XdpConv::ToHexStr(this) +
                       ") Info: " + theInfo);
}



// ============================================================================
// ===
// === WIN32 support functions functions
// ===
// ============================================================================

#ifdef RN_WIN32

// --- OutputToUdpSendWin32 ---------------------------------------------------
//
// Prints buf with the UDP prefix. The buffer is freed
//
bool XdpServices::Win32OutputToUdpSend(UINT8 *theBuf) {

    printf("UDP:%s",theBuf);

    delete[] theBuf;

    return true;
}


// --- OutputToSerialSendWin32 ---------------------------------------------------
//
// Prints buf with the UDP prefix. The buffer is freed
//
bool XdpServices::Win32OutputToSerialSend(UINT8 *theBuf) {

    printf("Serial:%s",theBuf);

    delete[] theBuf;

    return true;
}

#endif // RN_WIN32


/**************************************************************************

 NAME:  TimeStampGetInMicroSec

 DESCRIPTION:

 PARAMETERS:

 SCOPE:

 RETURN:

**************************************************************************/
UINT32 XdpServices::TimeStampGetInMicroSec()
{
    return (UINT32)(XdpServices::GetIt()->ItsTimeResolutionForMicroSec *
                    (double)(XdpServices::TimeStampGet(TIME_STAMP_PARAM_FOR_MICROSEC)));
}


/**************************************************************************

 NAME:  TimeStampGetInMilliSec

 DESCRIPTION:

 PARAMETERS:

 SCOPE:

 RETURN:

**************************************************************************/
UINT32 XdpServices::TimeStampGetInMilliSec()
{
    return (UINT32)(XdpServices::GetIt()->ItsTimeResolutionForMilliSec *
                    (double)(XdpServices::TimeStampGet(TIME_STAMP_PARAM_FOR_MILLISEC)));
}

