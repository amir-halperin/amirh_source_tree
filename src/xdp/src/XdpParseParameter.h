/* XdpParseParameter.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpParseParameter
   This is an abstruct base class that represet a parameter to XdpParse.

AUTHOR
   Gaash. SB, 2/03


**************************************************************************/

#ifndef XdpParseParameter_h
#define XdpParseParameter_h

#include "basictypes.h"
#include "RWHdr/cstring.h"


//----------------------------
// Forward Declarations
//----------------------------

class XdpParse; // To get the token/line

// Global defintion
enum { XDP_PARSE_PARAMETER_OPTIONAL  = 1<<31 };  // Parameter is optional 


class XdpParseParameter 
{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------

// Typedef & Enum

// Constructor & Destructor

    XdpParseParameter(const RWCString &theName, UINT32 theFlags);

// Copy constructor & Assign op

// Get methods

    virtual RWCString GetNameString();
    virtual RWCString GetUsageString();

    virtual RWCString GetDescriptionString() = 0;
    virtual RWCString GetErrorString() = 0;
    virtual RWCString GetDebugString() = 0;

// Set methods

// Miscellaneous Operations

    virtual bool      PreSetTest(XdpParse *theParser)=0; // to get tokens
    virtual bool      Set()=0;
    virtual bool      IsAssigned();

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

    bool IsOptional();
    
// Operators

// Conv Op

// Data members

    RWCString   ItsName;
    UINT32      ItsFlags;
    bool        ItsAssignedFlag;


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

#endif // XdpParseParameter_h

