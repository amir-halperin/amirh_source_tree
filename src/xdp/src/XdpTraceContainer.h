/* XdpTraceContainer.h   */

/*************************************************************************
*
*    (C) COPYRIGHT 2000/2001  SIEMENS AG.   All rights reserved.
*
*************************************************************************/

/*************************************************************************

DESCRIPTION
    Class: XdpTraceContainer
    Sorted List of pointers to XdpTrace objects,
    and standard list operations

AUTHOR
   Amit Rapaport, Seabridge

*************************************************************************/

#ifndef XdpTraceContainer_h
#define XdpTraceContainer_h

#include "basictypes.h"
#include "RWHdr/cstring.h"
#include "SLinkList.h"

#include "XdpTrace.h"

class XdpTraceContainer
{

//-------------------------------------------------------------------------
public:
//-------------------------------------------------------------------------
// Constructor

XdpTraceContainer();

static XdpTraceContainer* GetIt();


// Get methods
XdpTrace* GetTrace(const RWCString & theNameR);
XdpTrace* GetFirst();


// Miscellaneous Operations

void Insert(XdpTrace *theTraceP);

bool Remove(XdpTrace *theTraceP);

void printTraceList();

//-------------------------------------------------------------------------
protected:
//-------------------------------------------------------------------------

// Set methods

void SetHead(XdpTrace *theTraceP);

// Data members

static XdpTraceContainer *TheirTraceContainerP;

//-------------------------------------------------------------------------
private:
//-------------------------------------------------------------------------
//Destructor
virtual ~XdpTraceContainer();

// Copy ctor
XdpTraceContainer(const XdpTraceContainer &theTraceContainer);

// Operator =
XdpTraceContainer & operator=( const XdpTraceContainer theTraceCOntainer) ;

// Data members
SLinkList ItsTraceList;

// This declaration will supress compilation warnings
    friend void Dummy();

};
#endif // XdpTraceContainer_h

