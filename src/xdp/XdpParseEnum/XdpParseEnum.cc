/* XdpParseEnum.cc */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpParseEnum
   
     XdpParseEnum is an auxiliry class to XdpParse. It represent a 
     enum argument. The value is UINT32 which coresponses to one of
     the string/number pairs given in the ctor. It features

     o  Two type of ctors:
        - Simple, upto 10 string. The values are number from zero
        - Flexiable, unlimited string/number pairs, give by an array.
     o  The value is assigned only if the whole parsing completed OK
     o  Supported flag:
            XDP_PARSE_PARAMETER_OPTINAL   - Optinal parameter.
                                            The value must be initialied 
                                            to the default value

    Note that a null string terminates the list

AUTHOR
   Gaash, SB, 2/03


**************************************************************************/


#include <prepc.h>
#include "XdpParseEnum.h"
#include "XdpParse.h"         // To get tokens & match function
#include "Error.h"

//-------------------------------------------------------------------------
// public 
//-------------------------------------------------------------------------

// Constructor & Destructor
 

// --- ctor with strings (simple) ---------------------------------------------
XdpParseEnum::XdpParseEnum(UINT32 &theEnum,
                   const RWCString &theName, const RWCString &theDescription,
                   UINT32 theFlags, 
                   const char *theString0, const char *theString1,
                   const char *theString2, const char *theString3,
                   const char *theString4, const char *theString5,
                   const char *theString6, const char *theString7,
                   const char *theString8, const char *theString9 ) :

    XdpParseParameter(theName, theFlags),
    ItsEnum(theEnum),
    ItsDescription(theDescription),
    ItsNumberOfPairs(0),       // Will be overidden in the ctor
    ItsPairs(ItsSimplePairs),  // Use internal simple pairs array
    ItsTestValue(0),
    ItsHasTestValue(false)
    
{

    // Init the strings pointes of ItsSimplePairs
    ItsSimplePairs[0].stringP = theString0;
    ItsSimplePairs[1].stringP = theString1;
    ItsSimplePairs[2].stringP = theString2;
    ItsSimplePairs[3].stringP = theString3;
    ItsSimplePairs[4].stringP = theString4;
    ItsSimplePairs[5].stringP = theString5;
    ItsSimplePairs[6].stringP = theString6;
    ItsSimplePairs[7].stringP = theString7;
    ItsSimplePairs[8].stringP = theString8;
    ItsSimplePairs[9].stringP = theString9;

    // Find the last non null string & assign the numbers;
    
    UINT32 i;

    ItsNumberOfPairs = 0;

    for(i = 0; i < XDP_PARSE_ENUM_SIMPLE_MAX_PAIRS ; i++) {

        // If null string, break
        if ( ItsSimplePairs[i].stringP == NULL) {
            // NULL
            break;
        }

        // Assign the number
        ItsSimplePairs[i].value = i;


        // Increase the number of pairs
        ItsNumberOfPairs++;

    }

    // All done
}


// --- ctor with string/number array ------------------------------------------
//
// The array is terminated with NULL

XdpParseEnum::XdpParseEnum(UINT32 &theEnum,
                   const RWCString &theName, const RWCString &theDescription,
                   UINT32 theFlags, 
                   const XdpParseEnum::XdpParseEnumPair *thePairsP) :

    XdpParseParameter(theName, theFlags),
    ItsEnum(theEnum),
    ItsDescription(theDescription),
    ItsNumberOfPairs(0),       // Will be overidden in the ctor
    ItsPairs(thePairsP),       // Use the xtor pairs array
    ItsTestValue(0),
    ItsHasTestValue(false)
    
{


    // Find the last non null string;
    
    ItsNumberOfPairs = 0;

    const XdpParseEnum::XdpParseEnumPair *p = thePairsP;

    while (p->stringP != NULL) {
        
        // Increase the number of pairs
        ItsNumberOfPairs++;

        // Next 
        p++;
    }

    // All done
}

  

// Copy constructor & Assign op

// Get methods

// --- GetDescriptionString ---------------------------------------------------
RWCString XdpParseEnum::GetDescriptionString() {

    RWCString s;
    
    // Add type

    s += "enum:";

    // Add description

    s+= ItsDescription;

    // If optional, add default value

    if (IsOptional()) {
        // Optional 
        s += " [" + ToString( ItsEnum ) + "]";

    }
    // Add values

    s+= " Values: ";

    UINT32 i;
    char buf[20];

    for(i=0 ; i < ItsNumberOfPairs; i++) {

        // Convert value to string
        sprintf(buf,"(%u) ",ItsPairs[i].value);

        // Add to string
        s += RWCString(ItsPairs[i].stringP) + RWCString(buf);

    }

    return s;
}

// --- GetErrorString ---------------------------------------------------------
RWCString XdpParseEnum::GetErrorString() {

    return ItsErrorString;
}

// Set methods

// --- GetDebugString ---------------------------------------------------------
RWCString XdpParseEnum::GetDebugString() {

    RWCString s;

    char buf[50];

    s += "Enum=" + ToString( ItsEnum );

    sprintf(buf," Assigned=%d Flags=%08X NumberOfPair=%d",
                  (UINT32)ItsAssignedFlag,
                  ItsFlags,
                  ItsNumberOfPairs);
    
    s += buf;

    return s;

}

// --- Set --------------------------------------------------------------------

// If ItsTestValue is valid then, set ItsTestValue into theEnum.

bool XdpParseEnum::Set() {


    // Check if ItsTestValue has value

    if (ItsHasTestValue == false) {
        // ItsTestValue not value, don't set;
        return false;
    }

    // Set it

    ItsEnum = ItsTestValue;

    // Mark assigned
    ItsAssignedFlag = true;

    return true;
}

// Miscellaneous Operations

// --- PreSetTest -------------------------------------------------------------
bool XdpParseEnum::PreSetTest(XdpParse *theParser) {


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
    UINT32 value;
	RWCString errStr;

    OK = FromString(token, value, errStr);

    if (OK == false) {
        // Failure
        ItsErrorString = errStr;
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
// Returns theValue as string

RWCString XdpParseEnum::ToString(UINT32 theValue) {

    RWCString s;

    // Convert value to string

    RWCString valStr;

    UINT32 i;

    // Scan pairs array
    for(i=0 ; i < ItsNumberOfPairs; i++) {

        // Is value match to pair
        if (ItsPairs[i].value == theValue) {
            // Match, get string and break
            valStr = RWString(ItsPairs[i].stringP);
            break;
        }
    }

    // build string

    char buf[20];

    sprintf(buf,"(%u)", theValue);

        // Add to string
    s += valStr+buf;

    return s;

}


// --- FromString ---------------------------------------------------------------
// Convert string to number depending on the format. Return true if OK
// If an error occured, set the error string.
//
// match paritally, case insensative
//
bool XdpParseEnum::FromString(RWCString &theString, UINT32 &theValue, RWCString &theErrorStr) {

    theValue=0; //Just in case

    UINT32 i;

    UINT32 matchCount = 0;
    UINT32 matchIndex;
 
    UINT32 charIndex;          // match char index (tmp var)

	RWCString pairName;        // Currnet pair name string
	RWCString matchedNames;    // List of matched pair name

	// Scan pairs array for theString
    for(i=0 ; i < ItsNumberOfPairs; i++) {

		// Set currnet pair string
		pairName = ItsPairs[i].stringP;

		// Check exact match 
		if (theString == pairName) {
			// Exact match

			// Force one match
			matchCount = 1;
			matchIndex = i;
		
			// Stop searching
			break;

		}

        // Is value match to pair ?
        charIndex = pairName.index(theString, 0, RWCString::ignoreCase);
        
	    if (charIndex == 0) {

            // Match, increase match count and save the index
            matchCount++;
            matchIndex = i;

			// Add to the macthed list
			matchedNames += " " + pairName;
                    
        }
    }

	
	switch(matchCount) {

		case 0: // No match - Error
			theErrorStr = "Failed to convert \"" + theString + "\" to enum, no match";
			return false;

		case 1: // One match - OK - set value and return OK
    
	        theValue = ItsPairs[matchIndex].value;
		    return true;           
		default: // One than one match
			theErrorStr = "Failed to convert \"" + theString + 
				           "\" to enum, multiple matches:" + matchedNames;
			return false;

    }


    // Should never be here, just in case
	theErrorStr = "FromString internal error";
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

