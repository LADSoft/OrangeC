#ifndef _NETMON_H
#define _NETMON_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Microsoft Network Monitor definitions */

#include "npptypes.h"
#include "nmsupp.h"
#include "bhtypes.h"
#include "nmerr.h"
#include "bhfilter.h"
#include "parser.h"
#include "inilib.h"
#include "nmevent.h"
#include "nmexpert.h"

#include <pshpack1.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INVALID_FRAME_NUMBER  ((DWORD)-1)

#define CAPTUREFILE_OPEN  OPEN_EXISTING
#define CAPTUREFILE_CREATE  CREATE_NEW

extern LPSYSTEMTIME WINAPI GetCaptureTimeStamp(HCAPTURE);
extern DWORD WINAPI GetCaptureMacType(HCAPTURE);
extern DWORD WINAPI GetCaptureTotalFrames(HCAPTURE);
extern LPSTR WINAPI GetCaptureComment(HCAPTURE);
extern DWORD WINAPI MacTypeToAddressType(DWORD);
extern DWORD WINAPI AddressTypeToMacType(DWORD);
extern DWORD WINAPI GetFrameDstAddressOffset(HFRAME,DWORD,LPDWORD);
extern DWORD WINAPI GetFrameSrcAddressOffset(HFRAME,DWORD,LPDWORD);
extern HCAPTURE WINAPI GetFrameCaptureHandle(HFRAME);
extern DWORD WINAPI GetFrameDestAddress(HFRAME,LPADDRESS,DWORD,DWORD);
extern DWORD WINAPI GetFrameSourceAddress(HFRAME,LPADDRESS,DWORD,DWORD);
extern DWORD WINAPI GetFrameMacHeaderLength(HFRAME);
extern BOOL WINAPI CompareFrameDestAddress(HFRAME,LPADDRESS);
extern BOOL WINAPI CompareFrameSourceAddress(HFRAME,LPADDRESS);
extern DWORD WINAPI GetFrameLength(HFRAME);
extern DWORD WINAPI GetFrameStoredLength(HFRAME);
extern DWORD WINAPI GetFrameMacType(HFRAME);
extern DWORD WINAPI GetFrameMacHeaderLength(HFRAME);
extern DWORD WINAPI GetFrameNumber(HFRAME);
extern __int64 WINAPI GetFrameTimeStamp(HFRAME);
extern ULPFRAME WINAPI GetFrameFromFrameHandle(HFRAME);
extern HFRAME WINAPI ModifyFrame(HCAPTURE,DWORD,LPBYTE,DWORD,__int64);
extern HFRAME WINAPI FindNextFrame(HFRAME,LPSTR,LPADDRESS,LPADDRESS,LPWORD,DWORD,DWORD);
extern HFRAME WINAPI FindPreviousFrame(HFRAME,LPSTR,LPADDRESS,LPADDRESS,LPWORD,DWORD,DWORD);
extern HCAPTURE WINAPI GetFrameCaptureHandle(HFRAME);
extern HFRAME WINAPI GetFrame(HCAPTURE,DWORD);
extern LPRECOGNIZEDATATABLE WINAPI GetFrameRecognizeData(HFRAME);
extern HPROTOCOL WINAPI CreateProtocol(LPSTR,LPENTRYPOINTS,DWORD);
extern VOID WINAPI DestroyProtocol(HPROTOCOL);
extern LPPROTOCOLINFO WINAPI GetProtocolInfo(HPROTOCOL);
extern HPROPERTY WINAPI GetProperty(HPROTOCOL,LPSTR);
extern HPROTOCOL WINAPI GetProtocolFromName(LPSTR);
extern DWORD WINAPI GetProtocolStartOffset(HFRAME,LPSTR);
extern DWORD WINAPI GetProtocolStartOffsetHandle(HFRAME,HPROTOCOL);
extern DWORD WINAPI GetPreviousProtocolOffsetByName(HFRAME,DWORD,LPSTR,DWORD*);
extern LPPROTOCOLTABLE WINAPI GetEnabledProtocols(HCAPTURE);
extern DWORD WINAPI CreatePropertyDatabase(HPROTOCOL,DWORD);
extern DWORD WINAPI DestroyPropertyDatabase(HPROTOCOL);
extern HPROPERTY WINAPI AddProperty(HPROTOCOL,LPPROPERTYINFO);
extern BOOL WINAPI AttachPropertyInstance(HFRAME,HPROPERTY,DWORD,ULPVOID,DWORD,DWORD,DWORD);
extern BOOL WINAPI AttachPropertyInstanceEx(HFRAME,HPROPERTY,DWORD,ULPVOID,DWORD,ULPVOID,DWORD,DWORD,DWORD);
extern LPPROPERTYINST WINAPI FindPropertyInstance(HFRAME,HPROPERTY);
extern LPPROPERTYINST WINAPI FindPropertyInstanceRestart(HFRAME,HPROPERTY,LPPROPERTYINST*,BOOL);
extern LPPROPERTYINFO WINAPI GetPropertyInfo(HPROPERTY);
extern LPSTR WINAPI GetPropertyText(HFRAME,LPPROPERTYINST,LPSTR,DWORD);
extern DWORD WINAPI GetCaptureCommentFromFilename(LPSTR,LPSTR,DWORD);
extern int WINAPI CompareAddresses(LPADDRESS,LPADDRESS);
extern DWORD WINAPIV FormatPropertyInstance(LPPROPERTYINST,...);
extern SYSTEMTIME *WINAPI AdjustSystemTime(SYSTEMTIME*,__int64);
DWORD WINAPI ExpertGetFrame(HEXPERTKEY,DWORD,DWORD,DWORD,HFILTER,LPEXPERTFRAMEDESCRIPTOR);
LPVOID WINAPI ExpertAllocMemory(HEXPERTKEY,SIZE_T,DWORD*);
LPVOID WINAPI ExpertReallocMemory(HEXPERTKEY,LPVOID,SIZE_T,DWORD*);
DWORD WINAPI ExpertFreeMemory(HEXPERTKEY,LPVOID);
SIZE_T WINAPI ExpertMemorySize(HEXPERTKEY,LPVOID);
DWORD WINAPI ExpertIndicateStatus(HEXPERTKEY,EXPERTSTATUSENUMERATION,DWORD,const char*,LONG);
DWORD WINAPI ExpertSubmitEvent(HEXPERTKEY,PNMEVENTDATA);
DWORD WINAPI ExpertGetStartupInfo(HEXPERTKEY,PEXPERTSTARTUPINFO);
extern DWORD WINAPI BHGetLastError(VOID);

#ifdef DEBUG
#undef DebugBreak
#define DebugBreak()  dprintf("DebugBreak Called at %s:%s", __FILE__, __LINE__);
#define BreakPoint()  DebugBreak()
#endif /* DEBUG */

#ifdef __cplusplus
}
#endif

#include <poppack.h>

#endif /* _NETMON_H */
