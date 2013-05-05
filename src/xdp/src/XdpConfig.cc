/* XdpConfig.cc  */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************
DESCRIPTION
   Class: XdpConfig
   Base class for configuration storage: XdpTraceConfig & XdpGlobalConfig

AUTHOR
   Amit Rapaport, Seabridge

**************************************************************************/

#include "XdpConfig.h"
//extern const MaskFlagId MASK_XDP;
//-------------------------------------------------------------------------
// public
//-------------------------------------------------------------------------
// Constructor & Destructor

XdpConfig::XdpConfig(UINT32 theCardIndex,
                       XdpConfigService *theConfServiceP)
{

    RN_ASSERT( (NULL != theConfServiceP), "Xdp Config Service pointer is NULL");
    ItsCardIndex = CardIndex(theCardIndex);
    ItsConfServiceP = theConfServiceP;
}

