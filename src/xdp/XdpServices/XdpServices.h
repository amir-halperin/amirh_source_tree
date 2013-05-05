/* XdpServices.h   */

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

    See the .cc file for the provides services list.

AUTHOR
   Gaash, SB


**************************************************************************/

#ifndef XdpServices_h
#define XdpServices_h


#include "IpAddress.h"
#include "IpPortNumber.h"
#include "PTaskWithID.h"
#include "PQueue.h"

// --- VxWorks specific includes ----------------------------------------------
#ifdef RN_UNDER_VXWORKS

#include "pna.h"
#include "intLib.h"    // For interrupts stuff
#include "taskLib.h"

#endif

// --- pSOS specific includes -------------------------------------------------
#ifdef RN_UNDER_PSOS

#include "bsp.h"
#include "pna.h"

#endif

// --- WIN32 specific includes ------------------------------------------------
#ifdef RN_WIN32

#include "pna.h"

#endif

#if 1


// ****************************************************************************
// ****************************************************************************
// ***
// *** Macros - for high performace operations
// ***
// ****************************************************************************
// ****************************************************************************

// --- VxWorks specific macros ------------------------------------------------
#ifdef RN_UNDER_VXWORKS


// Interrupts: Use VxWork intLib & intArchLib to deal with interrups
#define XDP_SERVICES_INTERRUPTS_DISABLE(key) { (key) = intLock(); } ;
#define XDP_SERVICES_INTERRUPTS_RESTORE(key) { intUnlock(key); } ;
#define XDP_SERVICES_INTERRUPTS_IS_INSIDE()  ( intContext() )

// Tasks: Use VxWork taskLib to get the current task
#define XDP_SERVICES_TASK_STAMP_GET() ( taskIdSelf() )

#endif

// --- pSOS specific macros ---------------------------------------------------
#ifdef RN_UNDER_PSOS

extern "C" unsigned long splx(unsigned long mask); // Defined in bspfuncs.h

// interrupts
#define XDP_SERVICES_INTERRUPTS_DISABLE(key) { (key) = splx(31); } ;
#define XDP_SERVICES_INTERRUPTS_RESTORE(key) { splx(key); } ;

// is inside interrupt is not supportted (yet) under pSOS. To be on the saveside
// report as if we are inside interrupt, but this disables XDP trace print
// Use the flags below to enable the print if you are sure that you don't
// print indisde interrupts
#ifdef DBG_XDP_SERVICES_NEVER_INSIDE_INTERRUPT
// with debug flag
#define XDP_SERVICES_INTERRUPTS_IS_INSIDE()  ( false  )
#else
// normal
#define XDP_SERVICES_INTERRUPTS_IS_INSIDE()  ( true  )
#endif

// Tasks: Use variable for root.cc
extern UINT32 ServiceCardCurrentTaskId; // defined in root.cc

#define XDP_SERVICES_TASK_STAMP_GET() ( ServiceCardCurrentTaskId )

#endif

// --- WIN32 specific macros --------------------------------------------------
#ifdef RN_WIN32

// Interrupts: no interrupts under WIN32 - dummy staff
#define XDP_SERVICES_INTERRUPTS_DISABLE(key) { key=0x9999; } ; // To prevent warnings
#define XDP_SERVICES_INTERRUPTS_RESTORE(key) { key++; } ;
#define XDP_SERVICES_INTERRUPTS_IS_INSIDE()  ( false ) // never inside interrupt

// Tasks
#define XDP_SERVICES_TASK_STAMP_GET() ( 0x12345678 )

#endif

// Check for unsupported OS
#ifndef XDP_SERVICES_INTERRUPTS_DISABLE
#error "XDP_SERVICES_INTERRUPTS_DISABLE is not defined, check the OS flags"
#endif


// ****************************************************************************
// ***
// *** XdpSerives class declaration
// ***
// ****************************************************************************


class XdpServices

{

    friend class XdpMngr;
    friend class XdpMngrCmdMngr;
    friend class XdpLocalConfigService;

// ****************************************************************************
// ****************************************************************************
// ***
// *** public
// ***
// ****************************************************************************
// ****************************************************************************
public:

// ============================================================================
// ===
// === Enums (public)
// ===
// ============================================================================

// ============================================================================
// ===
// === Common function functions (public)
// ===
// ============================================================================


    static XdpServices * GetIt();

// ============================================================================
// ===
// === Output functions (public)
// ===
// ============================================================================

    // Output to UDP
    bool    OutputToUdp(const RWCString &theString);
    bool    OutputToUdp(const char *theStringP);

    // Output to Serial
    bool    OutputToSerial(const RWCString &theString);
    bool    OutputToSerial(const char *theString);
    
// ============================================================================
// ===
// === Interrupt functions (public)
// ===
// ============================================================================

    static void InterruptsDisable(UINT32 &theKey) { XDP_SERVICES_INTERRUPTS_DISABLE(theKey); };
    static void InterruptsRestroe(UINT32 theKey)  { XDP_SERVICES_INTERRUPTS_RESTORE(theKey); };
    static bool InterruptsIsInside()              { return XDP_SERVICES_INTERRUPTS_IS_INSIDE(); };

// ============================================================================
// ===
// === Time stamp functions (public)
// ===
// ============================================================================

           bool     TimeStampSetFrequency(double theFrequency);
           UINT32   TimeStampResolutionToParameter(double theResolution);
           double   TimeStampParameterToResolution(UINT32 TimeStampParameter);
    static UINT32   TimeStampGet( UINT32 TimeStampParameter );

    static UINT32   TimeStampGetInMicroSec();
    static UINT32   TimeStampGetInMilliSec();

// ============================================================================
// ===
// === Task stamp functions (public)
// ===
// ============================================================================

    RWCString  TaskStampToString(UINT32 theTaskStamp);

    void Save()
    {
        SaveConfig();
    }


// ****************************************************************************
// ****************************************************************************
// ***
// *** protected
// ***
// ****************************************************************************
// ****************************************************************************
protected:

// ============================================================================
// ===
// === Enums (protected)
// ===
// ============================================================================


// ============================================================================
// ===
// === Common functions (protected)
// ===
// ============================================================================

    XdpServices();

// ============================================================================
// ===
// === Output functions (protected)
// ===
// ============================================================================

    // Output to UDP

    void    OutputToUdpStart();
    bool    OutputToUdpSend(const char *theString);

    static  void    S_OutputToUdpTxTask();
    void            OutputToUdpTxTask();

    bool    OutputToUdpSetIP(const IpAddress &theIP);
    bool    OutputToUdpSetPort(const IpPortNumber &thePort);
    bool    OutputToUdpSetTaskPriority(UINT32 theTaskPriority);
    bool    OutputToUdpSetQueueSize(UINT32 theQueueSize);
    bool    OutputToUdpSetMaxMsgSize(UINT32 theMaxMsgSize);
    bool    OutputToUdpEnable(bool theEnable);


    // Output to Serial

    void    OutputToSerialStart();
    bool    OutputToSerialSend(const char *theString);

    static  void    S_OutputToSerialTxTask();
    void            OutputToSerialTxTask();

    bool    OutputToSerialSetTaskPriority(UINT32 theTaskPriority);
    bool    OutputToSerialSetQueueSize(UINT32 theQueueSize);
    bool    OutputToSerialSetMaxMsgSize(UINT32 theMaxMsgSize);
    bool    OutputToSerialEnable(bool theEnable);


// ============================================================================
// ===
// === Configuration save functions (protected)
// ===
// ============================================================================

   void           SaveConfig();
   void           PrintParams();

// ============================================================================
// ===
// === Time stamp functions (protected)
// ===
// ============================================================================

    UINT32  TimeStampGetMaxShift();

// ============================================================================
// ===
// === Error & info functions (support functions)
// ===
// ============================================================================

    void      ClearStatusStrings();        // Clears the error & info strings
    RWCString GetStatusStrings();          // Returns the error & info strings

    void      SetErrorString(RWCString theError);     // Set the error string
    void      SetInfoString(RWCString theInfo);       // Set the info string
   
// ============================================================================
// ===
// === WIN32 support functions (protected)
// ===
// ============================================================================

#ifdef RN_WIN32
    bool Win32OutputToUdpSend(UINT8 *theBuf);
    bool Win32OutputToSerialSend(UINT8 *theBuf);
#endif

// ============================================================================
// ===
// === Data members (protected)
// ===
// ============================================================================
  
// --- Common -----------------------------------------------------------------

    static XdpServices * TheirP;

// --- Output to UDP ----------------------------------------------------------

    IpAddress    ItsOutputToUdpHostIP;        // Host IP
    IpPortNumber ItsOutputToUdpHostPort;      // Host port
    struct sockaddr_in ItsOutputToUdpDestAddr;// UDP destination (Host IP & port)
    int          ItsOutputToUdpSocket;        // Socket number

    PTaskWithID  ItsOutputToUdpTask;          // Output UDP task
    PQueue       ItsOutputToUdpQueue;         // Messege queue
    UINT32       ItsOutoutToUdpTaskPriority;  // Task priority

    UINT32       ItsOutputToUdpQueueSize;     // Message queue size (# of msgs)
    UINT32       ItsOutputToUdpMaxMsgSize;    // Max message size
    
    bool         ItsOutputToUdpEnableFlag;    // Enable output to UDP flag
    bool         ItsOutputToUdpStartFlag;     // Start flag, also disables cfg changes

    UINT32       ItsOutputToUdpQueueSendCounter;         // Send to queue counter
    UINT32       ItsOutputToUdpQueueRcvCounter;          // Receive from queue counter
    UINT32       ItsOutputToUdpTooLongCounter;           // String length > MaxMsgSize
    UINT32       ItsOutputToUdpDiscardDisableCounter;    // Discard due to disabled
    UINT32       ItsOutputToUdpDiscardNoStartCounter;    // Discard due to not started
    UINT32       ItsOutputToUdpDiscardQueueErrorCounter; // Discard due to queue error (e.g. full)
    UINT32       ItsOutputToUdpDiscardNoMemCounter;      // Discard due to no memory
    UINT32       ItsOutputToUdpDiscardSocketErrorCounter;// Discard due to socket error


// --- Output to Serial  ------------------------------------------------------

    PTaskWithID  ItsOutputToSerialTask;          // Output serial task
    PQueue       ItsOutputToSerialQueue;         // Messege queue
    UINT32       ItsOutoutToSerialTaskPriority;  // Task priority

    UINT32       ItsOutputToSerialQueueSize;     // Message queue size (# of msgs)
    UINT32       ItsOutputToSerialMaxMsgSize;    // Max message size
    
    bool         ItsOutputToSerialEnableFlag;    // Enable output to serial flag
    bool         ItsOutputToSerialStartFlag;     // Start flag, also disables cfg changes

    UINT32       ItsOutputToSerialQueueSendCounter;         // Send to queue counter
    UINT32       ItsOutputToSerialQueueRcvCounter;          // Receive from queue counter
    UINT32       ItsOutputToSerialTooLongCounter;           // String length > MaxMsgSize
    UINT32       ItsOutputToSerialDiscardDisableCounter;    // Discard due to disabled
    UINT32       ItsOutputToSerialDiscardNoStartCounter;    // Discard due to not started
    UINT32       ItsOutputToSerialDiscardQueueErrorCounter; // Discard due to queue error (e.g. full)
    UINT32       ItsOutputToSerialDiscardNoMemCounter;      // Discard due to no memory

// --- TimeStamp --------------------------------------------------------------

    double       ItsTimeStampFrequency;  // Time stamp timer frequency in Hz
    double       ItsTimeResolutionForMicroSec;
    double       ItsTimeResolutionForMilliSec;


#ifdef RN_CPU_I960
	static HW_UINT32 *TheirTimeStampCounterP;  // Time stamp value pointer - 960 only
#endif	

// --- Status strings ---------------------------------------------------------

    RWCString ItsErrorString;    // Hold the last error string
    RWCString ItsInfoString;     // Hold the last info string



// ****************************************************************************
// ****************************************************************************
// ***
// *** private
// ***
// ****************************************************************************
// ****************************************************************************
private:

// ============================================================================
// ===
// === dtor, copy ctor, operator = (private)
// ===
// ============================================================================
//
// dtor, copy ctor & operator = are declared private and has no implemenation
// this procted from calling them

    // dtor
    ~XdpServices(); 

    // Copy ctor
    XdpServices(const XdpServices &);

    // Operator =
    XdpServices & operator=( const XdpServices) ;


};

#endif

#endif // XdpServices_h

