/* XdpTraceConfig.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
   Class: XdpTraceConfig
   Storage element that holds the configuration for an XdpTrace object

AUTHOR
   Amit Rapaport, Seabridge

		
*************************************************************************/

#ifndef XdpTraceConfig_h
#define XdpTraceConfig_h

#include "basictypes.h"
#include "XdpConfig.h"
#include "XdpTrace.h"

#include "XdpConfigService.h"
//#include "propriety inc"

//----------------------------
// Forward Declarations
//----------------------------
class XdpTraceConfig :public XdpConfig

{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------

// Constructor & Destructor
/*
XdpTraceConfig(const CardIndex & theCardIndexR,
               XdpConfigService *theConfigServicesP);
*/
XdpTraceConfig(UINT32 theCardIndex,
               const RWCString &theTraceNameR,
               XdpConfigService* theConfigServicesP);


virtual ~XdpTraceConfig();
//Get methods
RWCString GetName();

// Miscellaneous Operations
void UpdateConfig(const UINT32 enableAppTags,
                  const UINT32 *enableColorsBufferP,
                  const UINT32 printActiveAppTags,
                  const UINT32 *printActiveColorsBufferP,
                  const UINT32 printOutputChannel);

//saves the configuration from the XdpTraceObject into the file
//virtual void SaveConfig();
void SaveConfig(FILE *theFileP);
//virtual void DownloadConfig(const CardIndex & theCardIndexR);
virtual void DownloadConfig();
void PrintTraceConfig();

//-------------------------------------------------------------------------
protected:
//-------------------------------------------------------------------------
// Miscellaneous Operations


// Data members
RWCString ItsName;
UINT32 ItsEnableAppTags;
UINT32 ItsEnableColorsBuffP[XDP_TRACE_APP_LAST];
UINT32 ItsPrintActiveAppTags;
UINT32 ItsPrintActiveColorsBuffP[XDP_TRACE_APP_LAST];
UINT32 ItsPrintOutputChannel;


//-------------------------------------------------------------------------
private:
//-------------------------------------------------------------------------
// Copy ctor
XdpTraceConfig(const XdpTraceConfig &theTraceConfig);

// Operator =
XdpTraceConfig & operator=( const XdpTraceConfig theTraceConfig);

};

#endif // XdpTraceConfig_h

