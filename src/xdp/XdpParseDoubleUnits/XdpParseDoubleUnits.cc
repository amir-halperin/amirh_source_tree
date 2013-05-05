/* XdpParseDoubleUnits.cc */

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


#include "prepc.h"
#include "XdpParseDoubleUnits.h"
#include "XdpParse.h"
#include "XdpConv.h"
#include "Error.h"

//-------------------------------------------------------------------------
// public 
//-------------------------------------------------------------------------

// Constructor & Destructor



// --- ctor UINT32 ------------------------------------------------------------
XdpParseDoubleUnits::XdpParseDoubleUnits(double &theValue,
                   const RWCString &theName, const RWCString &theDescription,
                   UINT32 theFlags, double theMin, double theMax) :
    XdpParseParameter(theName, theFlags),
    ItsValue(theValue),
    ItsDescription(theDescription),
    ItsMinValue(theMin),
    ItsMaxValue(theMax),
    ItsHasTestValue(false)
    
{

    //empty
}

    

// Copy constructor & Assign op

// Get methods

        
// --- GetDescriptionString ---------------------------------------------------
RWCString XdpParseDoubleUnits::GetDescriptionString() {

    RWCString s;
    
    // Add type

    s += "double w/units ";

    // Add description

    s+= ItsDescription;

    // If optional, add default value

    if (IsOptional()) {
        // Optional 
        s += " [" + ToString( ItsValue ) + "]";

    }

    return s;
}

// --- GetErrorString ---------------------------------------------------------
RWCString XdpParseDoubleUnits::GetErrorString() {

    return ItsErrorString;
}

// Set methods

// --- GetDebugString ---------------------------------------------------------
RWCString XdpParseDoubleUnits::GetDebugString() {

    RWCString s;

    char buf[50];

    s += "Value=" + ToString( ItsValue );

    sprintf(buf," Assigned=%d Flags=%08X ",
                  (UINT32)ItsAssignedFlag,
                  ItsFlags);
        
    s += buf;

    s += " Min=" + ToString(ItsMinValue) + " Max="+ ToString(ItsMaxValue);

    return s;

}

// --- Set --------------------------------------------------------------------

// If ItsTestValue is valid then, set ItsTestValue into double 

bool XdpParseDoubleUnits::Set() {


    // Check if ItsTestValue has value

    if (ItsHasTestValue == false) {
        // ItsTestValue not value, don't set;
        return false;
    }

    // Set it

    ItsValue = ItsTestValue;
    
    // Mark assigned
    ItsAssignedFlag = true;

    return true;
}

// Miscellaneous Operations

// --- PreSetTest -------------------------------------------------------------
bool XdpParseDoubleUnits::PreSetTest(XdpParse *theParser) {


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

    // Try to convert to a double with units
    double val;

    OK = FromString(token, val);

    if (OK == false) {
        // Failure
        ItsErrorString = "Failed to convert \"" + token + "\" to a double w/units.";
        return false;
    }

    // Check limits

    if ((val < ItsMinValue) || (val > ItsMaxValue)) {
        // Failure;
        ItsErrorString = "The value " + ToString(val) + 
                         " is outside of the valid range: " + 
                         ToString(ItsMinValue) + " to " + 
                         ToString(ItsMaxValue) + ".";
        return false;
    }

    // All OK, save test value

    ItsTestValue    = val;
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

RWCString XdpParseDoubleUnits::ToString(double theValue) {

    RWCString s;

    // Convert to string

    s = XdpConv::ToUnitsStr(theValue);
    
    return s;

}

// --- FromString ---------------------------------------------------------------
// Convert string to double with units. Return true if OK.

bool XdpParseDoubleUnits::FromString(RWCString &theString, double &theValue) {

    bool rc;

    // Convert the string into IP address in a tmp variable
    double tmpDouble;
    
    rc = XdpConv::DoubleFromUnitsStr(theString.data(), &tmpDouble);

    if (rc == true) {
        // Valid, assign it and return true
        theValue = tmpDouble;
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

