/*****************************************************************************/
// Class: XdpMsgTable
// File:  XdpMsgTable.cc
// Author: Alejandro Ohlbaum
//
// Purpose: 
//		
// Descripton::  XDP messages id limits: 17500 - 17599
//
/*****************************************************************************/

#include "XdpMsgTable.h"


// The msg type is get as parameter on the error call.
// The msg id's must be in the area limits.
#ifdef RN_UNDER_PSOS
const ErrorMsgT XdpMsgTable::ItsErrTable[] = {
#endif
#if  defined(RN_UNDER_VXWORKS) || defined(RN_WIN32)
const ErrorMsgTable::ErrorMsgT XdpMsgTable::ItsErrTable[] = {
#endif
//PatternStart_XDP&ERROR_LOG

    { 17500,"- %c - XdpTraceCmdMngr::Instance TheirP is NULL" },
    { 17501,"- %c - XdpTrace::XdpTrace failure" },
    { 17502,"- %c - XdpTrace::_XdpWriteEntryNullTraceP XDP_WRITE has been called with NULL trace pointer" },
    { 17503,"- %c - XdpServices:: NULL problem" },
    { 17504,"- %c - XdpServices::OutputToUdpStart failure %08X" },
    { 17505,"- %c - XdpServices::OutputToUdpTxTask failure %08X" },
    { 17506,"- %c - XdpMngr:: Set output to UDP parameter error" },
    { 17507,"- %c - XdpServices::OutputToSerialStart failure %08X" },
    { 17508,"- %c - XdpServices::OutputToSerialTxTask failure %08X" },
    { 17509,"- %c - XdpMngr:: Set output to serial parameter error" },
    { 17510,"- %c - XdpServices::TimeStampParameterToResolution the frequency is zero (not inited)" },
    { 17511,"- %c - XdpMngr:: Set time stamp parameter error" },
    { 17512,"- %c - XdpLocalConfigService:: Download global parameters error" },
    { 17513,"- %c - XdpLocalConfigService:: XdpTrace object isn't found" },
    { 17514,"- %c - XdpLocalConfigService:: XdpTrace configuration restore failure" },
    { 17515,"- %c - XdpLocalConfigService::GetIt instance is not created" },
    { 17516,"- %c - XdpLocalConfigService::Create failed to allocate memory for itself"},
    { 17517,"- %c - XdpLocalConfigService::Create was already called"},
    { 17518,"- %c - XdpRemoteConfigService::GetIt instance is not created" },
    { 17519,"- %c - XdpRemoteConfigService::Create failed to allocate memory for itself" },
    { 17520,"- %c - XdpRemoteConfigService::Create was already called" },
    { 17521,"- %c - XdpRemoteConfigProxy::Create failed to allocate memory" },
    { 17522,"- %c - XdpRemoteConfigProxy::Create was already called" },
    { 17523,"- %c - XdpConfigMngr::GetIt - derived class instance is not created" },
    { 17524,"- %c - XdpConfigMngr::ctor - failed to allocate XdpTraceContainer" },
    { 17525,"- %c - XdpInSfcConfigMngr::GetIt - instance isn't created jet" },
    { 17526,"- %c - XdpInSfcConfigMngr::Create - failed to allocate memory" },
    { 17527,"- %c - XdpInSfcConfigMngr specific object is already created" },
    { 17528,"- %c - XdpInSfcConfigMngr failed to create ItsSaveConfigQueue, retVal %08X" },
    { 17529,"- %c - XdpInSfcConfigMngr failed to create ItsSaveConfigTask, retVal %08X" },
    { 17530,"- %c - XdpInSfcConfigMngr failed to start ItsSaveConfigTask, retVal %08X" },
    { 17531,"- %c - XdpInSfcConfigMngr::ctor failed to allocate XdpGlobalConfig %08X" },
    { 17532,"- %c - XdpInSfcConfigMngr failed to receive msg from ItsSaveConfigQueue, %08X" },
    { 17533,"- %c - XdpInSfcConfigMngr - undefined %08X XDP operation received" },
    { 17534,"- %c - XdpInSfcConfigMngr failed to send msg to ItsSaveConfigQueue, %08X" },
    { 17535,"- %c - XdpInSfcConfigMngr failed to get XdpTraceConfig" },
    { 17536,"- %c - XdpInLicConfigMngr::Create failed to allocate memory" },
    { 17537,"- %c - XdpInLicConfigMngr::Create object is already created" },
    { 17538,"- %c - XdpGlobalConfig - failed to open configuration file" },
    { 17539,"- %c - XdpTraceContainer - Trace Container is NULL" },
    { 17540,"- %c - XdpInLicConfigMngr::GetIt - instance isn't created yet" },
    { 17541,"- %c - XdpTraceContainer::Insert - Trace already Exist " },
    { 17542,"- %c - XdpGlobalConfig::NewTraceConfig - failed to create XdpTraceConfig " },
    { 17543,"- %c - XdpGlobalConfig::RestoreTraceConfigFromString - failed to Restore XdpTraceConfig " },
    { 17544,"- %c - XdpGlobalConfig::RestoreGlobalConfigFromString - failed to Restore XdpGlobalConfig " },
    { 17545,"- %c - XdpGlobalConfig::RestoreConfig - configuration file currupted " },



//PatternEnd
//  { 17597,"- %c - last msg ,example for REC_?"},
//  { 17598,"- %c - last msg param = %d ,example for REC_?_1P"},
//  { 17599,"- %c - last msg param1 = %d and param2 = %d ,example for REC_?_2P"},
  { MESSAGE_ID_MAX,"-------- dummy, must be last ------------"}
 };


//-----------------------------------------------------------------------------
// public 
//-----------------------------------------------------------------------------

// Constructor & Destructor

XdpMsgTable::XdpMsgTable() {

	ItsErrTableP = ItsErrTable;
}

