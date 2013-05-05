/* XdpMngr.h   */

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

#ifndef XdpMngr_h
#define XdpMngr_h

#include "basictypes.h"
#include "IpPortNumber.h"

// ****************************************************************************
// ***
// *** XdpMngr class declaration
// ***
// ****************************************************************************

class XdpMngrCmdMngr;
class IpAddress;

class XdpMngr

{

    friend class XdpMngrCmdMngr;

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

    XdpMngr();

    static XdpMngr * GetIt();

// ============================================================================
// ===
// === Output service functions (public)
// ===
// ============================================================================

    // Output to UDP

    void    OutputToUdpSetIP(const IpAddress &theIP);
    void    OutputToUdpSetPort(const IpPortNumber &thePort);
    void    OutputToUdpSetTaskPriority(UINT32 theTaskPriority);
    void    OutputToUdpSetQueueSize(UINT32 theQueueSize);
    void    OutputToUdpSetMaxMsgSize(UINT32 theMaxMsgSize);
    void    OutputToUdpEnable(bool theEnable);

    // Output to serial
    void    OutputToSerialSetTaskPriority(UINT32 theTaskPriority);
    void    OutputToSerialSetQueueSize(UINT32 theQueueSize);
    void    OutputToSerialSetMaxMsgSize(UINT32 theMaxMsgSize);
    void    OutputToSerialEnable(bool theEnable);

// ============================================================================
// ===
// === Time stamp service functions (public)
// ===
// ============================================================================


    void     TimeStampSetFrequency(double theFrequency);

// ============================================================================
// ===
// === Creation & start functions (public)
// ===
// ============================================================================

    // Internal

    void         CreateIndependent();
    void         StartIndependent();

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

// ============================================================================
// ===
// === Specific creation functions (protected)
// ===
// ============================================================================

    virtual void SpecificCreateIndependent() { };



// ============================================================================
// ===
// === WIN32 support functions (protected)
// ===
// ============================================================================


// ============================================================================
// ===
// === Data members (protected)
// ===
// ============================================================================

    static XdpMngr *TheirP;

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
  //    ~XdpMngr(); 

    // Copy ctor
    XdpMngr(const XdpMngr &);

    // Operator =
    XdpMngr & operator=( const XdpMngr) ;

};


#endif // XdpMngr_h

