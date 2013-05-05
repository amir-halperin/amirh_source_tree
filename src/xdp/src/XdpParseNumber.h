/* XdpParseNumber.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpParseNumber
   
     XdpParseNumber is an auxiliry class to XdpParse. It represent a 
     number argument. It features

     o  8,16,32 bit long numbers
     o  Signed or unsigned
     o  Decimal or hex formats
     o  Minimum & Maximum limits are checked
     o  Supported data types are UINT8, UINT16, UINT32, INT8, INT16, INT32
     o  The value is assigned only if the whole parsing completed OK
     o  Supported flags:
            XDP_PARSE_PARAMETER_OPTINAL   - Optinal parameter.
                                            The value must be initialied 
                                            to the default value
            XDP_PARSE_NUMBER_HEX          - Use hex format

AUTHOR
   Gaash, SB, 2/03


**************************************************************************/

#ifndef XdpParseNumber_h
#define XdpParseNumber_h

#include "basictypes.h"
#include "RWHdr/cstring.h"
#include "XdpParseParameter.h"

//----------------------------
// Forward Declarations
//----------------------------

// Global defintion

    enum { XDP_PARSE_NUMBER_HEX  = 1<<0 };  // Parameter is hex

class XdpParseNumber : virtual public XdpParseParameter

{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------

// Typedef & Enum

// Constructor & Destructor


    // UINT32
    XdpParseNumber(UINT32 &theNumber,
                   const RWCString &theName, const RWCString &theDescription,
                   UINT32 theFlags, 
                   UINT32 theMin=0, UINT32 theMax=0xFFFFFFFF);

    // UINT16
    XdpParseNumber(UINT16 &theNumber,
                   const RWCString &theName, const RWCString &theDescription,
                   UINT32 theFlags, 
                   UINT16 theMin=0, UINT16 theMax=0xFFFF);

    // UINT8
    XdpParseNumber(UINT8 &theNumber,
                   const RWCString &theName, const RWCString &theDescription,
                   UINT32 theFlags, 
                   UINT8  theMin=0, UINT8 theMax=0xFF);


    // INT32
    XdpParseNumber(INT32 &theNumber,
                   const RWCString &theName, const RWCString &theDescription,
                   UINT32 theFlags, 
                   INT32  theMin=(-2147483647-1), INT32 theMax=0x7FFFFFFF);

    // INT16
    XdpParseNumber(INT16 &theNumber,
                   const RWCString &theName, const RWCString &theDescription,
                   UINT32 theFlags, 
                   INT16  theMin=-32768, INT16 theMax=0x7FFF);

    // INT8
    XdpParseNumber(INT8 &theNumber,
                   const RWCString &theName, const RWCString &theDescription,
                   UINT32 theFlags, 
                   INT8   theMin=-128, INT8 theMax=0x7F);


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

    bool IsHex()      { return ((ItsFlags & XDP_PARSE_NUMBER_HEX) != 0) ? true:false; };
 
    INT32 SignExtension(UINT32 theValue);

    RWCString ToString(UINT32 theValue);

    bool FromString(RWCString &theString, UINT32 &theValue);

    bool CheckLimits(UINT32 theValue);

// Operators

// Conv Op

// Data members

    void        *ItsNumberP;    // Pointer type is void because 8/16/32 bit

    // UINT32 is the bigest size. It is used with care for signed value too

    UINT32      ItsMinValue;
    UINT32      ItsMaxValue;

    RWCString   ItsDescription;
    RWCString   ItsErrorString;

    // Internal data
    UINT32      ItsTestValue;    // Hold the value between "Test" to "Set"
    bool        ItsHasTestValue; // ItsTestValue is valid, need to set it.
    UINT32      ItsSizeInBits;
    bool        ItsSignedFlag;

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

#endif // XdpParseNumber_h

