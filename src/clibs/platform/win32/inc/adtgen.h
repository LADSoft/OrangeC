#ifndef _ADTGEN_H
#define _ADTGEN_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Generic audits generating definitions */


#define AUDIT_TYPE_LEGACY  1
#define AUDIT_TYPE_WMI  2

typedef enum _AUDIT_PARAM_TYPE {
    APT_None = 1,
    APT_String,
    APT_Ulong,
    APT_Pointer,
    APT_Sid,
    APT_LogonId,
    APT_ObjectTypeList,
} AUDIT_PARAM_TYPE;

#define AP_ParamTypeBits  8
#define AP_ParamTypeMask  0x000000ffL

#define AP_FormatHex  (0x0001L<<AP_ParamTypeBits)
#define AP_AccessMask  (0x0002L<<AP_ParamTypeBits)
#define AP_Filespec  (0x0001L<<AP_ParamTypeBits)
#define AP_PrimaryLogonId  (0x0001L<<AP_ParamTypeBits)
#define AP_ClientLogonId  (0x0002L<<AP_ParamTypeBits)

#define ApExtractType(TypeFlags)  ((AUDIT_PARAM_TYPE)(TypeFlags & AP_ParamTypeMask))
#define ApExtractFlags(TypeFlags)  ((TypeFlags & ~AP_ParamTypeMask))

#define APF_AuditFailure  0x00000000
#define APF_AuditSuccess  0x00000001

#define APF_ValidFlags  (APF_AuditSuccess)

typedef struct _AUDIT_OBJECT_TYPE {
    GUID ObjectType;
    USHORT Flags;
    USHORT Level;
    ACCESS_MASK AccessMask;
} AUDIT_OBJECT_TYPE, *PAUDIT_OBJECT_TYPE;

typedef struct _AUDIT_OBJECT_TYPES {
    USHORT Count;
    USHORT Flags;
    AUDIT_OBJECT_TYPE *pObjectTypes;
} AUDIT_OBJECT_TYPES, *PAUDIT_OBJECT_TYPES;

typedef struct _AUDIT_PARAM {
    AUDIT_PARAM_TYPE Type;
    ULONG Length;
    DWORD Flags;
    union {
        ULONG_PTR Data0;
        PWSTR String;
        ULONG_PTR u;
        SID *psid;
        ULONG LogonId_LowPart;
        AUDIT_OBJECT_TYPES *pObjectTypes;
    };
    union {
        ULONG_PTR Data1;
        LONG LogonId_HighPart;
    };
} AUDIT_PARAM, *PAUDIT_PARAM;

typedef struct _AUDIT_PARAMS {
    ULONG Length;
    DWORD Flags;
    USHORT Count;
    AUDIT_PARAM *Parameters;
} AUDIT_PARAMS, *PAUDIT_PARAMS;

typedef struct _AUTHZ_AUDIT_EVENT_TYPE_LEGACY {
    USHORT CategoryId;
    USHORT AuditId;
    USHORT ParameterCount;
} AUTHZ_AUDIT_EVENT_TYPE_LEGACY, *PAUTHZ_AUDIT_EVENT_TYPE_LEGACY;

typedef union _AUTHZ_AUDIT_EVENT_TYPE_UNION {
    AUTHZ_AUDIT_EVENT_TYPE_LEGACY Legacy;
} AUTHZ_AUDIT_EVENT_TYPE_UNION, *PAUTHZ_AUDIT_EVENT_TYPE_UNION;

typedef struct _AUTHZ_AUDIT_EVENT_TYPE_OLD {
    ULONG Version;
    DWORD dwFlags;
    LONG RefCount;
    ULONG_PTR hAudit;
    LUID LinkId;
    AUTHZ_AUDIT_EVENT_TYPE_UNION u;
} AUTHZ_AUDIT_EVENT_TYPE_OLD, *PAUTHZ_AUDIT_EVENT_TYPE_OLD;

typedef PVOID AUDIT_HANDLE, *PAUDIT_HANDLE;

BOOL AuthzpRegisterAuditEvent(PAUTHZ_AUDIT_EVENT_TYPE_OLD,PAUDIT_HANDLE);
BOOL AuthzpUnregisterAuditEvent(AUDIT_HANDLE*);


#endif /* _ADTGEN_H */
