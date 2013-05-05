/* XdpParseString.cc */

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

#include "prepc.h"
#include "XdpParseString.h"
#include "XdpParse.h"         // To get tokens

//-------------------------------------------------------------------------
// public 
//-------------------------------------------------------------------------

// Constructor & Destructor




// --- ctor RWCString ---------------------------------------------------------
XdpParseString::XdpParseString(RWCString &theString,
                   const RWCString &theName, const RWCString &theDescription,
                   UINT32 theFlags) :
    XdpParseParameter(theName, theFlags),
    ItsString(theString),
    ItsDescription(theDescription),
    ItsHasTestValue(false)    
{

    //empty
}


// Copy constructor & Assign op

// Get methods

        
// --- GetDescriptionString ---------------------------------------------------
RWCString XdpParseString::GetDescriptionString() {

    RWCString s;
    
    // Add type

    if (IsRestOfLine()) {
        // Rest of line
        s += "String(rest-of-line) ";
    }
    else {
        // Normal
        s += "String ";
    }

    // Add description

    s+= ItsDescription;

    // If optional, add default value

    if (IsOptional()) {
        // Optional 
        s += " [\"" + ItsString +"\"]";

    }

    return s;
}

// --- GetErrorString ---------------------------------------------------------
RWCString XdpParseString::GetErrorString() {

    return ItsErrorString;
}

// --- GetDebugString ---------------------------------------------------------
RWCString XdpParseString::GetDebugString() {

    RWCString s;

    char buf[50];

    s += "String=\"" + ItsString + "\"";

    sprintf(buf," Assigned=%d Flags=%08X",
                  (UINT32)ItsAssignedFlag,
                  ItsFlags);

    s += buf;
    
    return s;

}

// --- Set --------------------------------------------------------------------

// If ItsTestValue is valid then, set ItsTestValue into ItsString, using the right size

bool XdpParseString::Set() {


    // Check if ItsTestValue has value

    if (ItsHasTestValue == false) {
        // ItsTestValue not value, don't set;
        return false;
    }

    // Set it

    ItsString=ItsTestValue;

    
    // Mark assigned
    ItsAssignedFlag = true;

    return true;
}

// Miscellaneous Operations


// --- PreSetTest -------------------------------------------------------------
bool XdpParseString::PreSetTest(XdpParse *theParser) {


    ItsHasTestValue = false; // ItsTestValue is not valid yet

    // Get Token
    
    RWCString token;
    bool OK;

    // Get token call is deterement by the rest-of-line flag
    if (IsRestOfLine()) {
        // RestOfLine
        OK = theParser->GetRestOfLine(token);
    }
    else {
        // standard token
        OK = theParser->GetToken(token);
    }

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

    // All OK, save test value

    ItsTestValue    = token;
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

