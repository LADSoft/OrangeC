#ifndef _PDH_H
#define _PDH_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Performance Data Helper definitions */

#include <windows.h>
#include <winperf.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef LONG PDH_STATUS;

#define PDH_FUNCTION  PDH_STATUS __stdcall

#define PDH_CVERSION_WIN40  ((DWORD)0x0400)
#define PDH_CVERSION_WIN50  ((DWORD)0x0500)
#define PDH_VERSION  ((DWORD)((PDH_CVERSION_WIN50)+0x0003))

#define IsSuccessSeverity(ErrorCode)  ((((DWORD)(ErrorCode)&(0xC0000000L))==0x00000000L)?TRUE:FALSE)
#define IsInformationalSeverity(ErrorCode)  ((((DWORD)(ErrorCode)&(0xC0000000L))==0x40000000L)?TRUE:FALSE)
#define IsWarningSeverity(ErrorCode)  ((((DWORD)(ErrorCode)&(0xC0000000L))==0x80000000L)?TRUE:FALSE)
#define IsErrorSeverity(ErrorCode)  ((((DWORD)(ErrorCode)&(0xC0000000L))==0xC0000000L)?TRUE:FALSE)

#if (_WIN32_WINDOWS >= 0x0501 || _WIN32_WINNT >= 0x0501 || (defined(NTDDI_VERSION) && NTDDI_VERSION >= NTDDI_WINXP))
#define MAX_COUNTER_PATH  256

#if (_WIN32_WINDOWS >= 0x0502 || _WIN32_WINNT >= 0x0502 || (defined(NTDDI_VERSION) && NTDDI_VERSION >= NTDDI_WIN2003))
#define PDH_MAX_COUNTER_NAME  1024
#define PDH_MAX_INSTANCE_NAME  1024
#define PDH_MAX_COUNTER_PATH  2048
#define PDH_MAX_DATASOURCE_PATH  1024
#endif /* _WIN32_WINDOWS >= 0x0502 || ... */

#define PDH_OBJECT_HAS_INSTANCES    ((DWORD) 0x00000001)
#endif /* _WIN32_WINDOWS >= 0x0501 || ... */

#define MAX_TIME_VALUE  ((LONGLONG)0x7FFFFFFFFFFFFFFF)
#define MIN_TIME_VALUE  ((LONGLONG)0)

#define PDH_FMT_RAW  ((DWORD)0x00000010)
#define PDH_FMT_ANSI  ((DWORD)0x00000020)
#define PDH_FMT_UNICODE  ((DWORD)0x00000040)
#define PDH_FMT_LONG  ((DWORD)0x00000100)
#define PDH_FMT_DOUBLE  ((DWORD)0x00000200)
#define PDH_FMT_LARGE  ((DWORD)0x00000400)
#define PDH_FMT_NOSCALE  ((DWORD)0x00001000)
#define PDH_FMT_1000  ((DWORD)0x00002000)
#define PDH_FMT_NODATA  ((DWORD)0x00004000)
#define PDH_FMT_NOCAP100  ((DWORD)0x00008000)
#define PERF_DETAIL_COSTLY  ((DWORD)0x00010000)
#define PERF_DETAIL_STANDARD  ((DWORD)0x0000FFFF)

#define PDH_MAX_SCALE  (7L)
#define PDH_MIN_SCALE  (-7L)

#define PDH_PATH_WBEM_RESULT  ((DWORD)0x00000001)
#define PDH_PATH_WBEM_INPUT  ((DWORD)0x00000002)
#define PDH_PATH_LANG_FLAGS(LangId, Flags)  ((DWORD)(((LangId&0x0000FFFF)<<16)|(Flags&0x0000FFFF)))

#define PDH_NOEXPANDCOUNTERS  1
#define PDH_NOEXPANDINSTANCES  2
#if (_WIN32_WINDOWS >= 0x0502 || _WIN32_WINNT >= 0x0502 || (defined(NTDDI_VERSION) && NTDDI_VERSION >= NTDDI_WIN2003))
#define PDH_REFRESHCOUNTERS  4
#endif /* _WIN32_WINDOWS >= 0x0502 || ... */

#define PDH_LOG_READ_ACCESS  ((DWORD)0x00010000)
#define PDH_LOG_WRITE_ACCESS  ((DWORD)0x00020000)
#define PDH_LOG_UPDATE_ACCESS  ((DWORD)0x00040000)
#define PDH_LOG_ACCESS_MASK  ((DWORD)0x000F0000)
#define PDH_LOG_CREATE_NEW  ((DWORD)0x00000001)
#define PDH_LOG_CREATE_ALWAYS  ((DWORD)0x00000002)
#define PDH_LOG_OPEN_ALWAYS  ((DWORD)0x00000003)
#define PDH_LOG_OPEN_EXISTING  ((DWORD)0x00000004)
#define PDH_LOG_CREATE_MASK  ((DWORD)0x0000000F)
#define PDH_LOG_OPT_USER_STRING  ((DWORD)0x01000000)
#define PDH_LOG_OPT_CIRCULAR  ((DWORD)0x02000000)
#define PDH_LOG_OPT_MAX_IS_BYTES  ((DWORD)0x04000000)
#if (_WIN32_WINDOWS >= 0x0501 || _WIN32_WINNT >= 0x0501 || (defined(NTDDI_VERSION) && NTDDI_VERSION >= NTDDI_WINXP))
#define PDH_LOG_OPT_APPEND  ((DWORD) 0x08000000)
#endif /* _WIN32_WINDOWS >= 0x0501 || ... */
#define PDH_LOG_OPT_MASK  ((DWORD)0x0F000000)
#define PDH_LOG_TYPE_UNDEFINED  0
#define PDH_LOG_TYPE_CSV  1
#define PDH_LOG_TYPE_TSV  2
#define PDH_LOG_TYPE_TRACE_KERNEL  4
#define PDH_LOG_TYPE_TRACE_GENERIC  5
#define PDH_LOG_TYPE_PERFMON  6
#if (_WIN32_WINDOWS >= 0x0501 || _WIN32_WINNT >= 0x0501 || (defined(NTDDI_VERSION) && NTDDI_VERSION >= NTDDI_WINXP))
#define PDH_LOG_TYPE_SQL  7
#define PDH_LOG_TYPE_BINARY  8
#else
#define PDH_LOG_TYPE_BINARY  3
#endif /* _WIN32_WINDOWS >= 0x0501 || ... */

#define PDH_FLAGS_CLOSE_QUERY  ((DWORD)0x00000001)
#define PDH_FLAGS_FILE_BROWSER_ONLY ((DWORD)0x00000001)

#define DATA_SOURCE_REGISTRY  ((DWORD)0x00000001)
#define DATA_SOURCE_LOGFILE  ((DWORD)0x00000002)
#define DATA_SOURCE_WBEM  ((DWORD)0x00000004)

/*
#define PDH_LOGSVC_NO_WAIT  ((DWORD)0x80000000)
#define PDH_LOGSVC_ALL_QUERIES  ((DWORD)0x00000001)
#define PDH_LOGSVC_TRACE_LOG  ((DWORD)0x00000002)
#define PDH_LOGSVC_CMD_START  ((DWORD)0x00000010)
#define PDH_LOGSVC_CMD_STOP  ((DWORD)0x00000020)
#define PDH_LOGSVC_CTRL_ADD  ((DWORD)0x00000100)
#define PDH_LOGSVC_CTRL_REMOVE  ((DWORD)0x00000200)
#define PDH_LOGSVC_CTRL_INFO  ((DWORD)0x00000400)
#define PDH_LOGSVC_STATUS_RUNNING  ((DWORD)0x00001000)
#define PDH_LOGSVC_STATUS_STOPPED  ((DWORD)0x00002000)
#define PDH_LOGSVC_STATUS_PAUSED  ((DWORD)0x00004000)
#define PDH_LOGSVC_STATUS_ERROR  ((DWORD)0x00008000)
#define PDH_LOGSVC_STATUS_PENDING  ((DWORD)0x00010000)
#define PDH_LOGSVC_NAME_UNDEFINED  ((DWORD)-1)
#define PDH_LOGSVC_NAME_MMDDHH  ((DWORD)0)
#define PDH_LOGSVC_NAME_NNNNNN  ((DWORD)1)
#define PDH_LOGSVC_NAME_YYDDD  ((DWORD)2)
#define PDH_LOGSVC_NAME_YYMM  ((DWORD)3)
#define PDH_LOGSVC_NAME_YYMMDD  ((DWORD)4)
#define PDH_LOGSVC_NAME_YYMMDDHH  ((DWORD)5)
#define PDH_LOGSVC_RENAME_UNDEFINED ((DWORD)-1)
#define PDH_LOGSVC_RENAME_HOURS  ((DWORD)0)
#define PDH_LOGSVC_RENAME_DAYS  ((DWORD)1)
#define PDH_LOGSVC_RENAME_MONTHS  ((DWORD)2)
#define PDH_LOGSVC_RENAME_KBYTES  ((DWORD)3)
#define PDH_LOGSVC_RENAME_MBYTES  ((DWORD)4)
*/

typedef HANDLE PDH_HCOUNTER;
typedef HANDLE PDH_HQUERY;
typedef HANDLE PDH_HLOG;

typedef PDH_HCOUNTER HCOUNTER;
typedef PDH_HQUERY HQUERY;
#ifndef _LMHLOGDEFINED_
typedef PDH_HLOG HLOG;
#endif /* _LMHLOGDEFINED_ */

typedef struct _PDH_RAW_COUNTER {
    volatile DWORD CStatus;
    FILETIME TimeStamp;
    LONGLONG FirstValue;
    LONGLONG SecondValue;
    DWORD MultiCount;
} PDH_RAW_COUNTER, *PPDH_RAW_COUNTER;

typedef struct _PDH_RAW_COUNTER_ITEM_A {
    LPSTR szName;
    PDH_RAW_COUNTER RawValue;
} PDH_RAW_COUNTER_ITEM_A, *PPDH_RAW_COUNTER_ITEM_A;

typedef struct _PDH_RAW_COUNTER_ITEM_W {
    LPWSTR szName;
    PDH_RAW_COUNTER RawValue;
} PDH_RAW_COUNTER_ITEM_W, *PPDH_RAW_COUNTER_ITEM_W;

typedef struct _PDH_FMT_COUNTERVALUE {
    DWORD CStatus;
    union {
        LONG longValue;
        double doubleValue;
        LONGLONG largeValue;
        LPCSTR AnsiStringValue;
        LPCWSTR WideStringValue;
    };
} PDH_FMT_COUNTERVALUE, *PPDH_FMT_COUNTERVALUE;

typedef struct _PDH_FMT_COUNTERVALUE_ITEM_A {
    LPSTR szName;
    PDH_FMT_COUNTERVALUE FmtValue;
} PDH_FMT_COUNTERVALUE_ITEM_A, *PPDH_FMT_COUNTERVALUE_ITEM_A;

typedef struct _PDH_FMT_COUNTERVALUE_ITEM_W {
    LPWSTR szName;
    PDH_FMT_COUNTERVALUE FmtValue;
} PDH_FMT_COUNTERVALUE_ITEM_W, *PPDH_FMT_COUNTERVALUE_ITEM_W;

typedef struct _PDH_STATISTICS {
    DWORD dwFormat;
    DWORD count;
    PDH_FMT_COUNTERVALUE min;
    PDH_FMT_COUNTERVALUE max;
    PDH_FMT_COUNTERVALUE mean;
} PDH_STATISTICS, *PPDH_STATISTICS;

typedef struct _PDH_COUNTER_PATH_ELEMENTS_A {
    LPSTR szMachineName;
    LPSTR szObjectName;
    LPSTR szInstanceName;
    LPSTR szParentInstance;
    DWORD dwInstanceIndex;
    LPSTR szCounterName;
} PDH_COUNTER_PATH_ELEMENTS_A, *PPDH_COUNTER_PATH_ELEMENTS_A;

typedef struct _PDH_COUNTER_PATH_ELEMENTS_W {
    LPWSTR szMachineName;
    LPWSTR szObjectName;
    LPWSTR szInstanceName;
    LPWSTR szParentInstance;
    DWORD dwInstanceIndex;
    LPWSTR szCounterName;
} PDH_COUNTER_PATH_ELEMENTS_W, *PPDH_COUNTER_PATH_ELEMENTS_W;

typedef struct _PDH_DATA_ITEM_PATH_ELEMENTS_A {
    LPSTR szMachineName;
    GUID ObjectGUID;
    DWORD dwItemId;
    LPSTR szInstanceName;
} PDH_DATA_ITEM_PATH_ELEMENTS_A, *PPDH_DATA_ITEM_PATH_ELEMENTS_A;

typedef struct _PDH_DATA_ITEM_PATH_ELEMENTS_W {
    LPWSTR szMachineName;
    GUID ObjectGUID;
    DWORD dwItemId;
    LPWSTR szInstanceName;
} PDH_DATA_ITEM_PATH_ELEMENTS_W, *PPDH_DATA_ITEM_PATH_ELEMENTS_W;

typedef struct _PDH_COUNTER_INFO_A {
    DWORD dwLength;
    DWORD dwType;
    DWORD CVersion;
    DWORD CStatus;
    LONG lScale;
    LONG lDefaultScale;
    DWORD_PTR dwUserData;
    DWORD_PTR dwQueryUserData;
    LPSTR szFullPath;
    union {
        PDH_DATA_ITEM_PATH_ELEMENTS_A DataItemPath;
        PDH_COUNTER_PATH_ELEMENTS_A CounterPath;
        struct {
            LPSTR szMachineName;
            LPSTR szObjectName;
            LPSTR szInstanceName;
            LPSTR szParentInstance;
            DWORD dwInstanceIndex;
            LPSTR szCounterName;
        };
    };
    LPSTR szExplainText;
    DWORD DataBuffer[1];
} PDH_COUNTER_INFO_A, *PPDH_COUNTER_INFO_A;

typedef struct _PDH_COUNTER_INFO_W {
    DWORD dwLength;
    DWORD dwType;
    DWORD CVersion;
    DWORD CStatus;
    LONG lScale;
    LONG lDefaultScale;
    DWORD_PTR dwUserData;
    DWORD_PTR dwQueryUserData;
    LPWSTR szFullPath;
    union {
        PDH_DATA_ITEM_PATH_ELEMENTS_W DataItemPath;
        PDH_COUNTER_PATH_ELEMENTS_W CounterPath;
        struct {
            LPWSTR szMachineName;
            LPWSTR szObjectName;
            LPWSTR szInstanceName;
            LPWSTR szParentInstance;
            DWORD dwInstanceIndex;
            LPWSTR szCounterName;
        };
    };
    LPWSTR szExplainText;
    DWORD DataBuffer[1];
} PDH_COUNTER_INFO_W, *PPDH_COUNTER_INFO_W;

typedef struct _PDH_TIME_INFO {
    LONGLONG StartTime;
    LONGLONG EndTime;
    DWORD SampleCount;
} PDH_TIME_INFO, *PPDH_TIME_INFO;

typedef struct _PDH_RAW_LOG_RECORD {
    DWORD dwStructureSize;
    DWORD dwRecordType;
    DWORD dwItems;
    UCHAR RawBytes[1];
} PDH_RAW_LOG_RECORD, *PPDH_RAW_LOG_RECORD;

typedef struct _PDH_LOG_SERVICE_QUERY_INFO_A {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwLogQuota;
    LPSTR szLogFileCaption;
    LPSTR szDefaultDir;
    LPSTR szBaseFileName;
    DWORD dwFileType;
    DWORD dwReserved;
    union {
        struct {
            DWORD PdlAutoNameInterval;
            DWORD PdlAutoNameUnits;
            LPSTR PdlCommandFilename;
            LPSTR PdlCounterList;
            DWORD PdlAutoNameFormat;
            DWORD PdlSampleInterval;
            FILETIME PdlLogStartTime;
            FILETIME PdlLogEndTime;
        };
        struct {
            DWORD TlNumberOfBuffers;
            DWORD TlMinimumBuffers;
            DWORD TlMaximumBuffers;
            DWORD TlFreeBuffers;
            DWORD TlBufferSize;
            DWORD TlEventsLost;
            DWORD TlLoggerThreadId;
            DWORD TlBuffersWritten;
            DWORD TlLogHandle;
            LPSTR TlLogFileName;
        };
    };
} PDH_LOG_SERVICE_QUERY_INFO_A, *PPDH_LOG_SERVICE_QUERY_INFO_A;

typedef struct _PDH_LOG_SERVICE_QUERY_INFO_W {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwLogQuota;
    LPWSTR szLogFileCaption;
    LPWSTR szDefaultDir;
    LPWSTR szBaseFileName;
    DWORD dwFileType;
    DWORD dwReserved;
    union {
        struct {
            DWORD PdlAutoNameInterval;
            DWORD PdlAutoNameUnits;
            LPWSTR PdlCommandFilename;
            LPWSTR PdlCounterList;
            DWORD PdlAutoNameFormat;
            DWORD PdlSampleInterval;
            FILETIME PdlLogStartTime;
            FILETIME PdlLogEndTime;
        };
        struct {
            DWORD TlNumberOfBuffers;
            DWORD TlMinimumBuffers;
            DWORD TlMaximumBuffers;
            DWORD TlFreeBuffers;
            DWORD TlBufferSize;
            DWORD TlEventsLost;
            DWORD TlLoggerThreadId;
            DWORD TlBuffersWritten;
            DWORD TlLogHandle;
            LPWSTR TlLogFileName;
        };
    };
} PDH_LOG_SERVICE_QUERY_INFO_W, *PPDH_LOG_SERVICE_QUERY_INFO_W;

typedef PDH_STATUS(__stdcall *CounterPathCallBack)(DWORD_PTR);

#if (_WIN32_WINDOWS >= 0x0501 || _WIN32_WINNT >= 0x0501 || (defined(NTDDI_VERSION) && NTDDI_VERSION >= NTDDI_WINXP))
typedef struct _BrowseDlgConfig_HW {
    DWORD bIncludeInstanceIndex:1,
          bSingleCounterPerAdd:1,
          bSingleCounterPerDialog:1,
          bLocalCountersOnly:1,
          bWildCardInstances:1,
          bHideDetailBox:1,
          bInitializePath:1,
          bDisableMachineSelection:1,
          bIncludeCostlyObjects:1,
          bShowObjectBrowser:1,
          bReserved:22;
    HWND hWndOwner;
    PDH_HLOG hDataSource;
    LPWSTR szReturnPathBuffer;
    DWORD cchReturnPathLength;
    CounterPathCallBack pCallBack;
    DWORD_PTR dwCallBackArg;
    PDH_STATUS CallBackStatus;
    DWORD dwDefaultDetailLevel;
    LPWSTR szDialogBoxCaption;
} PDH_BROWSE_DLG_CONFIG_HW, *PPDH_BROWSE_DLG_CONFIG_HW;

typedef struct _BrowseDlgConfig_HA {
    DWORD bIncludeInstanceIndex:1,
          bSingleCounterPerAdd:1,
          bSingleCounterPerDialog:1,
          bLocalCountersOnly:1,
          bWildCardInstances:1,
          bHideDetailBox:1,
          bInitializePath:1,
          bDisableMachineSelection:1,
          bIncludeCostlyObjects:1,
          bShowObjectBrowser:1,
          bReserved:22;
    HWND hWndOwner;
    PDH_HLOG hDataSource;
    LPSTR szReturnPathBuffer;
    DWORD cchReturnPathLength;
    CounterPathCallBack pCallBack;
    DWORD_PTR dwCallBackArg;
    PDH_STATUS CallBackStatus;
    DWORD dwDefaultDetailLevel;
    LPSTR szDialogBoxCaption;
} PDH_BROWSE_DLG_CONFIG_HA, *PPDH_BROWSE_DLG_CONFIG_HA;

typedef struct _BrowseDlgConfig_W {
    DWORD bIncludeInstanceIndex:1,
          bSingleCounterPerAdd:1,
          bSingleCounterPerDialog:1,
          bLocalCountersOnly:1,
          bWildCardInstances:1,
          bHideDetailBox:1,
          bInitializePath:1,
          bDisableMachineSelection:1,
          bIncludeCostlyObjects:1,
          bShowObjectBrowser:1,
          bReserved:22;
    HWND hWndOwner;
    LPWSTR szDataSource;
    LPWSTR szReturnPathBuffer;
    DWORD cchReturnPathLength;
    CounterPathCallBack pCallBack;
    DWORD_PTR dwCallBackArg;
    PDH_STATUS CallBackStatus;
    DWORD dwDefaultDetailLevel;
    LPWSTR szDialogBoxCaption;
} PDH_BROWSE_DLG_CONFIG_W, *PPDH_BROWSE_DLG_CONFIG_W;

typedef struct _BrowseDlgConfig_A {
    DWORD bIncludeInstanceIndex:1,
          bSingleCounterPerAdd:1,
          bSingleCounterPerDialog:1,
          bLocalCountersOnly:1,
          bWildCardInstances:1,
          bHideDetailBox:1,
          bInitializePath:1,
          bDisableMachineSelection:1,
          bIncludeCostlyObjects:1,
          bShowObjectBrowser:1,
          bReserved:22;
    HWND hWndOwner;
    LPSTR szDataSource;
    LPSTR szReturnPathBuffer;
    DWORD cchReturnPathLength;
    CounterPathCallBack pCallBack;
    DWORD_PTR dwCallBackArg;
    PDH_STATUS CallBackStatus;
    DWORD dwDefaultDetailLevel;
    LPSTR szDialogBoxCaption;
} PDH_BROWSE_DLG_CONFIG_A, *PPDH_BROWSE_DLG_CONFIG_A;
#else /* yada, yada, yada */
typedef struct _BrowseDlgConfig_W {
    DWORD bIncludeInstanceIndex:1,
          bSingleCounterPerAdd:1,
          bSingleCounterPerDialog:1,
          bLocalCountersOnly:1,
          bWildCardInstances:1,
          bHideDetailBox:1,
          bInitializePath:1,
          bDisableMachineSelection:1,
          bIncludeCostlyObjects:1,
          bReserved:23;
    HWND hWndOwner;
    LPWSTR szDataSource;
    LPWSTR szReturnPathBuffer;
    DWORD cchReturnPathLength;
    CounterPathCallBack pCallBack;
    DWORD_PTR dwCallBackArg;
    PDH_STATUS CallBackStatus;
    DWORD dwDefaultDetailLevel;
    LPWSTR szDialogBoxCaption;
} PDH_BROWSE_DLG_CONFIG_W, *PPDH_BROWSE_DLG_CONFIG_W;

typedef struct _BrowseDlgConfig_A {
    DWORD bIncludeInstanceIndex:1,
          bSingleCounterPerAdd:1,
          bSingleCounterPerDialog:1,
          bLocalCountersOnly:1,
          bWildCardInstances:1,
          bHideDetailBox:1,
          bInitializePath:1,
          bDisableMachineSelection:1,
          bIncludeCostlyObjects:1,
          bReserved:23;
    HWND hWndOwner;
    LPSTR szDataSource;
    LPSTR szReturnPathBuffer;
    DWORD cchReturnPathLength;
    CounterPathCallBack pCallBack;
    DWORD_PTR dwCallBackArg;
    PDH_STATUS CallBackStatus;
    DWORD dwDefaultDetailLevel;
    LPSTR szDialogBoxCaption;
} PDH_BROWSE_DLG_CONFIG_A, *PPDH_BROWSE_DLG_CONFIG_A;
#endif /* _WIN32_WINDOWS >= 0x0501 || ... */

PDH_FUNCTION PdhGetDllVersion(LPDWORD);
PDH_FUNCTION PdhOpenQueryW(LPCWSTR,DWORD_PTR,PDH_HQUERY*);
PDH_FUNCTION PdhOpenQueryA(LPCSTR,DWORD_PTR,PDH_HQUERY*);
PDH_FUNCTION PdhAddCounterW(PDH_HQUERY,LPCWSTR,DWORD_PTR,PDH_HCOUNTER*);
PDH_FUNCTION PdhAddCounterA(PDH_HQUERY,LPCSTR,DWORD_PTR,PDH_HCOUNTER*);

#if (_WIN32_WINDOWS >= 0x0600 || _WIN32_WINNT >= 0x0600 || (defined(NTDDI_VERSION) && NTDDI_VERSION >= NTDDI_VISTA))
PDH_FUNCTION PdhAddEnglishCounterW(PDH_HQUERY,LPCWSTR,DWORD_PTR,PDH_HCOUNTER*);
PDH_FUNCTION PdhAddEnglishCounterA(PDH_HQUERY,LPCSTR,DWORD_PTR,PDH_HCOUNTER*);
PDH_FUNCTION PdhCollectQueryDataWithTime(PDH_HQUERY,LONGLONG*);
PDH_FUNCTION PdhValidatePathExW(PDH_HLOG,LPCWSTR);
PDH_FUNCTION PdhValidatePathExA(PDH_HLOG,LPCSTR);
#endif /* _WIN32_WINDOWS >= 0x0600 || ... */

PDH_FUNCTION PdhRemoveCounter(PDH_HCOUNTER);
PDH_FUNCTION PdhCollectQueryData(PDH_HQUERY);
PDH_FUNCTION PdhCloseQuery(PDH_HQUERY);
PDH_FUNCTION PdhGetFormattedCounterValue(PDH_HCOUNTER,DWORD,LPDWORD,PPDH_FMT_COUNTERVALUE);
PDH_FUNCTION PdhGetFormattedCounterArrayA(PDH_HCOUNTER,DWORD,LPDWORD,LPDWORD,PPDH_FMT_COUNTERVALUE_ITEM_A);
PDH_FUNCTION PdhGetFormattedCounterArrayW(PDH_HCOUNTER,DWORD,LPDWORD,LPDWORD,PPDH_FMT_COUNTERVALUE_ITEM_W);
PDH_FUNCTION PdhGetRawCounterValue(PDH_HCOUNTER,LPDWORD,PPDH_RAW_COUNTER);
PDH_FUNCTION PdhGetRawCounterArrayA(PDH_HCOUNTER,LPDWORD,LPDWORD,PPDH_RAW_COUNTER_ITEM_A);
PDH_FUNCTION PdhGetRawCounterArrayW(PDH_HCOUNTER,LPDWORD,LPDWORD,PPDH_RAW_COUNTER_ITEM_W);
PDH_FUNCTION PdhCalculateCounterFromRawValue(PDH_HCOUNTER,DWORD,PPDH_RAW_COUNTER,PPDH_RAW_COUNTER,PPDH_FMT_COUNTERVALUE);
PDH_FUNCTION PdhComputeCounterStatistics(PDH_HCOUNTER,DWORD,DWORD,DWORD,PPDH_RAW_COUNTER,PPDH_STATISTICS);
PDH_FUNCTION PdhGetCounterInfoW(PDH_HCOUNTER,BOOLEAN,LPDWORD,PPDH_COUNTER_INFO_W);
PDH_FUNCTION PdhGetCounterInfoA(PDH_HCOUNTER,BOOLEAN,LPDWORD,PPDH_COUNTER_INFO_A);
PDH_FUNCTION PdhSetCounterScaleFactor(PDH_HCOUNTER,LONG);
PDH_FUNCTION PdhConnectMachineW(LPCWSTR);
PDH_FUNCTION PdhConnectMachineA(LPCSTR);
PDH_FUNCTION PdhEnumMachinesW(LPCWSTR,LPWSTR,LPDWORD);
PDH_FUNCTION PdhEnumMachinesA(LPCSTR,LPSTR,LPDWORD);
PDH_FUNCTION PdhEnumObjectsW(LPCWSTR,LPCWSTR,LPWSTR,LPDWORD,DWORD,BOOL);
PDH_FUNCTION PdhEnumObjectsA(LPCSTR,LPCSTR,LPSTR,LPDWORD,DWORD,BOOL);
PDH_FUNCTION PdhEnumObjectItemsW(LPCWSTR,LPCWSTR,LPCWSTR,LPWSTR,LPDWORD,LPWSTR,LPDWORD,DWORD,DWORD);
PDH_FUNCTION PdhEnumObjectItemsA(LPCSTR,LPCSTR,LPCSTR,LPSTR,LPDWORD,LPSTR,LPDWORD,DWORD,DWORD);
PDH_FUNCTION PdhMakeCounterPathW(PPDH_COUNTER_PATH_ELEMENTS_W,LPWSTR,LPDWORD,DWORD);
PDH_FUNCTION PdhMakeCounterPathA(PPDH_COUNTER_PATH_ELEMENTS_A,LPSTR,LPDWORD,DWORD);
PDH_FUNCTION PdhParseCounterPathW(LPCWSTR,PPDH_COUNTER_PATH_ELEMENTS_W,LPDWORD,DWORD);
PDH_FUNCTION PdhParseCounterPathA(LPCSTR,PPDH_COUNTER_PATH_ELEMENTS_A,LPDWORD,DWORD);
PDH_FUNCTION PdhParseInstanceNameW(LPCWSTR,LPWSTR,LPDWORD,LPWSTR,LPDWORD,LPDWORD);
PDH_FUNCTION PdhParseInstanceNameA(LPCSTR,LPSTR,LPDWORD,LPSTR,LPDWORD,LPDWORD);
PDH_FUNCTION PdhValidatePathW(LPCWSTR);
PDH_FUNCTION PdhValidatePathA(LPCSTR);
PDH_FUNCTION PdhGetDefaultPerfObjectW(LPCWSTR,LPCWSTR,LPWSTR,LPDWORD);
PDH_FUNCTION PdhGetDefaultPerfObjectA(LPCSTR,LPCSTR,LPSTR,LPDWORD);
PDH_FUNCTION PdhGetDefaultPerfCounterW(LPCWSTR,LPCWSTR,LPCWSTR,LPWSTR,LPDWORD);
PDH_FUNCTION PdhGetDefaultPerfCounterA(LPCSTR,LPCSTR,LPCSTR,LPSTR,LPDWORD);

PDH_FUNCTION PdhBrowseCountersW(PPDH_BROWSE_DLG_CONFIG_W);
PDH_FUNCTION PdhBrowseCountersA(PPDH_BROWSE_DLG_CONFIG_A);
PDH_FUNCTION PdhExpandCounterPathW(LPCWSTR,LPWSTR,LPDWORD);
PDH_FUNCTION PdhExpandCounterPathA(LPCSTR,LPSTR,LPDWORD);
PDH_FUNCTION PdhLookupPerfNameByIndexW(LPCWSTR,DWORD,LPWSTR,LPDWORD);
PDH_FUNCTION PdhLookupPerfNameByIndexA(LPCSTR,DWORD,LPSTR,LPDWORD);
PDH_FUNCTION PdhLookupPerfIndexByNameW(LPCWSTR,LPCWSTR,LPDWORD);
PDH_FUNCTION PdhLookupPerfIndexByNameA(LPCSTR,LPCSTR,LPDWORD);
PDH_FUNCTION PdhExpandWildCardPathA(LPCSTR,LPCSTR,LPSTR,LPDWORD,DWORD);
PDH_FUNCTION PdhExpandWildCardPathW(LPCWSTR,LPCWSTR,LPWSTR,LPDWORD,DWORD);
PDH_FUNCTION PdhOpenLogW(LPCWSTR,DWORD,LPDWORD,PDH_HQUERY,DWORD,LPCWSTR,PDH_HLOG*);
PDH_FUNCTION PdhOpenLogA(LPCSTR,DWORD,LPDWORD,PDH_HQUERY,DWORD,LPCSTR,PDH_HLOG*);
PDH_FUNCTION PdhUpdateLogW(PDH_HLOG,LPCWSTR);
PDH_FUNCTION PdhUpdateLogA(PDH_HLOG,LPCSTR);
PDH_FUNCTION PdhUpdateLogFileCatalog(PDH_HLOG);
PDH_FUNCTION PdhGetLogFileSize(PDH_HLOG,LONGLONG*);
PDH_FUNCTION PdhCloseLog(PDH_HLOG,DWORD);
PDH_FUNCTION PdhSelectDataSourceW(HWND,DWORD,LPWSTR,LPDWORD);
PDH_FUNCTION PdhSelectDataSourceA(HWND,DWORD,LPSTR,LPDWORD);
BOOL PdhIsRealTimeQuery(PDH_HQUERY);
PDH_FUNCTION PdhSetQueryTimeRange(PDH_HQUERY,PPDH_TIME_INFO);
PDH_FUNCTION PdhGetDataSourceTimeRangeW(LPCWSTR,LPDWORD,PPDH_TIME_INFO,LPDWORD);
PDH_FUNCTION PdhGetDataSourceTimeRangeA(LPCSTR,LPDWORD,PPDH_TIME_INFO,LPDWORD);
PDH_FUNCTION PdhCollectQueryDataEx(PDH_HQUERY,DWORD,HANDLE);
PDH_FUNCTION PdhFormatFromRawValue(DWORD,DWORD,LONGLONG*,PPDH_RAW_COUNTER,PPDH_RAW_COUNTER,PPDH_FMT_COUNTERVALUE);
PDH_FUNCTION PdhGetCounterTimeBase(PDH_HCOUNTER,LONGLONG*);
PDH_FUNCTION PdhReadRawLogRecord(PDH_HLOG,FILETIME,PPDH_RAW_LOG_RECORD,LPDWORD);
PDH_FUNCTION PdhSetDefaultRealTimeDataSource(DWORD);

#if (_WIN32_WINDOWS >= 0x0501 || _WIN32_WINNT >= 0x0501 || (defined(NTDDI_VERSION) && NTDDI_VERSION >= NTDDI_WINXP))
PDH_FUNCTION PdhBindInputDataSourceW(PDH_HLOG*,LPCWSTR);
PDH_FUNCTION PdhBindInputDataSourceA(PDH_HLOG*,LPCSTR);
PDH_FUNCTION PdhOpenQueryH(PDH_HLOG,DWORD_PTR,PDH_HQUERY*);
PDH_FUNCTION PdhEnumMachinesHW(PDH_HLOG,LPWSTR,LPDWORD);
PDH_FUNCTION PdhEnumMachinesHA(PDH_HLOG,LPSTR,LPDWORD);
PDH_FUNCTION PdhEnumObjectsHW(PDH_HLOG,LPCWSTR,LPWSTR,LPDWORD,DWORD,BOOL);
PDH_FUNCTION PdhEnumObjectsHA(PDH_HLOG,LPCSTR,LPSTR,LPDWORD,DWORD,BOOL);
PDH_FUNCTION PdhEnumObjectItemsHW(PDH_HLOG,LPCWSTR,LPCWSTR,LPWSTR,LPDWORD,LPWSTR,LPDWORD,DWORD,DWORD);
PDH_FUNCTION PdhEnumObjectItemsHA(PDH_HLOG,LPCSTR,LPCSTR,LPSTR,LPDWORD,LPSTR,LPDWORD,DWORD,DWORD);
PDH_FUNCTION PdhExpandWildCardPathHW(PDH_HLOG,LPCWSTR,LPWSTR,LPDWORD,DWORD);
PDH_FUNCTION PdhExpandWildCardPathHA(PDH_HLOG,LPCSTR,LPSTR,LPDWORD,DWORD);
PDH_FUNCTION PdhGetDataSourceTimeRangeH(PDH_HLOG,LPDWORD,PPDH_TIME_INFO,LPDWORD);
PDH_FUNCTION PdhGetDefaultPerfObjectHW(PDH_HLOG,LPCWSTR,LPWSTR,LPDWORD);
PDH_FUNCTION PdhGetDefaultPerfObjectHA(PDH_HLOG,LPCSTR,LPSTR,LPDWORD);
PDH_FUNCTION PdhGetDefaultPerfCounterHW(PDH_HLOG,LPCWSTR,LPCWSTR,LPWSTR,LPDWORD);
PDH_FUNCTION PdhGetDefaultPerfCounterHA(PDH_HLOG,LPCSTR,LPCSTR,LPSTR,LPDWORD);
PDH_FUNCTION PdhBrowseCountersHW(PPDH_BROWSE_DLG_CONFIG_HW);
PDH_FUNCTION PdhBrowseCountersHA(PPDH_BROWSE_DLG_CONFIG_HA);
PDH_FUNCTION PdhVerifySQLDBW(LPCWSTR);
PDH_FUNCTION PdhVerifySQLDBA(LPCSTR);
PDH_FUNCTION PdhCreateSQLTablesW(LPCWSTR);
PDH_FUNCTION PdhCreateSQLTablesA(LPCSTR);
PDH_FUNCTION PdhEnumLogSetNamesW(LPCWSTR,LPWSTR,LPDWORD);
PDH_FUNCTION PdhEnumLogSetNamesA(LPCSTR,LPSTR,LPDWORD);
PDH_FUNCTION PdhGetLogSetGUID(PDH_HLOG,GUID*,int*);
PDH_FUNCTION PdhSetLogSetRunID(PDH_HLOG,int);
#endif /* _WIN32_WINDOWS >= 0x0501 || ... */

PDH_FUNCTION PdhLogServiceCommandA(LPCSTR,LPCSTR,DWORD,LPDWORD);  /* Dick */
PDH_FUNCTION PdhLogServiceCommandW(LPCWSTR,LPCWSTR,DWORD,LPDWORD);  /* Dick */
PDH_FUNCTION PdhLogServiceControlA(LPCSTR,LPCSTR,DWORD,PPDH_LOG_SERVICE_QUERY_INFO_A,LPDWORD);  /* Dick */
PDH_FUNCTION PdhLogServiceControlW(LPCWSTR,LPCWSTR,DWORD,PPDH_LOG_SERVICE_QUERY_INFO_W,LPDWORD);  /* Dick */

#ifdef UNICODE
#define PdhOpenQuery  PdhOpenQueryW
#define PdhAddCounter  PdhAddCounterW
#if (_WIN32_WINDOWS >= 0x0600 || _WIN32_WINNT >= 0x0600 || (defined(NTDDI_VERSION) && NTDDI_VERSION >= NTDDI_VISTA))
#define PdhAddEnglishCounter  PdhAddEnglishCounterW
#define PdhValidatePathEx  PdhValidatePathExW
#endif /* _WIN32_WINDOWS >= 0x0600 || ... */
#define PdhGetCounterInfo  PdhGetCounterInfoW
#define PDH_COUNTER_INFO  PDH_COUNTER_INFO_W
#define PPDH_COUNTER_INFO  PPDH_COUNTER_INFO_W
#define PdhConnectMachine  PdhConnectMachineW
#define PdhEnumMachines  PdhEnumMachinesW
#define PdhEnumObjects  PdhEnumObjectsW
#define PdhEnumObjectItems  PdhEnumObjectItemsW
#define PdhMakeCounterPath  PdhMakeCounterPathW
#define PDH_COUNTER_PATH_ELEMENTS  PDH_COUNTER_PATH_ELEMENTS_W
#define PPDH_COUNTER_PATH_ELEMENTS  PPDH_COUNTER_PATH_ELEMENTS_W
#define PdhParseCounterPath  PdhParseCounterPathW
#define PdhParseInstanceName  PdhParseInstanceNameW
#define PdhValidatePath  PdhValidatePathW
#define PdhGetDefaultPerfObject  PdhGetDefaultPerfObjectW
#define PdhGetDefaultPerfCounter  PdhGetDefaultPerfCounterW
#define PdhBrowseCounters  PdhBrowseCountersW
#define PDH_BROWSE_DLG_CONFIG  PDH_BROWSE_DLG_CONFIG_W
#define PPDH_BROWSE_DLG_CONFIG  PPDH_BROWSE_DLG_CONFIG_W
#define PdhExpandCounterPath  PdhExpandCounterPathW
/* v2.0 */
#define PDH_FMT_COUNTERVALUE_ITEM  PDH_FMT_COUNTERVALUE_ITEM_W
#define PPDH_FMT_COUNTERVALUE_ITEM  PPDH_FMT_COUNTERVALUE_ITEM_W
#define PDH_RAW_COUNTER_ITEM  PDH_RAW_COUNTER_ITEM_W
#define PPDH_RAW_COUNTER_ITEM  PPDH_RAW_COUNTER_ITEM_W
#define PdhGetFormattedCounterArray PdhGetFormattedCounterArrayW
#define PdhGetRawCounterArray  PdhGetRawCounterArrayW
#define PdhLookupPerfNameByIndex  PdhLookupPerfNameByIndexW
#define PdhOpenLog  PdhOpenLogW
#define PdhUpdateLog  PdhUpdateLogW
#define PdhSelectDataSource  PdhSelectDataSourceW
#define PdhGetDataSourceTimeRange  PdhGetDataSourceTimeRangeW
#define PDH_LOG_SERVICE_QUERY_INFO  PDH_LOG_SERVICE_QUERY_INFO_W
#define PPDH_LOG_SERVICE_QUERY_INFO PPDH_LOG_SERVICE_QUERY_INFO_W
#define PdhLogServiceControl  PdhLogServiceControlW
#define PdhLogServiceQuery  PdhLogServiceQueryW
#define PdhExpandWildCardPath  PdhExpandWildCardPathW
#if (_WIN32_WINDOWS >= 0x0501 || _WIN32_WINNT >= 0x0501 || (defined(NTDDI_VERSION) && NTDDI_VERSION >= NTDDI_WINXP))
#define PdhBindInputDataSource  PdhBindInputDataSourceW
#define PdhBrowseCountersH  PdhBrowseCountersHW
#define PDH_BROWSE_DLG_CONFIG_H  PDH_BROWSE_DLG_CONFIG_HW
#define PPDH_BROWSE_DLG_CONFIG_H  PPDH_BROWSE_DLG_CONFIG_HW
#define PdhEnumMachinesH  PdhEnumMachinesHW
#define PdhEnumObjectsH  PdhEnumObjectsHW
#define PdhEnumObjectItemsH  PdhEnumObjectItemsHW
#define PdhExpandWildCardPathH  PdhExpandWildCardPathHW
#define PdhGetDefaultPerfObjectH  PdhGetDefaultPerfObjectHW
#define PdhGetDefaultPerfCounterH  PdhGetDefaultPerfCounterHW
#define PdhEnumLogSetNames  PdhEnumLogSetNamesW
#define PdhCreateSQLTables  PdhCreateSQLTablesW
#define PdhVerifySQLDB  PdhVerifySQLDBW
#define PdhLookupPerfIndexByName  PdhLookupPerfIndexByNameW
#endif /* _WIN32_WINDOWS >= 0x0501 || ... */
#else /* !UNICODE */
#define PdhOpenQuery  PdhOpenQueryA
#define PdhAddCounter  PdhAddCounterA
#if (_WIN32_WINDOWS >= 0x0600 || _WINNT_WINNT >= 0x0600 || (defined(NTDDI_VERSION) && NTDDI_VERSION >= NTDDI_VISTA))
#define PdhAddEnglishCounter  PdhAddEnglishCounterA
#define PdhValidatePathEx  PdhValidatePathExA
#endif /* _WIN32_WINDOWS >= 0x0600 || ... */
#define PdhGetCounterInfo  PdhGetCounterInfoA
#define PDH_COUNTER_INFO  PDH_COUNTER_INFO_A
#define PPDH_COUNTER_INFO  PPDH_COUNTER_INFO_A
#define PdhConnectMachine  PdhConnectMachineA
#define PdhEnumMachines  PdhEnumMachinesA
#define PdhEnumObjects  PdhEnumObjectsA
#define PdhEnumObjectItems  PdhEnumObjectItemsA
#define PdhMakeCounterPath  PdhMakeCounterPathA
#define PDH_COUNTER_PATH_ELEMENTS  PDH_COUNTER_PATH_ELEMENTS_A
#define PPDH_COUNTER_PATH_ELEMENTS  PPDH_COUNTER_PATH_ELEMENTS_A
#define PdhParseCounterPath  PdhParseCounterPathA
#define PdhParseInstanceName  PdhParseInstanceNameA
#define PdhValidatePath  PdhValidatePathA
#define PdhGetDefaultPerfObject  PdhGetDefaultPerfObjectA
#define PdhGetDefaultPerfCounter  PdhGetDefaultPerfCounterA
#define PdhBrowseCounters  PdhBrowseCountersA
#define PDH_BROWSE_DLG_CONFIG  PDH_BROWSE_DLG_CONFIG_A
#define PPDH_BROWSE_DLG_CONFIG  PPDH_BROWSE_DLG_CONFIG_A
#define PdhExpandCounterPath  PdhExpandCounterPathA
/* v2.0 */
#define PDH_FMT_COUNTERVALUE_ITEM  PDH_FMT_COUNTERVALUE_ITEM_A
#define PPDH_FMT_COUNTERVALUE_ITEM  PPDH_FMT_COUNTERVALUE_ITEM_A
#define PDH_RAW_COUNTER_ITEM  PDH_RAW_COUNTER_ITEM_A
#define PPDH_RAW_COUNTER_ITEM  PPDH_RAW_COUNTER_ITEM_A
#define PdhGetFormattedCounterArray PdhGetFormattedCounterArrayA
#define PdhGetRawCounterArray  PdhGetRawCounterArrayA
#define PdhLookupPerfNameByIndex  PdhLookupPerfNameByIndexA
#define PdhOpenLog  PdhOpenLogA
#define PdhUpdateLog  PdhUpdateLogA
#define PdhSelectDataSource  PdhSelectDataSourceA
#define PdhGetDataSourceTimeRange  PdhGetDataSourceTimeRangeA
#define PDH_LOG_SERVICE_QUERY_INFO  PDH_LOG_SERVICE_QUERY_INFO_A
#define PPDH_LOG_SERVICE_QUERY_INFO PPDH_LOG_SERVICE_QUERY_INFO_A
#define PdhLogServiceControl  PdhLogServiceControlA
#define PdhLogServiceQuery  PdhLogServiceQueryA
#define PdhExpandWildCardPath  PdhExpandWildCardPathA
#if (_WIN32_WINDOWS >= 0x0501 || _WIN32_WINNT >= 0x0501 || (defined(NTDDI_VERSION) && NTDDI_VERSION >= NTDDI_WINXP))
#define PdhBindInputDataSource  PdhBindInputDataSourceA
#define PDH_BROWSE_DLG_CONFIG_H  PDH_BROWSE_DLG_CONFIG_HA
#define PPDH_BROWSE_DLG_CONFIG_H  PPDH_BROWSE_DLG_CONFIG_HA
#define PdhBrowseCountersH  PdhBrowseCountersHA
#define PdhEnumMachinesH  PdhEnumMachinesHA
#define PdhEnumObjectsH  PdhEnumObjectsHA
#define PdhEnumObjectItemsH  PdhEnumObjectItemsHA
#define PdhExpandWildCardPathH  PdhExpandWildCardPathHA
#define PdhGetDefaultPerfObjectH  PdhGetDefaultPerfObjectHA
#define PdhGetDefaultPerfCounterH  PdhGetDefaultPerfCounterHA
#define PdhEnumLogSetNames  PdhEnumLogSetNamesA
#define PdhCreateSQLTables  PdhCreateSQLTablesA
#define PdhVerifySQLDB  PdhVerifySQLDBA
#define PdhLookupPerfIndexByName  PdhLookupPerfIndexByNameA
#endif /* _WIN32_WINDOWS >= 0x0501 || ... */
#endif /* !UNICODE */


#ifdef __cplusplus
}
#endif

#endif /* _PDH_H */
