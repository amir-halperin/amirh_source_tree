/* XdpParseEnum.h   */

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

#ifndef XdpParseEnum_h
#define XdpParseEnum_h

#include "basictypes.h"
#include "RWHdr/cstring.h"
#include "XdpParseParameter.h"

//----------------------------
// Forward Declarations
//----------------------------

// Global defintion


class XdpParseEnum : virtual public XdpParseParameter

{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------

// Typedef & Enum

    // String/number pair structure, used in srting/pair ctor and internally
    struct XdpParseEnumPair {
        const char *stringP;
        UINT32      value;
    } ;

    // Max pairs with strings ctor
    enum { XDP_PARSE_ENUM_SIMPLE_MAX_PAIRS = 10 }; 

// Constructor & Destructor


    // Strings
    XdpParseEnum(UINT32 &theEnum,
                   const RWCString &theName, const RWCString &theDescription,
                   UINT32 theFlags, 
                   const char *theString0      , const char *theString1=NULL,
                   const char *theString2=NULL , const char *theString3=NULL,
                   const char *theString4=NULL , const char *theString5=NULL,
                   const char *theString6=NULL , const char *theString7=NULL,
                   const char *theString8=NULL , const char *theString9=NULL);
 
    // String/number pair array
    XdpParseEnum(UINT32 &theEnum,
                   const RWCString &theName, const RWCString &theDescription,
                   UINT32 theFlags, 
                   const XdpParseEnumPair *thePairsP);

// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

// Typedef & Enum

// Constructor & Destructor

// Copy constructor & Assign op

// Get methods

    virtual RWCString GetDescriptionString();
    virtual RWCString GetErrorString();
    virtual RWCString GetDebugString();

// Set methods

// Miscellaneous Operations

    virtual bool      PreSetTest(XdpParse *theParser); // to get tokens
    virtual bool      Set();
 
// Operators

// Conv Op

// RTTI
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
 
    RWCString ToString(UINT32 theValue);

    bool FromString(RWCString &theString, UINT32 &theValue, RWCString &theErrorStr);

// Operators

// Conv Op

// Data members

    UINT32      &ItsEnum;     // The caller var

    RWCString   ItsDescription;
    RWCString   ItsErrorString;

    // Enum pairs data
    UINT32                    ItsNumberOfPairs;  // Init at ctor-s
    const XdpParseEnumPair    *ItsPairs;         // Init at ctor-s

    // For strings ctor, build internal string/number array
    XdpParseEnumPair    ItsSimplePairs[XDP_PARSE_ENUM_SIMPLE_MAX_PAIRS]; 

    // Internal data
    UINT32      ItsTestValue;    // Hold the value between "Test" to "Set"
    bool        ItsHasTestValue; // ItsTestValue is valid, need to set it.


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

#endif // XdpParseEnum_h

