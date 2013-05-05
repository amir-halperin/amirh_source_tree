/*****************************************************************************/
// Class: XdpMsgTable
// File:  XdpMsgTable.h
// Author:	Alejandro Ohlbaum
//
// Purpose: Class of the error messages table for the XDP SW Project. 
//		
// Descripton: The class holds the static error area messages table and in
//					construntor set the base table pointer to point to his table.
//					The table is staticaly initialized in the *.cc file.
//
// Class Invariant:
//
// Copy Semantics: 
//
// Instantiation semantics:
/*****************************************************************************/

#ifndef XdpMsgTable_h
#define XdpMsgTable_h

#include "basictypes.h"
#include "ErrorMsgTable.h"

//----------------------------
// Forward Declarations
//----------------------------

class XdpMsgTable : public ErrorMsgTable
{

public:

//-----------------------------------------------------------------------------
// Miscellaneous Operations
//-----------------------------------------------------------------------------
  
	XdpMsgTable();

	virtual void SetConcrateClass() {}; // Dummy func. for making the class concrete

//-----------------------------------------------------------------------------
// Typedef & Enum
//-----------------------------------------------------------------------------

	static const ErrorMsgT ItsErrTable[];
};


#endif // XdpMsgTable_h





