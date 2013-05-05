/* XdpConv.cc */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpConv

  This class provide convert services:

   1. Number to string converts 
		ToStr    - Number (and bool) to decimal string
		ToHexStr - Number to hex string
		ToBinStr - Number to binary string

   2. Helper functions:
		Pad      - Pad a string with space to the left or to the right
		Repeat   - Create string by repeating a char N times
		Spaces   - Create a string of N spaces

AUTHOR
   Gaash,SB

		
*************************************************************************/

#include <prepc.h>
#include <math.h>  // fabs
#include "Error.h"
#include "XdpConv.h"


// --- Statics ----------------------------------------------------------------

XdpConv::XdpConvUnitInfo XdpConv::TheirUnitInfo[] = {
    
    { "T" , 1e12  },
    { "G" , 1e9   },
    { "M" , 1e6   },
    { "K" , 1e3   },
    { " " , 1e0   },
    { "m" , 1e-3  },
    { "u" , 1e-6  },
    { "n" , 1e-9  },
    { "p" , 1e-12 },

    { "NULL", 0   }  };


// --- ToStr(UINT32) ----------------------------------------------------------
// Convert number (or boolean) to decimal string. 
//   theWidth > 0, left pad to theWidth with space
//   theWidth < 0, right pad to theWidth with space
//   theWidth = 0, don't pad
// If theWidth is ommited, don't pad

RWCString XdpConv::ToStr(UINT32 theVal, INT32 theWidth) 
{

	char buf[30+1];


	// Convert the number to string in the buffer
	sprintf(buf, "%u",theVal);

	// pad the string
	RWCString  s = Pad( RWCString(buf), theWidth);

	return s;

}

// --- ToStr(UINT16) ----------------------------------------------------------
// See remarks at ToStr(UINT32)

RWCString XdpConv::ToStr(UINT16 theVal, INT32 theWidth) {

	   // cast to UINT32 and convert to string
	return ToStr( (UINT32) theVal, theWidth);

}


// --- ToStr(UINT8) -----------------------------------------------------------
// See remarks at ToStr(UINT32)

RWCString XdpConv::ToStr(UINT8 theVal, INT32 theWidth) {

	   // cast to UINT32 and convert to string
	return ToStr( (UINT32) theVal, theWidth);

}


// --- ToStr(INT32) -----------------------------------------------------------
// See remarks at ToStr(UINT32)

RWCString XdpConv::ToStr(INT32 theVal, INT32 theWidth) 
{

	char buf[30+1];


	// Convert the number to string in the buffer
	sprintf(buf, "%d",theVal);

	// pad the string
	RWCString  s = Pad( RWCString(buf), theWidth);

	return s;

}

// --- ToStr(INT16) -----------------------------------------------------------
// See remarks at ToStr(UINT32)

RWCString XdpConv::ToStr(INT16 theVal, INT32 theWidth) {

	   // cast to INT32 and convert to string
	return ToStr( (INT32) theVal, theWidth);

}


// --- ToStr(INT8) ------------------------------------------------------------
// See remarks at ToStr(UINT32)

RWCString XdpConv::ToStr(INT8 theVal, INT32 theWidth) {

	   // cast to INT32 and convert to string
	return ToStr( (INT32) theVal, theWidth);

}

// --- ToStr(bool) ------------------------------------------------------------
// See remarks at ToStr(UINT32)

RWCString XdpConv::ToStr(bool theVal, INT32 theWidth) 
{

	// Convert the boolean value to string

    RWCString valueAsStr;

	if (theVal) {
		// true
	    valueAsStr = "true";
	}
	else {
		// false
	    valueAsStr = "false";
	}

	// pad the string
	RWCString  s = Pad( valueAsStr, theWidth);

	return s;

}

#ifdef RN_64BIT
// --- ToStr(UINT64) -----------------------------------------------------------
//
// See remarks at ToStr(UINT32)
//
RWCString XdpConv::ToStr(UINT64 theVal, INT32 theWidth) {


    char buf[30];
    
    // Digit pointer, begin at end of buffer since we are going backword least to most
    char *p = buf + sizeof(buf) - 1;

    // Set the terminating NULL & update pointer
    *p = '\0';

    // Convert the number to char, going from least to most with weight, backward with the digits
    UINT32 digitNum;
    UINT64 left = theVal;

    for (digitNum = 0; digitNum < sizeof(buf)-3 ; digitNum++) {
        
        // Take back the pointer, making room for the digit
        p--;

        // Convert the digit to char
        *p = (char) (left % 10 + '0');

        // Remove the digit form the number
        left /= 10;

        // Is nothing left ?
        if (left == 0) {
            // Nothing left
            break;
        }

    }

    // p is the pointer to the number in the buffer, pad it

    RWCString s = XdpConv::Pad( p , theWidth);

    // done

	return s;

}

// --- ToStr(INT64) -----------------------------------------------------------
//
// See remarks at ToStr(UINT32)
//
RWCString XdpConv::ToStr(INT64 theVal, INT32 theWidth) {


    RWCString s;

    // Check sign
    if (theVal >= 0) {
        // Positive 
        s = XdpConv::ToStr ( (UINT64) theVal, theWidth);
    }
    else {
        // Negative
        s = "-" + XdpConv::ToStr ( (UINT64) -theVal, 0 );
        s = XdpConv::Pad(s,theWidth);
    }

    // All done
	return s;
}

#endif // RN_64BIT

// --- ToHexStr(UINT32) -------------------------------------------------------
// Convert number to hex string. 
//   theWidth > 0, left pad to theWidth with space
//   theWidth = 0, don't pad
// If theWidth is ommited, don't pad

RWCString XdpConv::ToHexStr(UINT32 theVal, UINT32 theWidth) 
{

	char buf[30+1];


	// Convert the number to string in the buffer
	sprintf(buf, "%X",theVal);

	// pad the string
	RWCString  s = Pad( RWCString(buf), theWidth , '0' );

	return s;

}

// --- ToHexStr(UINT16) -------------------------------------------------------
// See remarks at ToHexStr(UINT32)

RWCString XdpConv::ToHexStr(UINT16 theVal, UINT32 theWidth) {

	   // cast to UINT32 and convert to string
	return ToHexStr( (UINT32) theVal, theWidth);

}


// --- ToHexStr(UINT8) --------------------------------------------------------
// See remarks at ToHexStr(UINT32)

RWCString XdpConv::ToHexStr(UINT8 theVal, UINT32 theWidth) {

	   // cast to UINT32 and convert to string
	return ToHexStr( (UINT32) theVal, theWidth);

}


// --- ToHexStr(void *) -------------------------------------------------------
// See remarks at ToHexStr(UINT32)

RWCString XdpConv::ToHexStr(const void *theVal, UINT32 theWidth) {

	   // cast to UINT32 and convert to string
	return ToHexStr( (UINT32) theVal, theWidth);

}

#ifdef RN_64BIT

// --- ToHexStr(UINT64) -------------------------------------------------------
// Convert number to hex string. 
//   theWidth > 0, left pad to theWidth with space
//   theWidth = 0, don't pad
// If theWidth is ommited, don't pad

RWCString XdpConv::ToHexStr(UINT64 theVal, UINT32 theWidth) 
{

	char buf[30+1];


	// Split the 64 bit into two 32 bit

    UINT32 low    = (UINT32) (theVal & 0xffffffff) ; 

	// We want to do: high = theval >> 32, but this generate an assembler
    // warning under GNU. The work around is to do is twice in 16 bits
    UINT64 high16 = theVal >> 16;
    UINT32 high   = (UINT32) (high16 >> 16);

    // Do we have the high part
    if (high == 0) {
        // No high, use the low part
	    sprintf(buf, "%X",low);
    }
    else {
        // High exists, concat low and high
	    sprintf(buf, "%X%X",high,low);
    }

	// pad the string
	RWCString  s = Pad( RWCString(buf), theWidth , '0' );

	return s;

}

#endif // RN_64BIT

// --- ToBinStr(UINT32) -------------------------------------------------------
// Convert number to binary string. 
//   theWidth > 0, left pad to theWidth with space
//   theWidth = 0, don't pad
// If theWidth is ommited, don't pad

RWCString XdpConv::ToBinStr(UINT32 theVal, UINT32 theWidth) 
{

	char buf[32+1]; // 32 bit + NULL char

	// Convert the number to a binary string in the buffer

	// Find the highest bit which is not zero

	INT32 i;
	for (i = 31; i >= 0 ; i--) {
		// Is bit number "i" is non zero break;
		if (  ( theVal & (1<<i) ) != 0) {
			// Non zero, break
			break;
		}
	}

	// If not breaked, (all bits are zero) then
	//  set i to 1 because we need at least one digit

	if (i<0) {
		// Fast bit 0 is non-zero, forcing one digit
		i = 0;
	}

	// Convert the value to binary string, taking only non-zero leading bits
	char *p=buf;

	// i has the holds the bit number to begin with

	for ( ; i >= 0 ; i-- ) {
		
		// convert the bit to digit
		if ( ( theVal & (1<<i) ) != 0) {
			// bit is set
			*p = '1';
		}
		else {
			// bit is clear
			*p = '0';
		}

		// Adavance buffer pointer
		p++;
	}

	// Add '\0'
	*p = '\0';
		
	
	// pad the string
	RWCString  s = Pad( RWCString(buf), theWidth , '0');

	return s;

}

// --- ToBinStr(UINT16) -------------------------------------------------------
// See remarks at ToBinStr(UINT32)

RWCString XdpConv::ToBinStr(UINT16 theVal, UINT32 theWidth) {

	   // cast to UINT32 and convert to string
	return ToBinStr( (UINT32) theVal, theWidth);

}


// --- ToBinStr(UINT8) --------------------------------------------------------
// See remarks at ToBinStr(UINT32)

RWCString XdpConv::ToBinStr(UINT8 theVal, UINT32 theWidth) {

	   // cast to UINT32 and convert to string
	return ToBinStr( (UINT32) theVal, theWidth);

}

// --- Repeat -----------------------------------------------------------------
// Returns a string with the theChar repeated theCount times
RWCString XdpConv::Repeat(char theChar, UINT32 theCount) {

	RWCString s;

	// Append to an empty string
	s.append(theChar, (unsigned int) theCount);

	return (s);
}

// --- Spaces -----------------------------------------------------------------
// Returns a string with theCount spaces
RWCString XdpConv::Spaces(UINT32 theCount) {

	RWCString s;

	// Append to an empty string
	s.append(' ', (unsigned int) theCount);

	return (s);
}

// --- Pad --------------------------------------------------------------------
// Returns a string with theString padded with thePadChar to theWidth
//
//   thePadChar default is space
//
//   theWidth > 0, left pad to theWidth with space
//   theWidth < 0, right pad to theWidth with space
//   theWidth = 0, don't pad
 
RWCString XdpConv::Pad(const RWCString& theString, INT32 theWidth, char thePadChar) {

	// Check no padding, in other words, nothing to do

	if (theWidth == 0) {
		return (theString);
	}

	// We need to pad, get original string length and copy the orginal string

	INT32 len = theString.length();

	RWCString s = theString;
	
	// Check left or right pad {

	if (theWidth > 0) {
		// left pad

		// Is it requied ?
		if (theWidth > len) {
			// Need to left pad
			s.prepend(thePadChar, (unsigned int)(theWidth-len));
		}

	}
	else {
		// right pad, change the sign of theWidth, make it positive

		theWidth = -theWidth;
		
		// Is it requied ?
		if (theWidth > len) {
			// Need to right pad
			s.append(thePadChar, (unsigned int)(theWidth-len));
		}

	}


	// All done

	return (s);

}

// --- ToBaseFilename ---------------------------------------------------------
//
// Returns a string of the base filename of a giving file name
//
//   theWidth > 0, left pad to theWidth with space
//   theWidth < 0, right pad to theWidth with space
//   theWidth = 0, don't pad

RWCString XdpConv::ToBaseFilename(const char *theFullFilename, INT32 theWidth) {

    const char *p;

    // Check NULL pointer
    if (theFullFilename == NULL) {
        // NULL, return a NULL
        p = "!NULL!";
    }
    else {
        // Not NULL, lets do it

        // Goto the end of string
        p = theFullFilename + strlen(theFullFilename);

        // Go back till we hit "\" or "/", but keep in string

        while( p >= theFullFilename) {

            // Check a "\" or "/"
            if ( ((*p) == '\\') || ((*p) == '/') ){
                // Hit, notem p is on the slash
                break;
            }

            // Prev char
            p--;

        } // while


        // Go forward one char because we either on the slash on one char before the string
        p++;


    } // else
  
    // Now p is pointer to the final string - Pad it;

    RWCString s = Pad(p,theWidth);

    // Done;

    return s;

}

// --- ToUnitsStr -------------------------------------------------------------
//
// Returns a string of a double, formated to xxx.xxxU, where U is the proper unit
//
// If theUnitSelector is non zero, it is used to select the unit. Useful when we
// have a some data and it should be displayed with the same unit.
//

RWCString XdpConv::ToUnitsStr(double theValue, INT32 theWidth, double theUnitSelector) {

    char buf[50];

    // Sperate value and sign

    double value;
    RWCString signString;

    if (theValue < 0) {
        // Negative
        value      = - theValue; 
        signString = "-";
    }
    else {
        // Positive (or zero)
        value      = theValue;
        signString = "";
    }

    // Now value is the positive part of the value.

    // Choose the unit seletor
    double unitSelector;
    
    // Did we get a unit selector

    if (theUnitSelector != 0) {
        // Got a unit selector, use it
        unitSelector = fabs(theUnitSelector)*1000;
    }
    else {
        // No unit selector, use the value;
        unitSelector = value;
    }

    // Select unit
    
    XdpConv::XdpConvUnitInfo *unitP = TheirUnitInfo;
    double    number = 0;  // value left after the unit is accounted
    while( unitP->name != NULL ) {

        // Value is inside the unit range ?
        
        if ( (unitSelector >= unitP->value) && (unitSelector <= 1000.5 * unitP->value) ) {
            // match, get the number (value without the unit)
            number = value / unitP->value;
            break;
        }

        // next unit;
        unitP++;
    }

    // Check if we have a unit
    if (number == 0) {
        // no unit, either outside of range or zero, use the simplets format
        sprintf(buf,"%g ",theValue);
    }
    else {
        // we have unit, built string in buf
        sprintf(buf,"%s%.3f%s",signString.data(),number,unitP->name);
    }

    // Now we have the string is buf, pad it and return

    RWCString s = Pad(buf,theWidth);

    // All done
    return s;
    // round the number to 3 digits

}


// --- DoubleFromUnitsStr -----------------------------------------------------
//
// Converts a string to a double. The string might have a unit suffix.
//
// Returns true if conversion is OK.
//

bool XdpConv::DoubleFromUnitsStr(const char *theStrP, double *theResultP) {

    RN_ASSERT(theStrP    != NULL , "XdpConv::DoubleFromUnitsStr theStrP is NULL");
    RN_ASSERT(theResultP != NULL , "XdpConv::DoubleFromUnitsStr theResultP is NULL");

    // convert the string into number and optinal units 
    
    double number;
    char unitChar;
    char dummyChar;
        
    INT32 numberOfValues = sscanf(theStrP, "%lf%c%c", &number, &unitChar, &dummyChar);

    switch(numberOfValues) {

        case 1:  // OK - Number without units

            *theResultP = number;
            return true;

        case 2: { // Number with units

            // Scearch the units
            XdpConv::XdpConvUnitInfo *unitP = TheirUnitInfo;

            while( unitP->name != NULL ) {
            
                // Is unit name match
                if (unitP->name[0] == unitChar) {
                    // Match, adjust number with unit value, all done OK
                    *theResultP = number * unitP->value;
                    return true;
                }

                // next unit;
                unitP++;

            } // while

            // Unit is invalid
            return false;
        } // case

        default: // failed

            return false;
    }

    // Just in case
    return false;
}

// EOF
