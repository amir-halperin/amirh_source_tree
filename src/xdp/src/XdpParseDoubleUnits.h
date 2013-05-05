/* XdpParseDoubleUnits.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpParseDoubleUnits
   
     XdpParseDoubleUnits is an auxiliry class to XdpParse. It represent a
     floating point number (double). The number notaion is stadard C 
     notation. An optinal unit suffix scales the value.

     The units are defined in XdpConv:
     p = 10^-12
     n = 10^-9
     u = 10^-6
     m = 10^-3
     K = 10^3
     M = 10^6
     G = 10^9
     T = 10^12

     Supported flag:
            XDP_PARSE_PARAMETER_OPTINAL   - Optinal parameter.
                                            The value must be initialied 
                                            to the default value

AUTHOR
   Gaash, SB, 5/02


**************************************************************************/

#ifndef XdpParseDoubleUnits_h
#define XdpParseDoubleUnits_h

#include "basictypes.h"
#include "RWHdr/cstring.h"
#include "XdpParseParameter.h"

//----------------------------
// Forward Declarations
//----------------------------

// Global defintion


class XdpParseDoubleUnits : virtual public XdpParseParameter

{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------

// Typedef & Enum

// Constructor & Destructor


    // bool
    XdpParseDoubleUnits(double &theValue,
                   const RWCString &theName, const RWCString &theDescription,
                   UINT32 theFlags, double theMin, double theMax);


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


    RWCString ToString(double theValue);
 
    bool FromString(RWCString &theString, double &theNumber);

// Operators

// Conv Op

// Data members

    double      &ItsValue;    // The caller var
    double      ItsMinValue;
    double      ItsMaxValue;

    RWCString   ItsDescription;
    RWCString   ItsErrorString;

    // Internal data
    double      ItsTestValue;    // Hold the value between "Test" to "Set"
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

#endif // XdpParseDoubleUnits_h

