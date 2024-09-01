#ifndef _RTUTILS_H
#define _RTUTILS_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Router process utility function definitions */

#ifdef __cplusplus
extern "C" {
#endif

#define TRACE_USE_FILE  0x00000001
#define TRACE_USE_CONSOLE  0x00000002
#define TRACE_NO_SYNCH  0x00000004

#define TRACE_NO_STDINFO  0x00000001
#define TRACE_USE_MASK  0x00000002
#define TRACE_USE_MSEC  0x00000004

#define INVALID_TRACEID  0xFFFFFFFF

#define NUM_ALERTABLE_THREADS  2
#define MAX_WORKER_THREADS  10

#define WORK_QUEUE_TIMEOUT  1
#define THREAD_IDLE_TIMEOUT  10

#ifndef MAX_PROTOCOL_NAME_LEN
#define MAX_PROTOCOL_NAME_LEN  40
#endif
#define MAX_PROTOCOL_DLL_LEN  48

#define ROUTING_RESERVED
#define OPT1_1
#define OPT1_2
#define OPT2_1
#define OPT2_2
#define OPT3_1
#define OPT3_2

#define TIMER_INACTIVE  3
#define TIMER_ACTIVE  4

#define WT_EVENT_BINDING  WT_WORK_ITEM
#define PWT_EVENT_BINDING  PWT_WORK_ITEM

#define DEBUGPRINT_FILTER_NONCLIENT_EVENTS  0x2
#define DEBUGPRINT_FILTER_EVENTS  0x4
#define DEBUGPRINT_FILTER_TIMERS  0x8

#define ERROR_WAIT_THREAD_UNAVAILABLE  1
#define ERROR_WT_EVENT_ALREADY_DELETED  2

#define TIMER_HIGH(time)  (((LARGE_INTEGER*)&time)->HighPart)
#define TIMER_LOW(time)  (((LARGE_INTEGER*)&time)->LowPart)

#if DBG
#define RTASSERT(exp)  if (!(exp)) RouterAssert(#exp,__FILE__,__LINE__,NULL)
#define RTASSERTMSG(msg,exp)  if (!(exp)) RouterAssert(#exp,__FILE__,__LINE__,msg)
#else
#define RTASSERT(exp)
#define RTASSERTMSG(msg, exp)
#endif

typedef VOID(APIENTRY *WORKERFUNCTION)(PVOID);

typedef struct _MPR_PROTOCOL_0 {
    DWORD dwProtocolId;
    WCHAR wszProtocol[MAX_PROTOCOL_NAME_LEN + 1];
    WCHAR wszDLLName[MAX_PROTOCOL_DLL_LEN + 1];
} MPR_PROTOCOL_0;

struct _WAIT_THREAD_ENTRY;
struct _WT_EVENT_ENTRY;

typedef struct _WT_TIMER_ENTRY {
    LONGLONG te_Timeout;
    WORKERFUNCTION te_Function;
    PVOID te_Context;
    DWORD te_ContextSz;
    BOOL te_RunInServer;
    DWORD te_Status;
    DWORD te_ServerId;
    struct _WAIT_THREAD_ENTRY *teP_wte;
    LIST_ENTRY te_ServerLinks;
    LIST_ENTRY te_Links;
    BOOL te_Flag;
    DWORD te_TimerId;
} WT_TIMER_ENTRY, *PWT_TIMER_ENTRY;

typedef struct _WT_WORK_ITEM {
    WORKERFUNCTION wi_Function;
    PVOID wi_Context;
    DWORD wi_ContextSz;
    BOOL wi_RunInServer;
    struct _WT_EVENT_ENTRY *wiP_ee;
    LIST_ENTRY wi_ServerLinks;
    LIST_ENTRY wi_Links;
} WT_WORK_ITEM, *PWT_WORK_ITEM;

typedef struct _WT_EVENT_ENTRY {
    HANDLE ee_Event;
    BOOL ee_bManualReset;
    BOOL ee_bInitialState;
    BOOL ee_bDeleteEvent;
    DWORD ee_Status;
    BOOL ee_bHighPriority;
    LIST_ENTRY eeL_wi;
    BOOL ee_bSignalSingle;
    BOOL ee_bOwnerSelf;
    INT ee_ArrayIndex;
    DWORD ee_ServerId;
    struct _WAIT_THREAD_ENTRY *eeP_wte;
    LIST_ENTRY ee_ServerLinks;
    LIST_ENTRY ee_Links;
    DWORD ee_RefCount;
    BOOL ee_bFlag;
    DWORD ee_EventId;
} WT_EVENT_ENTRY, *PWT_EVENT_ENTRY;

DWORD APIENTRY TraceRegisterExA(LPCSTR,DWORD);
DWORD APIENTRY TraceDeregisterA(DWORD);
DWORD APIENTRY TraceDeregisterExA(DWORD,DWORD);
DWORD APIENTRY TraceGetConsoleA(DWORD,LPHANDLE);
DWORD WINAPIV /*APIENTRY*/ TracePrintfA(DWORD,LPCSTR,...);
DWORD WINAPIV /*APIENTRY*/ TracePrintfExA(DWORD,DWORD,LPCSTR,...);
DWORD APIENTRY TraceVprintfExA(DWORD,DWORD,LPCSTR,va_list);
DWORD APIENTRY TracePutsExA(DWORD,DWORD,LPCSTR);
DWORD APIENTRY TraceDumpExA(DWORD,DWORD,LPBYTE,DWORD,DWORD,BOOL,LPCSTR);
DWORD APIENTRY TraceRegisterExW(LPCWSTR,DWORD);
DWORD APIENTRY TraceDeregisterW(DWORD);
DWORD APIENTRY TraceDeregisterExW(DWORD,DWORD);
DWORD APIENTRY TraceGetConsoleW(DWORD,LPHANDLE);
DWORD WINAPIV /*APIENTRY*/ TracePrintfW(DWORD,LPCWSTR,...);
DWORD WINAPIV /*APIENTRY*/ TracePrintfExW(DWORD,DWORD,LPCWSTR,...);
DWORD APIENTRY TraceVprintfExW(DWORD,DWORD,LPCWSTR,va_list);
DWORD APIENTRY TracePutsExW(DWORD,DWORD,LPCWSTR);
DWORD APIENTRY TraceDumpExW(DWORD,DWORD,LPBYTE,DWORD,DWORD,BOOL,LPCWSTR);
VOID APIENTRY LogErrorA(DWORD,DWORD,LPSTR*,DWORD);
VOID APIENTRY LogEventA(DWORD,DWORD,DWORD,LPSTR*);
VOID LogErrorW(DWORD,DWORD,LPWSTR*,DWORD);
VOID LogEventW(DWORD,DWORD,DWORD,LPWSTR*);
HANDLE RouterLogRegisterA(LPCSTR);
VOID RouterLogDeregisterA(HANDLE);
VOID RouterLogEventA(HANDLE,DWORD,DWORD,DWORD,LPSTR*,DWORD);
VOID RouterLogEventDataA(HANDLE,DWORD,DWORD,DWORD,LPSTR*,DWORD,LPBYTE);
VOID RouterLogEventStringA(HANDLE,DWORD,DWORD,DWORD,LPSTR*,DWORD,DWORD);
VOID RouterLogEventExA(HANDLE,DWORD,DWORD,DWORD,LPCSTR,...);
VOID RouterLogEventValistExA(HANDLE,DWORD,DWORD,DWORD,LPCSTR,va_list);
DWORD RouterGetErrorStringA(DWORD,LPSTR*);
HANDLE RouterLogRegisterW(LPCWSTR);
VOID RouterLogDeregisterW(HANDLE);
VOID RouterLogEventW(HANDLE,DWORD,DWORD,DWORD,LPWSTR*,DWORD);
VOID RouterLogEventDataW(HANDLE,DWORD,DWORD,DWORD,LPWSTR*,DWORD,LPBYTE);
VOID RouterLogEventStringW(HANDLE,DWORD,DWORD,DWORD,LPWSTR*,DWORD,DWORD);
VOID RouterLogEventExW(HANDLE,DWORD,DWORD,DWORD,LPCWSTR,...);
VOID RouterLogEventValistExW(HANDLE,DWORD,DWORD,DWORD,LPCWSTR,va_list);
DWORD RouterGetErrorStringW(DWORD,LPWSTR*);
DWORD APIENTRY QueueWorkItem(WORKERFUNCTION,PVOID,BOOL);
DWORD APIENTRY SetIoCompletionProc(HANDLE,LPOVERLAPPED_COMPLETION_ROUTINE);
VOID RouterAssert(PSTR,PSTR,DWORD,PSTR);
DWORD APIENTRY MprSetupProtocolEnum(DWORD,LPBYTE*,LPDWORD);
DWORD APIENTRY MprSetupProtocolFree(LPVOID);
PWT_EVENT_ENTRY APIENTRY CreateWaitEvent(HANDLE,LPSECURITY_ATTRIBUTES,BOOL,BOOL,LPCTSTR,BOOL,WORKERFUNCTION,PVOID,DWORD,BOOL);
PWT_EVENT_BINDING APIENTRY CreateWaitEventBinding(PWT_EVENT_ENTRY,WORKERFUNCTION,PVOID,DWORD,BOOL);
PWT_TIMER_ENTRY APIENTRY CreateWaitTimer(WORKERFUNCTION,PVOID,DWORD,BOOL);
DWORD APIENTRY DeRegisterWaitEventBindingSelf(PWT_EVENT_BINDING);
DWORD APIENTRY DeRegisterWaitEventBinding(PWT_EVENT_BINDING);
DWORD APIENTRY DeRegisterWaitEventsTimers(PLIST_ENTRY,PLIST_ENTRY);
DWORD APIENTRY DeRegisterWaitEventsTimersSelf(PLIST_ENTRY,PLIST_ENTRY);
DWORD APIENTRY RegisterWaitEventBinding(PWT_EVENT_BINDING);
DWORD APIENTRY RegisterWaitEventsTimers(PLIST_ENTRY,PLIST_ENTRY);
DWORD APIENTRY UpdateWaitTimer(PWT_TIMER_ENTRY,LONGLONG*);
VOID APIENTRY WTFree(PVOID);
VOID APIENTRY WTFreeEvent(PWT_EVENT_ENTRY);
VOID APIENTRY WTFreeTimer(PWT_TIMER_ENTRY);
VOID APIENTRY DebugPrintWaitWorkerThreads(DWORD);

#define TraceRegisterA(a)  TraceRegisterExA(a,0)
#define TraceVprintfA(a,b,c)  TraceVprintfExA(a,0,b,c)
#define TracePutsA(a,b)  TracePutsExA(a,0,b)
#define TraceDumpA(a,b,c,d,e,f)  TraceDumpExA(a,0,b,c,d,e,f)

#define TraceRegisterW(a)  TraceRegisterExW(a,0)
#define TraceVprintfW(a,b,c)  TraceVprintfExW(a,0,b,c)
#define TracePutsW(a,b)  TracePutsExW(a,0,b)
#define TraceDumpW(a,b,c,d,e,f)  TraceDumpExW(a,0,b,c,d,e,f)

#define RouterLogErrorA(h,msg,count,array,err)  RouterLogEventA(h,EVENTLOG_ERROR_TYPE,msg,count,array,err)
#define RouterLogWarningA(h,msg,count,array,err)  RouterLogEventA(h,EVENTLOG_WARNING_TYPE,msg,count,array,err)
#define RouterLogInformationA(h,msg,count,array,err)  RouterLogEventA(h,EVENTLOG_INFORMATION_TYPE,msg,count,array,err)

#define RouterLogErrorDataA(h,msg,count,array,c,buf)  RouterLogEventDataA(h,EVENTLOG_ERROR_TYPE,msg,count,array,c,buf)
#define RouterLogWarningDataA(h,msg,count,array,c,buf)  RouterLogEventDataA(h,EVENTLOG_WARNING_TYPE,msg,count,array,c,buf)
#define RouterLogInformationDataA(h,msg,count,array,c,buf)  RouterLogEventDataA(h,EVENTLOG_INFORMATION_TYPE,msg,count,array,c,buf)

#define RouterLogErrorStringA(h,msg,count,array,err,index)  RouterLogEventStringA(h,EVENTLOG_ERROR_TYPE,msg,count,array,err,index)
#define RouterLogWarningStringA(h,msg,count,array,err,index)  RouterLogEventStringA(h,EVENTLOG_WARNING_TYPE,msg,count,array,err,index)
#define RouterLogInformationStringA(h,msg,count,array,err,index)  RouterLogEventStringA(h,EVENTLOG_INFORMATION_TYPE,msg,count,array,err,index)

#define RouterLogErrorW(h,msg,count,array,err)  RouterLogEventW(h,EVENTLOG_ERROR_TYPE,msg,count,array,err)
#define RouterLogWarningW(h,msg,count,array,err)  RouterLogEventW(h,EVENTLOG_WARNING_TYPE,msg,count,array,err)
#define RouterLogInformationW(h,msg,count,array,err)  RouterLogEventW(h,EVENTLOG_INFORMATION_TYPE,msg,count,array,err)

#define RouterLogErrorDataW(h,msg,count,array,c,buf)  RouterLogEventDataW(h,EVENTLOG_ERROR_TYPE,msg,count,array,c,buf)
#define RouterLogWarningDataW(h,msg,count,array,c,buf)  RouterLogEventDataW(h,EVENTLOG_WARNING_TYPE,msg,count,array,c,buf)
#define RouterLogInformationDataW(h,msg,count,array,c,buf)  RouterLogEventDataW(h,EVENTLOG_INFORMATION_TYPE,msg,count,array,c,buf)

#define RouterLogErrorStringW(h,msg,count,array,err,index)  RouterLogEventStringW(h,EVENTLOG_ERROR_TYPE,msg,count,array,err,index)
#define RouterLogWarningStringW(h,msg,count,array,err,index)  RouterLogEventStringW(h,EVENTLOG_WARNING_TYPE,msg,count,array,err,index)
#define RouterLogInformationStringW(h,msg,count,array,err,index)  RouterLogEventStringW(h,EVENTLOG_INFORMATION_TYPE,msg,count,array,err,index)

#ifdef UNICODE
#define TraceRegister  TraceRegisterW
#define TraceDeregister  TraceDeregisterW
#define TraceDeregisterEx  TraceDeregisterExW
#define TraceGetConsole  TraceGetConsoleW
#define TracePrintf  TracePrintfW
#define TraceVprintf  TraceVprintfW
#define TracePuts  TracePutsW
#define TraceDump  TraceDumpW
#define TraceRegisterEx  TraceRegisterExW
#define TracePrintfEx  TracePrintfExW
#define TraceVprintfEx  TraceVprintfExW
#define TracePutsEx  TracePutsExW
#define TraceDumpEx  TraceDumpExW
#define LogError  LogErrorW
#define LogEvent  LogEventW
#define RouterLogRegister  RouterLogRegisterW
#define RouterLogDeregister  RouterLogDeregisterW
#define RouterLogEvent  RouterLogEventW
#define RouterLogError  RouterLogErrorW
#define RouterLogWarning  RouterLogWarningW
#define RouterLogInformation  RouterLogInformationW
#define RouterLogEventData  RouterLogEventDataW
#define RouterLogErrorData  RouterLogErrorDataW
#define RouterLogWarningData  RouterLogWarningDataW
#define RouterLogInformationData  RouterLogInformationDataW
#define RouterLogEventString  RouterLogEventStringW
#define RouterLogEventEx  RouterLogEventExW
#define RouterLogEventValistEx  RouterLogEventValistExW
#define RouterLogErrorString  RouterLogErrorStringW
#define RouterLogWarningString  RouterLogWarningStringW
#define RouterLogInformationString  RouterLogInformationStringW
#define RouterGetErrorString  RouterGetErrorStringW
#else
#define TraceRegister  TraceRegisterA
#define TraceDeregister  TraceDeregisterA
#define TraceDeregisterEx  TraceDeregisterExA
#define TraceGetConsole  TraceGetConsoleA
#define TracePrintf  TracePrintfA
#define TraceVprintf  TraceVprintfA
#define TracePuts  TracePutsA
#define TraceDump  TraceDumpA
#define TraceRegisterEx  TraceRegisterExA
#define TracePrintfEx  TracePrintfExA
#define TraceVprintfEx  TraceVprintfExA
#define TracePutsEx  TracePutsExA
#define TraceDumpEx  TraceDumpExA
#define LogError  LogErrorA
#define LogEvent  LogEventA
#define RouterLogRegister  RouterLogRegisterA
#define RouterLogDeregister  RouterLogDeregisterA
#define RouterLogEvent  RouterLogEventA
#define RouterLogError  RouterLogErrorA
#define RouterLogWarning  RouterLogWarningA
#define RouterLogInformation  RouterLogInformationA
#define RouterLogEventData  RouterLogEventDataA
#define RouterLogErrorData  RouterLogErrorDataA
#define RouterLogWarningData  RouterLogWarningDataA
#define RouterLogInformationData  RouterLogInformationDataA
#define RouterLogEventString  RouterLogEventStringA
#define RouterLogEventEx  RouterLogEventExA
#define RouterLogEventValistEx  RouterLogEventValistExA
#define RouterLogErrorString  RouterLogErrorStringA
#define RouterLogWarningString  RouterLogWarningStringA
#define RouterLogInformationString  RouterLogInformationStringA
#define RouterGetErrorString  RouterGetErrorStringA
#endif /* UNICODE */

#ifdef __cplusplus
}
#endif

#endif /* _RTUTILS_H */
