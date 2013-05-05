/* XdpGlobalConfig.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpGlobalConfig
   Storage element which holds the XDP configuration per card (LICs and SFC).
   It holds the global configuration (IP address, port etc.) as data members,
   and a linked list of pointers to XdpTraceConfig objects.
   each one of those holds the configuration for one XdpTrace

AUTHOR
   Amit Rapaport, Seabridge

		
*************************************************************************/

#ifndef XdpGlobalConfig_h
#define XdpGlobalConfig_h

#include "basictypes.h"
#include "XdpConfig.h"

#include "RWHdr/cstring.h"
#include "SLinkList.h"
#include "IpAddress.h"
#include "IpPortNumber.h"


#include "XdpConfigService.h"

#define MAX_CONFIG_LEN 250


//----------------------------
// Forward Declarations
//----------------------------

class XdpTraceConfig;

class XdpGlobalConfig : public XdpConfig
{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------


// Constructor & Destructor
XdpGlobalConfig(UINT32 theCardIndex, XdpConfigService* theConfigServicesP);

virtual ~XdpGlobalConfig(){}

// Copy constructor & Assign op

// Get methods
XdpTraceConfig* GetTraceConfig(const RWCString &theTraceNameR);

char* GetFileName(){ return ItsFileName; }

bool GetFileFlag(){ return ItsConfigFileFlag; }


// Miscellaneous Operations
void SaveConfig();



void RestoreConfig();

void UpdateConfig(IpAddress outputToUdpHostIp,
                  IpPortNumber outputToUdpHostPort,
                  bool outputToUdpEnableFlag,
                  bool outputToSerialEnableFlag);

virtual void DownloadConfig();

XdpTraceConfig* NewTraceConfig(const RWCString&   theTraceNameR);


// Tests methods

void PrintList();

void PrintGlobal();

void DeleteFile();

void PrintFile();

//-------------------------------------------------------------------------
protected:
//-------------------------------------------------------------------------
// Miscellaneous Operations
void AddToTraceConfigList(XdpTraceConfig* theTraceConfigP);

void RestoreGlobalConfigFromString(const char *theLine);

void RestoreTraceConfigFromString(const char *theLine);

void UpdateFileFlag();


//-------------------------------------------------------------------------
private:
//-------------------------------------------------------------------------
// Copy ctor
XdpGlobalConfig(const XdpGlobalConfig &theGlobalConfig);

// Operator =
XdpGlobalConfig & operator=( const XdpGlobalConfig theGlobalConfig);

// Data members
char ItsFileName[20];
bool ItsConfigFileFlag;

IpAddress ItsOutputToUdpHostIp;
IpPortNumber ItsOutputToUdpHostPort;
bool ItsOutputToUdpEnableFlag;
bool ItsOutputToSerialEnableFlag;

SLinkList ItsTraceConfigList;

};

#endif // XdpGlobalConfig_h

