/* XdpParseParameter.cc  */

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


#include "XdpParseParameter.h"

//-------------------------------------------------------------------------
// public 
//-------------------------------------------------------------------------

// Constructor & Destructor

/// --- ctor ------------------------------------------------------------------
XdpParseParameter::XdpParseParameter(const RWCString &theName, UINT32 theFlags) :

    ItsName(theName),
    ItsFlags(theFlags),
    ItsAssignedFlag(false)

{

    // empty

}

// Copy constructor & Assign op

// Get methods

// --- IsAssigned -------------------------------------------------------------
bool XdpParseParameter::IsAssigned() {

    return (ItsAssignedFlag);

}


// Set methods

// Miscellaneous Operations

// Operators

// Conv Operators

//-------------------------------------------------------------------------
// protected
//-------------------------------------------------------------------------

// Constructor & Destructor

// Get methods

// Set methods

// --- GetNameString ----------------------------------------------------------
// Returns the parameter name
    
RWCString XdpParseParameter::GetNameString() {
    
    return ItsName;
}

// --- GetUsageString ---------------------------------------------------------
// Returns the usage string which is the parameter name with "[ ]" or "< >"

RWCString XdpParseParameter::GetUsageString() {

    RWCString s;

    // Add "[ ]" or "< >" depending if optinal parameter

    if (IsOptional() == true) {
        // Optional
        s = "[" + ItsName + "]";
    }
    else {
        // Not optinal
        s = "<" + ItsName + ">";
    }

    return s;
}
// Miscellaneous Operations

// --- IsOptional -------------------------------------------------------------

bool XdpParseParameter::IsOptional() {
    
    if ( (ItsFlags & XDP_PARSE_PARAMETER_OPTIONAL) ==0 ) {
        return false;
    }
    else {
        return true;
    }

}
// Operators

// Conv Operators

//-------------------------------------------------------------------------
// private
//-------------------------------------------------------------------------

// Constructor & Destructor

// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

// Operators

// Conv Operators

// Data members

// Friend Declarations

