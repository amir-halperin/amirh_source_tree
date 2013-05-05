/* XdpConv.h   */

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
		ToHexStr - Number (and pointer) to hex string
		ToBinStr - Number to binary string

   2. Helper functions:
		Pad      - Pad a string with space to the left or to the right
		Repeat   - Create string by repeating a char N times
		Spaces   - Create a string of N spaces

   3. Utility funtions
        ToBaseFilename - Return the basename of a givin filename

   4. Number with units conversion
        ToUnitsStr - Convert a double to a string with units
AUTHOR
   Gaash,SB

		
*************************************************************************/

#ifndef XdpConv_h
#define XdpConv_h

#include "basictypes.h"
#include "RWHdr/cstring.h"

//----------------------------
// Forward Declarations
//----------------------------

class XdpConv
{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------




static RWCString ToStr(UINT32 theVal, INT32 theWidth = 0);
static RWCString ToStr(UINT16 theVal, INT32 theWidth = 0);
static RWCString ToStr(UINT8  theVal, INT32 theWidth = 0);
static RWCString ToStr(INT32  theVal, INT32 theWidth = 0);
static RWCString ToStr(INT16  theVal, INT32 theWidth = 0);
static RWCString ToStr(INT8   theVal, INT32 theWidth = 0);

static RWCString ToStr(bool   theVal, INT32 theWidth = 0);

#ifdef RN_64BIT
static RWCString ToStr(UINT64 theVal, INT32 theWidth = 0);
static RWCString ToStr(INT64  theVal, INT32 theWidth = 0);
#endif

// Convert number (or boolean) to decimal string.
//   theWidth > 0, left pad to theWidth with space
//   theWidth < 0, right pad to theWidth with space
//   theWidth = 0, don't pad
// If theWidth is ommited, don't pad

static RWCString ToHexStr(UINT32 theVal, UINT32 theWidth = 8);
static RWCString ToHexStr(UINT16 theVal, UINT32 theWidth = 4);
static RWCString ToHexStr(UINT8  theVal, UINT32 theWidth = 2);
static RWCString ToHexStr(const void * theVal, UINT32 theWidth = 8);

#ifdef RN_64BIT
static RWCString ToHexStr(UINT64 theVal, UINT32 theWidth = 16);
#endif

// Convert number (and pointer) to hex string.
//   theWidth > 0, left pad to theWidth with zeros
//   theWidth = 0, don't pad
// If theWidth is ommited, pad according to the value width



static RWCString ToBinStr(UINT32 theVal, UINT32 theWidth = 32);
static RWCString ToBinStr(UINT16 theVal, UINT32 theWidth = 16);
static RWCString ToBinStr(UINT8  theVal, UINT32 theWidth = 8);

// Convert number to binary string.
//   theWidth > 0, left pad to theWidth with zeros
//   theWidth = 0, don't pad



static RWCString Repeat(char theChar, UINT32 theCount);

// Returns a string with the theChar repeated theCount times

static RWCString Spaces(UINT32 theCount);

// Returns a string with theCount spaces

static RWCString Pad(const RWCString& theString, INT32 theWidth, char thePadChar=' ');

// Returns a string with theString padded with thePadChar to theWidth
//
//   thePadChar default is space
//
//   theWidth > 0, left pad to theWidth with thePadChar
//   theWidth < 0, right pad to theWidth with thePadChar
//   theWidth = 0, don't pad


static RWCString ToBaseFilename(const char *theFullFilename, INT32 theWidth=0);

// Returns a string of the base filename of a giving file name
//
//   theWidth > 0, left pad to theWidth with space
//   theWidth < 0, right pad to theWidth with space
//   theWidth = 0, don't pad


static RWCString ToUnitsStr(double theValue, INT32 theWidth = 0, double theUnitSelector = 0);

// Returns a string of a double, formated to xxx.xxxU, where U is the proper unit
// with standard padding
//
// If theUnitSelector is non zero, it is used to select the unit. Useful when we
// have a some data and it should be displayed with the same unit.
//

static bool DoubleFromUnitsStr(const char *theStrP, double *theResultP);
//
// Converts a string to a double. The string might have a unit suffix.
//
// Returns true if conversion is OK.
//


// --- XdpConvUnitInfo --------------------------------------------------------

// Structure of unit name to unit value

struct XdpConvUnitInfo  {
    const char *name;
    double      value;
};

//-------------------------------------------------------------------------
protected:
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

    static XdpConvUnitInfo TheirUnitInfo[];


};

#endif // XdpConv_h

