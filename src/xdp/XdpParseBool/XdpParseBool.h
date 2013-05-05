/* XdpParseBool.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpParseBool
   
     XdpParseBool is an auxiliry class to XdpParse. It represent a 
     boolean argument. The only values are 'true' & 'false'.

     Supported flag:
            XDP_PARSE_PARAMETER_OPTINAL   - Optinal parameter.
                                            The value must be initialied 
                                            to the default value

AUTHOR
   Gaash, SB, 2/03


**************************************************************************/

#ifndef XdpParseBool_h
#define XdpParseBool_h

#include "basictypes.h"
#include "RWHdr/cstring.h"
#include "XdpParseParameter.h"

//----------------------------
// Forward Declarations
//----------------------------

// Global defintion


class XdpParseBool : virtual public XdpParseParameter

{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------

// Typedef & Enum

// Constructor & Destructor


    // bool
    XdpParseBool(bool &theBool,
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


    RWCString ToString(bool theValue);
 
    bool FromString(RWCString &theString, bool &theValue);

// Operators

// Conv Op

// Data members

    bool        &ItsBool;    // The caller bool var

    RWCString   ItsDescription;
    RWCString   ItsErrorString;

    // Internal data
    bool        ItsTestValue;    // Hold the value between "Test" to "Set"
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

#endif // XdpParseBool_h

