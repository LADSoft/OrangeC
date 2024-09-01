#ifndef _EVNTPROV_H
#define _EVNTPROV_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef EVNTAPI
#define EVNTAPI DECLSPEC_IMPORT __stdcall
#endif /* EVNTAPI */

#define EVENT_MIN_LEVEL  (0)
#define EVENT_MAX_LEVEL  (0xff)

#define EVENT_ACTIVITY_CTRL_GET_ID  (1)
#define EVENT_ACTIVITY_CTRL_SET_ID  (2)
#define EVENT_ACTIVITY_CTRL_CREATE_ID  (3)
#define EVENT_ACTIVITY_CTRL_GET_SET_ID  (4)
#define EVENT_ACTIVITY_CTRL_CREATE_SET_ID  (5)

typedef ULONGLONG REGHANDLE, *PREGHANDLE;

#define MAX_EVENT_DATA_DESCRIPTORS  (128)
#define MAX_EVENT_FILTER_DATA_SIZE  (1024)
#define MAX_EVENT_FILTER_PAYLOAD_SIZE  (4096)
#define MAX_EVENT_FILTER_EVENT_NAME_SIZE  (4096)

#define MAX_EVENT_FILTERS_COUNT  (8)

#define MAX_EVENT_FILTER_PID_COUNT  (8)
#define MAX_EVENT_FILTER_EVENT_ID_COUNT  (64)

#define EVENT_FILTER_TYPE_NONE  (0x00000000)
#define EVENT_FILTER_TYPE_SCHEMATIZED  (0x80000000)
#define EVENT_FILTER_TYPE_SYSTEM_FLAGS  (0x80000001)
#define EVENT_FILTER_TYPE_TRACEHANDLE  (0x80000002)
#define EVENT_FILTER_TYPE_PID  (0x80000004)
#define EVENT_FILTER_TYPE_EXECUTABLE_NAME  (0x80000008)
#define EVENT_FILTER_TYPE_PACKAGE_ID  (0x80000010)
#define EVENT_FILTER_TYPE_PACKAGE_APP_ID  (0x80000020)
#define EVENT_FILTER_TYPE_PAYLOAD  (0x80000100)
#define EVENT_FILTER_TYPE_EVENT_ID  (0x80000200)
#define EVENT_FILTER_TYPE_EVENT_NAME  (0x80000400)
#define EVENT_FILTER_TYPE_STACKWALK  (0x80001000)

#define EVENT_DATA_DESCRIPTOR_TYPE_NONE  (0)
#define EVENT_DATA_DESCRIPTOR_TYPE_EVENT_METADATA  (1)
#define EVENT_DATA_DESCRIPTOR_TYPE_PROVIDER_METADATA  (2)
#define EVENT_DATA_DESCRIPTOR_TYPE_TIMESTAMP_OVERRIDE (3)

#define EVENT_WRITE_FLAG_NO_FAULTING 0x00000001

#define EVENT_WRITE_FLAG_INPRIVATE  0x00000002

typedef struct _EVENT_DATA_DESCRIPTOR {

    ULONGLONG Ptr;
    ULONG Size;
    union {
        ULONG Reserved;
        struct {
            UCHAR Type;
            UCHAR Reserved1;
            USHORT Reserved2;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} EVENT_DATA_DESCRIPTOR, *PEVENT_DATA_DESCRIPTOR;

#ifndef EVENT_DESCRIPTOR_DEF
#define EVENT_DESCRIPTOR_DEF
typedef struct _EVENT_DESCRIPTOR {
    USHORT Id;
    UCHAR Version;
    UCHAR Channel;
    UCHAR Level;
    UCHAR Opcode;
    USHORT Task;
    ULONGLONG Keyword;
} EVENT_DESCRIPTOR, *PEVENT_DESCRIPTOR;
typedef const EVENT_DESCRIPTOR *PCEVENT_DESCRIPTOR;
#endif /* EVENT_DESCRIPTOR_DEF */

typedef struct _EVENT_FILTER_DESCRIPTOR {
    ULONGLONG Ptr;
    ULONG Size;
    ULONG Type;
} EVENT_FILTER_DESCRIPTOR, *PEVENT_FILTER_DESCRIPTOR;

typedef struct _EVENT_FILTER_HEADER {
    USHORT Id;
    UCHAR Version;
    UCHAR Reserved[5];
    ULONGLONG InstanceId;
    ULONG Size;
    ULONG NextOffset;
} EVENT_FILTER_HEADER, *PEVENT_FILTER_HEADER;

typedef struct _EVENT_FILTER_EVENT_ID {
    BOOLEAN FilterIn;
    UCHAR Reserved;
    USHORT Count;
    USHORT Events[ANYSIZE_ARRAY];
} EVENT_FILTER_EVENT_ID, *PEVENT_FILTER_EVENT_ID;

typedef struct _EVENT_FILTER_EVENT_NAME {
    ULONGLONG MatchAnyKeyword;
    ULONGLONG MatchAllKeyword;
    UCHAR Level;
    BOOLEAN FilterIn;
    USHORT NameCount;
    UCHAR Names[ANYSIZE_ARRAY];
} EVENT_FILTER_EVENT_NAME, *PEVENT_FILTER_EVENT_NAME;

typedef enum _EVENT_INFO_CLASS {
    EventProviderBinaryTrackInfo,
    EventProviderSetEnableAllKeywords,
    EventProviderSetTraits,
    EventProviderUseDescriptorType,
    MaxEventInfo
} EVENT_INFO_CLASS;

#if !defined(_ETW_KM_)

typedef VOID (NTAPI *PENABLECALLBACK)(LPCGUID, ULONG, UCHAR, ULONGLONG, ULONGLONG, PEVENT_FILTER_DESCRIPTOR, PVOID);

#ifndef _APISET_EVENTING

#if (WINVER >= _WIN32_WINNT_VISTA)
ULONG EVNTAPI EventRegister(LPCGUID, PENABLECALLBACK, PVOID, PREGHANDLE);
ULONG EVNTAPI EventUnregister(REGHANDLE);
#if (WINVER >= _WIN32_WINNT_WIN8)
ULONG EVNTAPI EventSetInformation(REGHANDLE, EVENT_INFO_CLASS, PVOID, ULONG);
#endif /* WINVER >= _WIN32_WINNT_WIN8 */
BOOLEAN EVNTAPI EventEnabled(REGHANDLE, PCEVENT_DESCRIPTOR);
BOOLEAN EVNTAPI EventProviderEnabled(REGHANDLE, UCHAR, ULONGLONG);
ULONG EVNTAPI EventWrite(REGHANDLE, PCEVENT_DESCRIPTOR, ULONG, PEVENT_DATA_DESCRIPTOR);
ULONG EVNTAPI EventWriteTransfer(REGHANDLE, PCEVENT_DESCRIPTOR, LPCGUID, LPCGUID, ULONG, PEVENT_DATA_DESCRIPTOR);
#if (WINVER >= _WIN32_WINNT_WIN7)
ULONG EVNTAPI EventWriteEx(REGHANDLE, PCEVENT_DESCRIPTOR, ULONG64, ULONG, LPCGUID, LPCGUID, ULONG, PEVENT_DATA_DESCRIPTOR);
#endif /* WINVER >= _WIN32_WINNT_WIN7 */
ULONG EVNTAPI EventWriteString(REGHANDLE, UCHAR, ULONGLONG, PCWSTR);
ULONG EVNTAPI EventActivityIdControl(ULONG, LPGUID);
#endif /* WINVER >= _WIN32_WINNT_VISTA */

#endif /* _APISET_EVENTING */

#endif /* _ETW_KM_ */

FORCEINLINE VOID EventDataDescCreate(PEVENT_DATA_DESCRIPTOR EventDataDescriptor, const VOID *DataPtr, ULONG DataSize) {
    EventDataDescriptor->Ptr = (ULONGLONG) (ULONG_PTR)DataPtr;
    EventDataDescriptor->Size = DataSize;
    EventDataDescriptor->Reserved = 0;
}

FORCEINLINE VOID EventDescCreate(PEVENT_DESCRIPTOR EventDescriptor, USHORT Id, UCHAR Version, UCHAR Channel, UCHAR Level, USHORT Task, UCHAR Opcode, ULONGLONG Keyword) {
    EventDescriptor->Id = Id;
    EventDescriptor->Version = Version;
    EventDescriptor->Channel = Channel;
    EventDescriptor->Level = Level;
    EventDescriptor->Task = Task;
    EventDescriptor->Opcode = Opcode;
    EventDescriptor->Keyword = Keyword;
}

FORCEINLINE VOID EventDescZero(PEVENT_DESCRIPTOR EventDescriptor) {
    memset(EventDescriptor, 0, sizeof(EVENT_DESCRIPTOR));
}

FORCEINLINE USHORT EventDescGetId(PCEVENT_DESCRIPTOR EventDescriptor) {
    return (EventDescriptor->Id);
}

FORCEINLINE UCHAR EventDescGetVersion(PCEVENT_DESCRIPTOR EventDescriptor) {
    return (EventDescriptor->Version);
}

FORCEINLINE USHORT EventDescGetTask(PCEVENT_DESCRIPTOR EventDescriptor) {
    return (EventDescriptor->Task);
}

FORCEINLINE UCHAR EventDescGetOpcode(PCEVENT_DESCRIPTOR EventDescriptor) {
    return (EventDescriptor->Opcode);
}

FORCEINLINE UCHAR EventDescGetChannel(PCEVENT_DESCRIPTOR EventDescriptor) {
    return (EventDescriptor->Channel);
}

FORCEINLINE UCHAR EventDescGetLevel(PCEVENT_DESCRIPTOR EventDescriptor) {
    return (EventDescriptor->Level);
}

FORCEINLINE ULONGLONG EventDescGetKeyword(PCEVENT_DESCRIPTOR EventDescriptor) {
    return (EventDescriptor->Keyword);
}

FORCEINLINE PEVENT_DESCRIPTOR EventDescSetId(PEVENT_DESCRIPTOR EventDescriptor, USHORT Id) {
    EventDescriptor->Id = Id;
    return (EventDescriptor);
}

FORCEINLINE PEVENT_DESCRIPTOR EventDescSetVersion(PEVENT_DESCRIPTOR EventDescriptor, UCHAR Version) {
    EventDescriptor->Version = Version;
    return (EventDescriptor);
}

FORCEINLINE PEVENT_DESCRIPTOR EventDescSetTask(PEVENT_DESCRIPTOR EventDescriptor, USHORT Task) {
    EventDescriptor->Task = Task;
    return (EventDescriptor);
}

FORCEINLINE PEVENT_DESCRIPTOR EventDescSetOpcode(PEVENT_DESCRIPTOR EventDescriptor, UCHAR Opcode) {
    EventDescriptor->Opcode = Opcode;
    return (EventDescriptor);
}

FORCEINLINE PEVENT_DESCRIPTOR EventDescSetLevel(PEVENT_DESCRIPTOR EventDescriptor, UCHAR Level) {
    EventDescriptor->Level = Level;
    return (EventDescriptor);
}

FORCEINLINE PEVENT_DESCRIPTOR EventDescSetChannel(PEVENT_DESCRIPTOR EventDescriptor, UCHAR Channel) {
    EventDescriptor->Channel = Channel;
    return (EventDescriptor);
}

FORCEINLINE PEVENT_DESCRIPTOR EventDescSetKeyword(PEVENT_DESCRIPTOR EventDescriptor, ULONGLONG Keyword) {
    EventDescriptor->Keyword = Keyword;
    return (EventDescriptor);
}

FORCEINLINE PEVENT_DESCRIPTOR EventDescOrKeyword(PEVENT_DESCRIPTOR EventDescriptor, ULONGLONG Keyword) {
    EventDescriptor->Keyword |= Keyword;
    return (EventDescriptor);
}

#endif /* _EVNTPROV_H */


