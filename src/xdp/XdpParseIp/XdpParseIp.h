/* XdpParseIp.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpParseIp
   
     XdpParseIp is an auxiliry class to XdpParse. It represent an IP
     address argument with the notaion xx.xx.xx.xx

     Supported flag:
            XDP_PARSE_PARAMETER_OPTINAL   - Optinal parameter.
                                            The value must be initialied 
                                            to the default value

AUTHOR
   Gaash, SB, 5/02


**************************************************************************/

#ifndef XdpParseIp_h
#define XdpParseIp_h

#include "basictypes.h"
#include "RWHdr/cstring.h"
#include "XdpParseParameter.h"
#include "IpAddress.h"

//----------------------------
// Forward Declarations
//----------------------------

// Global defintion


class XdpParseIp : virtual public XdpParseParameter

{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------

// Typedef & Enum

// Constructor & Destructor


    // bool
    XdpParseIp(IpAddress &theIpAddress,
                   const RWCString &theName, const RWCString &theDescription,
                   UINT32 theFlags);


// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

// Typedef & Enum

// Constructor & Destructor

// Copy constructor & Assign op

// Get methods

    virtual RWCString GetDescriptionString();
    virtual RWCString GetErrorString();
    virtual RWCString GetDebugString();

// Set methods

// Miscellaneous Operations

    virtual bool      PreSetTest(XdpParse *theParser); // to get tokens
    virtual bool      Set();
 
// Operators

// Conv Op

// RTTI
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

    UINT32 GetNumber();

  
// Set methods

// Miscellaneous Operations


    RWCString ToString(IpAddress &IpAddress);
 
    bool FromString(RWCString &theString, IpAddress &theIpAddress);

// Operators

// Conv Op

// Data members

    IpAddress   &ItsIpAddress;    // The caller var

    RWCString   ItsDescription;
    RWCString   ItsErrorString;

    // Internal data
    IpAddress   ItsTestValue;    // Hold the value between "Test" to "Set"
    bool        ItsHasTestValue; // ItsTestValue is valid, need to set it.

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

#endif // XdpParseIp_h

