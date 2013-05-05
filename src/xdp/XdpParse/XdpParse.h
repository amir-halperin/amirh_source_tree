/* XdpParse.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpParse
   This base is used to parse easily command line strings. Usually it is 
   used to easily interpret command manager command arguments.

AUTHOR
   Gaash, SB

		
*************************************************************************/

#ifndef XdpParse_h
#define XdpParse_h

#include "basictypes.h"
#include "RWHdr/cstring.h"

//----------------------------
// Forward Declarations
//----------------------------

class XdpParseParameter;
class CmdMngr;

class XdpParse 
{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------

// Typedef & Enum

    struct XdpParseSubCommand {   
        const char *name;
        const char *desc;
        UINT32     id;
        UINT32     groupMask;
    } ;  // Used in ParseSubCommands

// Constructor & Destructor

    XdpParse(CmdMngr *theCmdMngr); // The command manager is used of user IO

// Copy constructor & Assign op

// Get methods

    bool   IsOK()           { return (ItsOK) ; };
    UINT32 GetLastAssigned() { return (ItsLastAssigned); };
    bool   GetToken(RWCString &theToken); 
    bool   GetRestOfLine(RWCString &theLine); 
    
// Set methods

// Miscellaneous Operations

    bool ParseParameters(
             const RWCString &theCommandName, 
             const RWCString &theDescription,  
             UINT32   theFlags, 
             // The paramertes, upto 16, NULL if not used  
             XdpParseParameter *theP1  = NULL, XdpParseParameter *theP2  = NULL,
             XdpParseParameter *theP3  = NULL, XdpParseParameter *theP4  = NULL,
             XdpParseParameter *theP5  = NULL, XdpParseParameter *theP6  = NULL,
             XdpParseParameter *theP7  = NULL, XdpParseParameter *theP8  = NULL,
             XdpParseParameter *theP9  = NULL, XdpParseParameter *theP10 = NULL,
             XdpParseParameter *theP11 = NULL, XdpParseParameter *theP12 = NULL,
             XdpParseParameter *theP13 = NULL, XdpParseParameter *theP14 = NULL,
             XdpParseParameter *theP15 = NULL, XdpParseParameter *theP16 = NULL);

    bool ParseSubCommands(
             const RWCString &theCommandName,
             const RWCString &theDescription,
             UINT32           theFlags,
             const XdpParseSubCommand *theSubCommandsP,
             UINT32           theActiveGroups,
             UINT32          &theSelectedId);


// Operators

// Conv Op

// RTTI


//-------------------------------------------------------------------------
protected:
//-------------------------------------------------------------------------

// Typedef & Enum

    enum { XDP_PARSE_MAX_PARAMS = 16 }; // must match the ctor
// Constructor & Destructor

// Copy constructor & Assign op

// Get methods

// Set methods

// Miscellaneous Operations

    // Parse paramters functions
    virtual bool ParamsProcessLine();
    virtual void ParamsPrintUsage();
    virtual void ParamsPrintDebug();

    // Parse sub commands functions
    virtual bool SubCmdsProcessLine(
             const XdpParseSubCommand *theSubCommandsP,
             UINT32                    theActiveGroups,
             UINT32                   &theSelectedId);

    virtual void SubCmdsPrintUsage(
             const XdpParseSubCommand *theSubCommandsP,
             UINT32                    theActiveGroups);
    

    virtual void SubCmdsPrintDebug(
             const XdpParseSubCommand *theSubCommandsP,
             UINT32                   theActiveGroups,
             UINT32                   theSelectedId);

    virtual INT32 SubCmdFindCommandIndex(          
             const XdpParseSubCommand *theSubCommandsP,
             RWCString                &theCmdToken);

    // Common functions
    virtual bool ProcessInternalFlags();
    void SetTokenString(RWCString &theString);
    void PrintString(RWCString theString);


  
// Operators

// Conv Op

// Data members


    RWCString   ItsCommandName;
    RWCString   ItsDescription;
    CmdMngr     *ItsCmdMngrP;
    UINT32      ItsFlags;
    UINT32      ItsNumberOfParams;
    RWCString   ItsTokenString;
    RWCString   ItsOriginalTokenString;
    char        *ItsTokenP;
    bool        ItsFirstTokenFlag;

    XdpParseParameter *ItsParam[XDP_PARSE_MAX_PARAMS];

    char        ItsLineBuffer[200];

    bool        ItsOK;
    UINT32      ItsLastAssigned;

    bool        ItsDebugFlag;
    bool        ItsHelpFlag;
    bool        ItsPrintUsageFlag;

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

#endif // XdpParse_h

