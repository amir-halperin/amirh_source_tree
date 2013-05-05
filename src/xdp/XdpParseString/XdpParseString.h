/* XdpParseString.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpParseString
   
     XdpParseString is an auxiliry class to XdpParse. It represent a 
     string argument. It features

     o  Get a signle token or get the rest of the line
     o  The value is assigned only if the whole parsing completed OK
     o  Supported flags:
            XDP_PARSE_PARAMETER_OPTINAL   - Optinal parameter.
                                            The value must be initialied 
                                            to the default value
            XDP_PARSE_STRING_REST_OF_LINE - Get till rest of line

AUTHOR
   Gaash, SB, 2/03


**************************************************************************/

#ifndef XdpParseString_h
#define XdpParseString_h

#include "basictypes.h"
#include "RWHdr/cstring.h"
#include "XdpParseParameter.h"

//----------------------------
// Forward Declarations
//----------------------------

// Global defintion

    enum { XDP_PARSE_STRING_REST_OF_LINE  = 1<<0 };  // Get till rest of line

class XdpParseString : virtual public XdpParseParameter

{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------

// Typedef & Enum

// Constructor & Destructor


    // RWCString
    XdpParseString(RWCString &theString,
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

// Set methods

// Miscellaneous Operations

    bool IsRestOfLine()      { return ((ItsFlags & XDP_PARSE_STRING_REST_OF_LINE) != 0) 
                                        ? true:false; };

// Operators

// Conv Op

// Data members

    RWCString   &ItsString;      // The call string

    RWCString   ItsDescription;
    RWCString   ItsErrorString;

    // Internal data
    RWCString   ItsTestValue;    // Hold the value between "Test" to "Set"
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

#endif // XdpParseString_h

