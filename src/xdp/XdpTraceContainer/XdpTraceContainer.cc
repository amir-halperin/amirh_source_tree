/* XdpTraceContainer.cc  */

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

**************************************************************************/
#include "XdpTraceContainer.h"

#include "stdio.h"
#include "Error.h"
#include "casts.h"
#include <stdio.h>

extern const MaskFlagId MASK_XDP;

XdpTraceContainer *XdpTraceContainer::TheirTraceContainerP = 0;
//-------------------------------------------------------------------------
// public
//-------------------------------------------------------------------------

// Constructor & Destructor
XdpTraceContainer::XdpTraceContainer()
{
    RN_TRACE(MASK_XDP,"::XdpTraceContainer - ctor begin");

    // Make sure the object is created exactly once
    if (NULL != TheirTraceContainerP)
    {
        REC_FATAL(17539);
    }
    // Save the object pointer
    TheirTraceContainerP = this;
}

/**************************************************************************
*
* GetIt
*
* RETURNS
*    returns a pointer to the XdpTraceContainer singelton
*
**************************************************************************/
XdpTraceContainer *XdpTraceContainer::GetIt()
{
    RN_TRACE(MASK_XDP,"XdpTraceContainer::GetIt");

    // Check object already exists
    if (NULL == TheirTraceContainerP)
    {
        // Not exists, error
        REC_FATAL(17539);
    }
    // Object exists, return it
    return TheirTraceContainerP;
}

// Get methods
/**************************************************************************
*
* GetTrace
*
* DESCRIPTION
*    Searching the list for XdpTrace with name theNameR
*
* RETURNS
*    returns a pointer to the XdpTrace with name theNameR,
*    or null if such does not exist
**************************************************************************/
XdpTrace *XdpTraceContainer::GetTrace(const RWCString &theNameR)
{
    RN_TRACE(MASK_XDP,"XdpTraceContainer::GetTrace" + theNameR);
    RN_ASSERT ((NULL != theNameR), "XdpTrace Name reference is NULL");

    SLinkList::SLinkListIter iter;
    iter = ItsTraceList.Begin();
    XdpTrace *traceP = rn_static_cast(XdpTrace*, iter.Get());

    while(NULL != traceP)
    {
        RWCString tracePName = traceP->GetYourId();
		if (0 == theNameR.compareTo(tracePName))
        {
			break;
		}
		
		//get next
        ++iter;
        traceP = rn_static_cast(XdpTrace *, iter.Get());
    }
    return traceP;
}

/**************************************************************************
*
* GetFirst
*
* RETURNS
*    returns a pointer to the first trace in the list
*       or NULL if the list is empty
*
**************************************************************************/
XdpTrace* XdpTraceContainer::GetFirst()
{
    RN_TRACE(MASK_XDP,"XdpTraceContainer::GetFirst");
    XdpTrace *traceP = NULL;
    if (! ItsTraceList.IsEmpty())
    {
        SLinkList::SLinkListIter iter;
        iter = ItsTraceList.Begin();
        traceP = rn_static_cast(XdpTrace*, iter.Get());
    }
    return traceP;
}


// Miscellaneous Operations
/**************************************************************************
*
* Insert
*
* DESCRIPTION
*    Insert theTraceP into the list
*
*
**************************************************************************/


void XdpTraceContainer::Insert(XdpTrace *theTraceP)
{
    RN_TRACE(MASK_XDP,"XdpTraceContainer::Insert "+ theTraceP->GetYourId());
    RN_ASSERT ((NULL != theTraceP), "XdpTrace pointer is NULL");

    if (NULL != theTraceP)
    {
        RWCString traceToInsertName = theTraceP->GetYourId();

        if (ItsTraceList.IsEmpty())
        {
            ItsTraceList.InsertFirst(theTraceP);
        }
        else if(NULL != GetTrace(traceToInsertName))
        {
            REC_FATAL(17541);
        }
        else
        {
            SLinkList::SLinkListIter iterCurrent;
            iterCurrent = ItsTraceList.Begin();

            //current trace  & its name
            XdpTrace *currentTraceP = rn_static_cast(XdpTrace*, iterCurrent.Get());
            RWCString currentTraceName = currentTraceP->GetYourId();

            if(traceToInsertName.compareTo(currentTraceName) < 0)
            {
                ItsTraceList.InsertFirst(theTraceP);
            }

            else
            {
                //next trace iterator
                SLinkList::SLinkListIter iterNext;
                iterNext = ItsTraceList.Begin();
                ++iterNext;
                XdpTrace *nextTraceP = rn_static_cast(XdpTrace*, iterNext.Get());

                while (NULL != currentTraceP && NULL != nextTraceP)
                {
                    //curent trace name
                    currentTraceName = currentTraceP->GetYourId();
                    //next trace name
                    RWCString nextTraceName = nextTraceP->GetYourId();
                    if((traceToInsertName.compareTo(currentTraceName) > 0))
                    {
                        if(traceToInsertName.compareTo(nextTraceName)<0)
                        {
                            break;
                        }
                    }

                    //get next
                    ++iterCurrent;
                    currentTraceP = rn_static_cast(XdpTrace*, iterCurrent.Get());

                    ++iterNext;
                    nextTraceP = rn_static_cast(XdpTrace*, iterNext.Get());
                }
                ItsTraceList.Insert(iterCurrent,theTraceP);
            }
        }
        //printTraceList();
    }
}

/**************************************************************************
            printTraceList


****************************************************************************/
void XdpTraceContainer::printTraceList(){

    if (! ItsTraceList.IsEmpty())
    {

        SLinkList::SLinkListIter iter;
        iter = ItsTraceList.Begin();
        XdpTrace *traceP = rn_static_cast(XdpTrace*, iter.Get());
        UINT32 counter = 1;
        printf("*******************************************************\n");
        printf("*****************     Traces     **********************\n");
        printf("*******************************************************\n");
        while(traceP)
        {
            const char *name = (traceP->GetYourId()).operator const char*();
            printf ("Trace number %d :: %s \n", counter,name);

            //get next
            counter++;
            ++iter;
            traceP = rn_static_cast(XdpTrace *, iter.Get());
        }
    }
    else
    {
        printf("XdpTraceContainer :: List is Empty\n");
    }
}

/**************************************************************************
*
* Remove
*
* DESCRIPTION
*    Removes trace theTraceP from list
*
* RETURNS
*    true - if the trace was found (and removed)
*    false - if the trace was not found in the list
**************************************************************************/
bool XdpTraceContainer::Remove(XdpTrace *theTraceP)
{

    RN_ASSERT ((NULL != theTraceP), "XdpTrace pointer is NULL");
    RN_TRACE(MASK_XDP,"XdpTraceContainer::Remove "+ theTraceP->GetYourId());

    RWCString traceToRemoveName = theTraceP->GetYourId();
    bool retValue;

    SLinkList::SLinkListIter iter;
    iter = ItsTraceList.Begin();
    XdpTrace *traceP = rn_static_cast(XdpTrace*, iter.Get());

    while(NULL != traceP)
    {

         RWCString tracePName = traceP->GetYourId();
		if (0 == traceToRemoveName.compareTo(tracePName))
        {
			retValue = true;
            break;
		}

		//get next		
        ++iter;
        traceP = rn_static_cast(XdpTrace *, iter.Get());
    }
    ItsTraceList.Remove(iter);
    return retValue;
}
//Destructor
XdpTraceContainer::~XdpTraceContainer()
{
    delete this;
}



