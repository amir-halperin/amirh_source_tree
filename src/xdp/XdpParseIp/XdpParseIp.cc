/* XdpParseIp.cc */

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


#include <prepc.h>
#include "XdpParseIp.h"
#include "XdpParse.h"         // To get tokens
#include "Error.h"

//-------------------------------------------------------------------------
// public 
//-------------------------------------------------------------------------

// Constructor & Destructor



// --- ctor UINT32 ------------------------------------------------------------
XdpParseIp::XdpParseIp(IpAddress &theIpAddress,
                   const RWCString &theName, const RWCString &theDescription,
                   UINT32 theFlags) :
    XdpParseParameter(theName, theFlags),
    ItsIpAddress(theIpAddress),
    ItsDescription(theDescription),
    ItsHasTestValue(false)
    
{

    //empty
}

    

// Copy constructor & Assign op

// Get methods

        
// --- GetDescriptionString ---------------------------------------------------
RWCString XdpParseIp::GetDescriptionString() {

    RWCString s;
    
    // Add type

    s += "IP address ";

    // Add description

    s+= ItsDescription;

    // If optional, add default value

    if (IsOptional()) {
        // Optional 
        s += " [" + ToString( ItsIpAddress ) + "]";

    }

    return s;
}

// --- GetErrorString ---------------------------------------------------------
RWCString XdpParseIp::GetErrorString() {

    return ItsErrorString;
}

// Set methods

// --- GetDebugString ---------------------------------------------------------
RWCString XdpParseIp::GetDebugString() {

    RWCString s;

    char buf[50];

    s += "IpAddr=" + ToString( ItsIpAddress );

    sprintf(buf," Assigned=%d Flags=%08X ",
                  (UINT32)ItsAssignedFlag,
                  ItsFlags);
        
    s += buf;
   
    return s;

}

// --- Set --------------------------------------------------------------------

// If ItsTestValue is valid then, set ItsTestValue into ItsIpAddress

bool XdpParseIp::Set() {


    // Check if ItsTestValue has value

    if (ItsHasTestValue == false) {
        // ItsTestValue not value, don't set;
        return false;
    }

    // Set it

    ItsIpAddress = ItsTestValue;
    
    // Mark assigned
    ItsAssignedFlag = true;

    return true;
}

// Miscellaneous Operations

// --- PreSetTest -------------------------------------------------------------
bool XdpParseIp::PreSetTest(XdpParse *theParser) {


    ItsHasTestValue = false; // ItsTestValue is not valid yet

    // Get Token
    
    RWCString token;
    bool OK;
    
    OK = theParser->GetToken(token);

    // Check if we don't have token. If not and the parameter is optinal,OK, else error
    
    if (OK == false) {
        // No token , if optional OK, else error
        if (IsOptional()) {
            // Optional, all OK
            return true;
        }
        else {
            // Not optional, failure;
            ItsErrorString = "Non-optional parameter is missing";
            return false;
        }
    }

    // Try to convert to an IP address
    IpAddress ipAddr;

    OK = FromString(token, ipAddr);

    if (OK == false) {
        // Failure
        ItsErrorString = "Failed to convert \"" + token + "\" to a IP address.";
        return false;
    }


    // All OK, save test value

    ItsTestValue    = ipAddr;
    ItsHasTestValue = true;

    return true;


}
// Operators

// Conv Operators

//-------------------------------------------------------------------------
// protected
//-------------------------------------------------------------------------

// Constructor & Destructor

// Get methods


// Set methods


// Miscellaneous Operations


// --- ToString ---------------------------------------------------------------
// Returns theValue IP address as string

RWCString XdpParseIp::ToString(IpAddress &theValue) {

    RWCString s;

    // Convert to string

    s = theValue.ToStr();
    
    return s;

}

// --- FromString ---------------------------------------------------------------
// Convert string to IP Address. Return true if OK.

bool XdpParseIp::FromString(RWCString &theString, IpAddress &theValue) {

    // Convert the string into IP address in a tmp variable
    IpAddress tmpIP(theString.data());

    // Is it valid IP address

    bool rc;
    rc = tmpIP.IsValid();
    if (rc == true) {
        // Valid, assign it and return true
        theValue = tmpIP;
        return true;
    }

    // Failed

    return false;
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

