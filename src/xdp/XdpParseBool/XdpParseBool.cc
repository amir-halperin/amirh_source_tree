/* XdpParseBool.cc */

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


#include <prepc.h>
#include "XdpParseBool.h"
#include "XdpParse.h"         // To get tokens
#include "Error.h"

//-------------------------------------------------------------------------
// public 
//-------------------------------------------------------------------------

// Constructor & Destructor



// --- ctor UINT32 ------------------------------------------------------------
XdpParseBool::XdpParseBool(bool &theBool,
                   const RWCString &theName, const RWCString &theDescription,
                   UINT32 theFlags) :
    XdpParseParameter(theName, theFlags),
    ItsBool(theBool),
    ItsDescription(theDescription),
    ItsTestValue(false),
    ItsHasTestValue(false)
    
{

    //empty
}

    

// Copy constructor & Assign op

// Get methods

        
// --- GetDescriptionString ---------------------------------------------------
RWCString XdpParseBool::GetDescriptionString() {

    RWCString s;
    
    // Add type

    s += "bool ";

    // Add description

    s+= ItsDescription;

    // If optional, add default value

    if (IsOptional()) {
        // Optional 
        s += " [" + ToString( ItsBool ) + "]";

    }

    return s;
}

// --- GetErrorString ---------------------------------------------------------
RWCString XdpParseBool::GetErrorString() {

    return ItsErrorString;
}

// Set methods

// --- GetDebugString ---------------------------------------------------------
RWCString XdpParseBool::GetDebugString() {

    RWCString s;

    char buf[50];

    s += "Bool=" + ToString( ItsBool );

    sprintf(buf," Assigned=%d Flags=%08X ",
                  (UINT32)ItsAssignedFlag,
                  ItsFlags);
        
    s += buf;
   
    return s;

}

// --- Set --------------------------------------------------------------------

// If ItsTestValue is valid then, set ItsTestValue into ItsBool

bool XdpParseBool::Set() {


    // Check if ItsTestValue has value

    if (ItsHasTestValue == false) {
        // ItsTestValue not value, don't set;
        return false;
    }

    // Set it

    ItsBool = ItsTestValue;
    
    // Mark assigned
    ItsAssignedFlag = true;

    return true;
}

// Miscellaneous Operations

// --- PreSetTest -------------------------------------------------------------
bool XdpParseBool::PreSetTest(XdpParse *theParser) {


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

    // Try to convert to number
    bool value;

    OK = FromString(token, value);

    if (OK == false) {
        // Failure
        ItsErrorString = "Failed to convert \"" + token + "\" to boolean {false,true}.";
        return false;
    }


    // All OK, save test value

    ItsTestValue    = value;
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
// Returns theValue as string, "true" or "false"

RWCString XdpParseBool::ToString(bool theValue) {

    RWCString s;

    // Convert to string, Is false ?
    if (theValue==false) {
        // False
        s = "false";
    }
    else {
        // True
        s = "true";
    }
    
    return s;

}

// --- FromString ---------------------------------------------------------------
// Convert string to boolean. Return true if OK.

bool XdpParseBool::FromString(RWCString &theString, bool &theValue) {

    // List of word to be consider for true & false

    const static char *trueList[] ={ "true" , "t", "on" , "1", "en" ,       "enable" , "yes","y", NULL };
    const static char *falseList[]={ "false", "f", "off", "0", "di", "dis", "disable", "no", "n", NULL };

    theValue=false; //Just in case

    // Is false - check the false list
    const char **p;
    for(p = falseList ; *p != NULL ; p++) {
        if (theString == *p) {
            // False
            theValue=false;
            return true; // OK
        }
    }

    // Is true - check the true list
    for(p = trueList ; *p != NULL ; p++) {
        if (theString == *p) {
            // True
            theValue=true;
            return true; // OK
        }
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

