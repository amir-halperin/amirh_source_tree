/* XdpParseNumber.cc */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpParseNumber

     XdpParseNumber is an auxiliry class to XdpParse. It represent a
     number argument. It features

     o  8,16,32 bit long numbers
     o  Signed or unsigned
     o  Decimal or hex formats
     o  Minimum & Maximum limits are checked
     o  Supported data types are UINT8, UINT16, UINT32, INT8, INT16, INT32
     o  The value is assigned only if the whole parsing completed OK
     o  Supported flags:
            XDP_PARSE_OPTIONAL    - Optinal parameter.
                                    The value must be initialied
                                    to the default value
            XDP_PARSE_NUMBER_HEX  - Use hex format
     o  [v0.21] The "_" character is ignore inside the number. 123_456_789
        is a valid number.

AUTHOR
   Gaash, SB, 2/03


**************************************************************************/

#include <prepc.h>
#include "XdpParseNumber.h"
#include "XdpParse.h"         // To get tokens
#include "Error.h"
#include <ctype.h>

//-------------------------------------------------------------------------
// public
//-------------------------------------------------------------------------

// Constructor & Destructor



// --- ctor UINT32 ------------------------------------------------------------
XdpParseNumber::XdpParseNumber(UINT32 &theNumber,
                               const RWCString &theName, const RWCString &theDescription,
                               UINT32 theFlags,
                               UINT32 theMin, UINT32 theMax) :
XdpParseParameter(theName, theFlags),
ItsNumberP( (void *) &theNumber),
ItsDescription(theDescription),
ItsMinValue(theMin),
ItsMaxValue(theMax),
ItsSizeInBits(32),
ItsSignedFlag(false),
ItsTestValue(0),
ItsHasTestValue(false)
{

    //empty
}


// --- ctor UINT16 ------------------------------------------------------------
XdpParseNumber::XdpParseNumber(UINT16 &theNumber,
                               const RWCString &theName, const RWCString &theDescription,
                               UINT32 theFlags,
                               UINT16 theMin, UINT16 theMax) :
XdpParseParameter(theName, theFlags),
ItsNumberP( (void *) &theNumber),
ItsDescription(theDescription),
ItsMinValue(theMin),
ItsMaxValue(theMax),
ItsSizeInBits(16),
ItsSignedFlag(false),
ItsTestValue(0),
ItsHasTestValue(false)
{

    //empty
}

// --- ctor UINT8 -------------------------------------------------------------
XdpParseNumber::XdpParseNumber(UINT8 &theNumber,
                               const RWCString &theName, const RWCString &theDescription,
                               UINT32 theFlags,
                               UINT8 theMin, UINT8 theMax) :
XdpParseParameter(theName, theFlags),
ItsNumberP( (void *) &theNumber),
ItsDescription(theDescription),
ItsMinValue(theMin),
ItsMaxValue(theMax),
ItsSizeInBits(8),
ItsSignedFlag(false),
ItsTestValue(0),
ItsHasTestValue(false)
{

    //empty
}


// --- ctor INT32 -------------------------------------------------------------
XdpParseNumber::XdpParseNumber(INT32 &theNumber,
                               const RWCString &theName, const RWCString &theDescription,
                               UINT32 theFlags,
                               INT32 theMin, INT32 theMax) :
XdpParseParameter(theName, theFlags),
ItsNumberP( (void *) &theNumber),
ItsDescription(theDescription),
ItsMinValue((INT32)theMin),
ItsMaxValue((INT32)theMax),
ItsSizeInBits(32),
ItsSignedFlag(true),
ItsTestValue(0),
ItsHasTestValue(false)
{

    //empty
}


// --- ctor INT16 -------------------------------------------------------------
XdpParseNumber::XdpParseNumber(INT16 &theNumber,
                               const RWCString &theName, const RWCString &theDescription,
                               UINT32 theFlags,
                               INT16 theMin, INT16 theMax) :
XdpParseParameter(theName, theFlags),
ItsNumberP( (void *) &theNumber),
ItsDescription(theDescription),
ItsMinValue((INT32)theMin),
ItsMaxValue((INT32)theMax),
ItsSizeInBits(16),
ItsSignedFlag(true),
ItsTestValue(0),
ItsHasTestValue(false)
{

    //empty
}

// --- ctor INT8 --------------------------------------------------------------
XdpParseNumber::XdpParseNumber(INT8 &theNumber,
                               const RWCString &theName, const RWCString &theDescription,
                               UINT32 theFlags,
                               INT8 theMin, INT8 theMax) :
XdpParseParameter(theName, theFlags),
ItsNumberP( (void *) &theNumber),
ItsDescription(theDescription),
ItsMinValue((INT32)theMin),
ItsMaxValue((INT32)theMax),
ItsSizeInBits(8),
ItsSignedFlag(true),
ItsTestValue(0),
ItsHasTestValue(false)
{

    //empty
}


// Copy constructor & Assign op

// Get methods


// --- GetDescriptionString ---------------------------------------------------
RWCString XdpParseNumber::GetDescriptionString()
{

    RWCString s;

    // Add Size and type

    char buf[30];

    // Is signed
    if (ItsSignedFlag)
    {
        // Signed
        sprintf(buf,"INT%d",ItsSizeInBits);
    } else
    {
        // Unsigned
        sprintf(buf,"UINT%d",ItsSizeInBits);
    }

    s += buf;


    // Add Hex/Dec
    if (IsHex())
    {
        // Hex
        s += "(hex) ";
    } else
    {
        // Dec
        s += "(dec) ";
    }


    // Add description

    s+= ItsDescription;

    // If optional, add default value

    if (IsOptional())
    {
        // Optional
        s += " [" + ToString( GetNumber() ) + "]";

    }

    return s;
}

// --- GetErrorString ---------------------------------------------------------
RWCString XdpParseNumber::GetErrorString()
{

    return ItsErrorString;
}

// Set methods

// --- GetDebugString ---------------------------------------------------------
RWCString XdpParseNumber::GetDebugString()
{

    RWCString s;

    char buf[50];

    s += "Number=" + ToString( GetNumber() );

    sprintf(buf," Assigned=%d Flags=%08X Size=%d Signed=%d ",
            (UINT32)ItsAssignedFlag,
            ItsFlags,
            ItsSizeInBits,
            (UINT32)ItsSignedFlag);

    s += buf;

    s += " Min=" + ToString(ItsMinValue) + " Max="+ ToString(ItsMaxValue);

    return s;

}

// --- Set --------------------------------------------------------------------

// If ItsTestValue is valid then, set ItsTestValue into theNumberP, using the right size

bool XdpParseNumber::Set()
{


    // Check if ItsTestValue has value

    if (ItsHasTestValue == false)
    {
        // ItsTestValue not value, don't set;
        return false;
    }

    // Set it

    switch (ItsSizeInBits)
    {

    case 8:
        *(UINT8 *) ItsNumberP = (UINT8)  ItsTestValue;
        break;

    case 16:
        *(UINT16 *)ItsNumberP = (UINT16) ItsTestValue;
        break;

    case 32:
        *(UINT32 *)ItsNumberP = (UINT32) ItsTestValue;
        break;

    default:
        RN_ASSERT(0,"Invalid size");
        break;
    }

    // Mark assigned
    ItsAssignedFlag = true;

    return true;
}

// Miscellaneous Operations

// --- PreSetTest -------------------------------------------------------------
bool XdpParseNumber::PreSetTest(XdpParse *theParser)
{


    ItsHasTestValue = false; // ItsTestValue is not valid yet

    // Get Token

    RWCString token;
    bool OK;

    OK = theParser->GetToken(token);

    // Check if we don't have token. If not and the parameter is optinal,OK, else error

    if (OK == false)
    {
        // No token , if optional OK, else error
        if (IsOptional())
        {
            // Optional, all OK
            return true;
        } else
        {
            // Not optional, failure;
            ItsErrorString = "Non-optional parameter is missing";
            return false;
        }
    }

    // Try to convert to number
    UINT32 value;

    OK = FromString(token, value);

    if (OK == false)
    {
        // Failure
        ItsErrorString = "Failed to convert \"" + token + "\" to number";
        return false;
    }

    // Check limits

    OK = CheckLimits(value);

    if (OK == false)
    {
        // Failure;
        ItsErrorString = "The value " + ToString(value) +
                         " is outside of the valid range: " +
                         ToString(ItsMinValue) + " to " +
                         ToString(ItsMaxValue) + ".";
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

// --- GetNumber --------------------------------------------------------------

// Returns the number value in UINT32 with sign extention

UINT32 XdpParseNumber::GetNumber()
{

    UINT32 v;

    switch (ItsSizeInBits)
    {

    case 8:
        v = *(UINT8 *)ItsNumberP;
        break;

    case 16:
        v = *(UINT16 *)ItsNumberP;
        break;

    case 32:
        v = *(UINT32 *)ItsNumberP;
        break;

    default:
        v = 0;
        RN_ASSERT(0,"Invalid size");
        break;
    }


    // Is signed
    if (ItsSignedFlag)
    {
        // Signed, do Sign extension
        v = (UINT32) SignExtension(v);
    }

    return v;
}

// Set methods


// Miscellaneous Operations


// --- SignExtension ----------------------------------------------------------

// Returns the sign extension of the value in INT32

INT32 XdpParseNumber::SignExtension(UINT32 theValue)
{

    INT32 v;

    switch (ItsSizeInBits)
    {

    case 8:
        v = (INT8)(UINT8)theValue;
        break;

    case 16:
        v = (INT16)(UINT16)theValue;
        break;

    case 32:
        v = (INT32)theValue;
        break;

    default:
        v = 0;
        RN_ASSERT(0,"Invalid size");
        break;
    }

    return v;
}


// --- ToString ---------------------------------------------------------------
// Returns theValue as string depending on the HEX flag and the sign flag

RWCString XdpParseNumber::ToString(UINT32 theValue)
{

    char buf[30];

    // Make the buffer empty string
    buf[0] = '\0';

    // Is hex ?

    if (IsHex())
    {
        // Hex

        switch (ItsSizeInBits)
        {
        case 8:
            sprintf(buf,"0x%02X",theValue);
            break;

        case 16:
            sprintf(buf,"0x%04X",theValue);
            break;

        case 32:
            sprintf(buf,"0x%08X",theValue);
            break;

        default:
            RN_ASSERT(0,"Invalid size");
            break;
        } // switch
    } else
    {
        // Decimal. Signed or Unsigned ?

        if (ItsSignedFlag)
        {
            // Signed
            sprintf(buf,"%d",theValue);
        } else
        {
            // Unsigned
            sprintf(buf,"%u",theValue);
        } // if signed

    } // else hex



    return RWCString(buf);

}

// --- FromString ---------------------------------------------------------------
// Convert string to number depending on the format. Return true if OK.

bool XdpParseNumber::FromString(RWCString &theString, UINT32 &theValue) {

    // Remove all "_" from string
    RWCString str;

    // Empty the string
    str = "";

    // Loop all char, if it is not a "_" copy it to str
    UINT32 i;
    char c;

    for( i = 0 ; i < theString.length() ; i++) {

        // Get a char
        c = theString[i];

        // Is a "_" ?
        if (c != '_') {
            // Not a "_", copy it
            str += c;
        }
    }


    theValue=0; //Just in case

    // Build format string.
    // Trick try to get char after the value. If got a char, error becuase it is extra

    char *fmt="";
    bool try4Binary = false;

    // Is hex ?

    if (IsHex())
    {
        // Hex
        fmt="%x%c";
    } else
    {
        // Decimal. Signed or Unsigned ?

        if (ItsSignedFlag)
        {
            // Signed
            fmt="%d%c";
        } else
        {
            // Unsigned
            // some additions
            // If we see first two characters 0x - read hexdecimal number
            // special case is binary number - it is sequence of [01] and finished with letter "b"

            try4Binary = true; // if read failed - we'll try to read binary number as a last chance
            if (str.length() > 2 && str(0) == '0' && toupper(str(1)) == 'X')
            {
                fmt = "%x%c";
            } else {
                fmt="%u%c";
            }
        } // if signed

    } // else hex

    // Parse string

    // Trick try to get char after the value. If got a char, error becuase it is extra
    char dummyChar;

    INT32 rc = sscanf(str.data(), fmt, &theValue, &dummyChar);

    if (rc==1)
    {
        // OK
        return true;
    } else
    {
        // fail
        if (try4Binary == true)
        {
            char number_array[33]; // max 32 bits and last - end of string
            char b_char;
            rc = sscanf(str.data(), "%32[01]%c%c", &number_array, &b_char, &dummyChar);
            if (rc == 2 && toupper(b_char) == 'B')
            {
                theValue = 0;
                for (UINT32 iii = 0; iii < strlen(number_array); iii++)
                {
                    theValue <<= 1;
                    if (number_array[iii] == '1')
                    {
                        theValue |= 1;
                    }
                }
                return true;
            }
        }
        return false;
    }
}


// --- CheckLimits ------------------------------------------------------------
// Return true if theValue is within the limits.

bool XdpParseNumber::CheckLimits(UINT32 theValue)
{

    // Is Signed ?

    if (ItsSignedFlag == true)
    {
        // Signed
        if (( (INT32)theValue >= (INT32)ItsMinValue )  &&
            ( (INT32)theValue <= (INT32)ItsMaxValue ))
        {
            // OK
            return true;
        } else
        {
            // Failed
            return false;
        }
    } else
    {
        // Unsigned
        if (( theValue >= ItsMinValue )  && ( theValue <= ItsMaxValue ))
        {
            // OK
            return true;
        } else
        {
            // Failed
            return false;
        }
    } // unsigned

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

