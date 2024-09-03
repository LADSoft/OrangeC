#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef _WTYPESBASE_H
#define _WTYPESBASE_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Forward Declarations */ 

#include "basetsd.h"
#include "guiddef.h"

#ifdef __cplusplus
extern "C"{
#endif 

extern RPC_IF_HANDLE __MIDL_itf_wtypesbase_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wtypesbase_0000_0000_v0_0_s_ifspec;

#ifndef __IWinTypesBase_INTERFACE_DEFINED__
#define __IWinTypesBase_INTERFACE_DEFINED__

#if defined(_WIN32) && !defined(OLE2ANSI)
typedef WCHAR OLECHAR;
typedef OLECHAR *LPOLESTR;
typedef const OLECHAR *LPCOLESTR;
#define OLESTR(str)  L##str
#else
typedef char OLECHAR;
typedef LPSTR LPOLESTR;
typedef LPCSTR LPCOLESTR;
#define OLESTR(str)  str
#endif

#ifndef _WINDEF_H
#ifndef _MINWINDEF_H
typedef void *PVOID;
typedef void *LPVOID;
typedef float FLOAT;
#endif  /* _MINWINDEF_H */
#endif  /* _WINDEF_H */

typedef unsigned char UCHAR;
typedef short SHORT;
typedef unsigned short USHORT;
typedef DWORD ULONG;
typedef double DOUBLE;
#ifndef _DWORDLONG_
typedef unsigned __int64 DWORDLONG;
typedef DWORDLONG *PDWORDLONG;
#endif /* !_DWORDLONG_ */

#ifndef _ULONGLONG_
typedef __int64 LONGLONG;
typedef unsigned __int64 ULONGLONG;
typedef LONGLONG *PLONGLONG;
typedef ULONGLONG *PULONGLONG;
#endif /* !_ULONGLONG_ */

#ifndef _WINBASE_H

#ifndef _FILETIME_
#define _FILETIME_
typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;
#endif /* _FILETIME_ */

#ifndef _SYSTEMTIME_
#define _SYSTEMTIME_
typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;
#endif /* _SYSTEMTIME_ */

#ifndef _SECURITY_ATTRIBUTES_
#define _SECURITY_ATTRIBUTES_
typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;
#endif /* _SECURITY_ATTRIBUTES_ */

#ifndef SECURITY_DESCRIPTOR_REVISION
typedef USHORT SECURITY_DESCRIPTOR_CONTROL;
typedef USHORT *PSECURITY_DESCRIPTOR_CONTROL;
typedef PVOID PSID;

typedef struct _ACL {
    UCHAR AclRevision;
    UCHAR Sbz1;
    USHORT AclSize;
    USHORT AceCount;
    USHORT Sbz2;
} ACL, *PACL;

typedef struct _SECURITY_DESCRIPTOR {
    UCHAR Revision;
    UCHAR Sbz1;
    SECURITY_DESCRIPTOR_CONTROL Control;
    PSID Owner;
    PSID Group;
    PACL Sacl;
    PACL Dacl;
} SECURITY_DESCRIPTOR, *PISECURITY_DESCRIPTOR;
#endif /* SECURITY_DESCRIPTOR_REVISION */

#endif /* _WINBASE_H */

typedef struct _COAUTHIDENTITY {
    USHORT * User;
    ULONG UserLength;
    USHORT *Domain;
    ULONG DomainLength;
    USHORT *Password;
    ULONG PasswordLength;
    ULONG Flags;
} COAUTHIDENTITY;

typedef struct _COAUTHINFO {
    DWORD dwAuthnSvc;
    DWORD dwAuthzSvc;
    LPWSTR pwszServerPrincName;
    DWORD dwAuthnLevel;
    DWORD dwImpersonationLevel;
    COAUTHIDENTITY *pAuthIdentityData;
    DWORD dwCapabilities;
} COAUTHINFO;

typedef LONG SCODE;
typedef SCODE *PSCODE;

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef LONG HRESULT;
#endif /* _HRESULT_DEFINED */

#ifndef __OBJECTID_DEFINED
#define __OBJECTID_DEFINED
#define _OBJECTID_DEFINED
typedef struct _OBJECTID {
    GUID Lineage;
    ULONG Uniquifier;
} OBJECTID;
#endif /* _OBJECTID_DEFINED */

typedef enum tagMEMCTX {
    MEMCTX_TASK = 1,
    MEMCTX_SHARED = 2,
    MEMCTX_MACSYSTEM = 3,
    MEMCTX_UNKNOWN = -1,
    MEMCTX_SAME = -2
} MEMCTX;

#ifndef _ROTREGFLAGS_DEFINED
#define _ROTREGFLAGS_DEFINED
#define ROTREGFLAGS_ALLOWANYCLIENT  0x1
#endif /* _ROTREGFLAGS_DEFINED */
#ifndef _APPIDREGFLAGS_DEFINED
#define _APPIDREGFLAGS_DEFINED
#define APPIDREGFLAGS_ACTIVATE_IUSERVER_INDESKTOP  0x1
#define APPIDREGFLAGS_SECURE_SERVER_PROCESS_SD_AND_BIND  0x2
#define APPIDREGFLAGS_ISSUE_ACTIVATION_RPC_AT_IDENTIFY  0x4
#define APPIDREGFLAGS_IUSERVER_UNMODIFIED_LOGON_TOKEN  0x8
#define APPIDREGFLAGS_IUSERVER_SELF_SID_IN_LAUNCH_PERMISSION  0x10
#define APPIDREGFLAGS_IUSERVER_ACTIVATE_IN_CLIENT_SESSION_ONLY  0x20
#define APPIDREGFLAGS_RESERVED1  0x40
#define APPIDREGFLAGS_RESERVED2  0x80
#define APPIDREGFLAGS_RESERVED3  0x100
#define APPIDREGFLAGS_RESERVED4  0x200
#define APPIDREGFLAGS_RESERVED5  0x400
#define APPIDREGFLAGS_RESERVED6  0x800
#endif /* _APPIDREGFLAGS_DEFINED */
#ifndef _DCOMSCM_REMOTECALL_FLAGS_DEFINED
#define _DCOMSCM_REMOTECALL_FLAGS_DEFINED
#define DCOMSCM_ACTIVATION_USE_ALL_AUTHNSERVICES  0x1
#define DCOMSCM_ACTIVATION_DISALLOW_UNSECURE_CALL  0x2
#define DCOMSCM_RESOLVE_USE_ALL_AUTHNSERVICES  0x4
#define DCOMSCM_RESOLVE_DISALLOW_UNSECURE_CALL  0x8
#define DCOMSCM_PING_USE_MID_AUTHNSERVICE  0x10
#define DCOMSCM_PING_DISALLOW_UNSECURE_CALL  0x20
#endif /* _DCOMSCM_REMOTECALL_FLAGS_DEFINED */

typedef enum tagCLSCTX {
    CLSCTX_INPROC_SERVER = 0x1,
    CLSCTX_INPROC_HANDLER = 0x2,
    CLSCTX_LOCAL_SERVER = 0x4,
    CLSCTX_INPROC_SERVER16 = 0x8,
    CLSCTX_REMOTE_SERVER = 0x10,
    CLSCTX_INPROC_HANDLER16 = 0x20,
    CLSCTX_RESERVED1 = 0x40,
    CLSCTX_RESERVED2 = 0x80,
    CLSCTX_RESERVED3 = 0x100,
    CLSCTX_RESERVED4 = 0x200,
    CLSCTX_NO_CODE_DOWNLOAD = 0x400,
    CLSCTX_RESERVED5 = 0x800,
    CLSCTX_NO_CUSTOM_MARSHAL = 0x1000,
    CLSCTX_ENABLE_CODE_DOWNLOAD = 0x2000,
    CLSCTX_NO_FAILURE_LOG = 0x4000,
    CLSCTX_DISABLE_AAA = 0x8000,
    CLSCTX_ENABLE_AAA = 0x10000,
    CLSCTX_FROM_DEFAULT_CONTEXT = 0x20000,
    CLSCTX_ACTIVATE_32_BIT_SERVER = 0x40000,
    CLSCTX_ACTIVATE_64_BIT_SERVER = 0x80000,
    CLSCTX_ENABLE_CLOAKING = 0x100000,
    CLSCTX_APPCONTAINER = 0x400000,
    CLSCTX_ACTIVATE_AAA_AS_IU = 0x800000,
    CLSCTX_PS_DLL = (int)0x80000000
} CLSCTX;

#define CLSCTX_VALID_MASK \
   (CLSCTX_INPROC_SERVER | \
    CLSCTX_INPROC_HANDLER | \
    CLSCTX_LOCAL_SERVER | \
    CLSCTX_INPROC_SERVER16 | \
    CLSCTX_REMOTE_SERVER | \
    CLSCTX_NO_CODE_DOWNLOAD | \
    CLSCTX_NO_CUSTOM_MARSHAL | \
    CLSCTX_ENABLE_CODE_DOWNLOAD | \
    CLSCTX_NO_FAILURE_LOG | \
    CLSCTX_DISABLE_AAA | \
    CLSCTX_ENABLE_AAA | \
    CLSCTX_FROM_DEFAULT_CONTEXT | \
    CLSCTX_ACTIVATE_32_BIT_SERVER | \
    CLSCTX_ACTIVATE_64_BIT_SERVER | \
    CLSCTX_ENABLE_CLOAKING | \
    CLSCTX_APPCONTAINER | \
    CLSCTX_ACTIVATE_AAA_AS_IU | \
    CLSCTX_PS_DLL)

typedef enum tagMSHLFLAGS {
    MSHLFLAGS_NORMAL = 0,
    MSHLFLAGS_TABLESTRONG = 1,
    MSHLFLAGS_TABLEWEAK = 2,
    MSHLFLAGS_NOPING = 4,
    MSHLFLAGS_RESERVED1 = 8,
    MSHLFLAGS_RESERVED2 = 16,
    MSHLFLAGS_RESERVED3 = 32,
    MSHLFLAGS_RESERVED4 = 64
} MSHLFLAGS;

typedef enum tagMSHCTX {
    MSHCTX_LOCAL = 0,
    MSHCTX_NOSHAREDMEM = 1,
    MSHCTX_DIFFERENTMACHINE = 2,
    MSHCTX_INPROC = 3,
    MSHCTX_CROSSCTX = 4
} MSHCTX;

typedef struct _BYTE_BLOB {
    ULONG clSize;
    byte abData[1];
} BYTE_BLOB, *UP_BYTE_BLOB;

typedef struct _WORD_BLOB {
    ULONG clSize;
    unsigned short asData[1];
} WORD_BLOB, *UP_WORD_BLOB;

typedef struct _DWORD_BLOB {
    ULONG clSize;
    ULONG alData[1];
} DWORD_BLOB, *UP_DWORD_BLOB;

typedef struct _FLAGGED_BYTE_BLOB {
    ULONG fFlags;
    ULONG clSize;
    byte abData[1];
} FLAGGED_BYTE_BLOB, *UP_FLAGGED_BYTE_BLOB;

typedef struct _FLAGGED_WORD_BLOB {
    ULONG fFlags;
    ULONG clSize;
    unsigned short asData[1];
} FLAGGED_WORD_BLOB, *UP_FLAGGED_WORD_BLOB;

typedef struct _BYTE_SIZEDARR {
    ULONG clSize;
    byte *pData;
} BYTE_SIZEDARR;

typedef struct _SHORT_SIZEDARR {
    ULONG clSize;
    unsigned short *pData;
} WORD_SIZEDARR;

typedef struct _LONG_SIZEDARR {
    ULONG clSize;
    ULONG *pData;
} DWORD_SIZEDARR;

typedef struct _HYPER_SIZEDARR {
    ULONG clSize;
    hyper *pData;
} HYPER_SIZEDARR;

extern RPC_IF_HANDLE IWinTypesBase_v0_1_c_ifspec;
extern RPC_IF_HANDLE IWinTypesBase_v0_1_s_ifspec;
#endif /* __IWinTypesBase_INTERFACE_DEFINED__ */

typedef boolean BOOLEAN;

#ifndef _tagBLOB_DEFINED
#define _tagBLOB_DEFINED
#define _BLOB_DEFINED
#define _LPBLOB_DEFINED
typedef struct tagBLOB {
    ULONG cbSize;
    BYTE *pBlobData;
} BLOB, *LPBLOB;
#endif

#ifndef SID_IDENTIFIER_AUTHORITY_DEFINED
#define SID_IDENTIFIER_AUTHORITY_DEFINED
typedef struct _SID_IDENTIFIER_AUTHORITY {
    UCHAR Value[6];
} SID_IDENTIFIER_AUTHORITY, *PSID_IDENTIFIER_AUTHORITY;
#endif

#ifndef SID_DEFINED
#define SID_DEFINED
typedef struct _SID {
    BYTE Revision;
    BYTE SubAuthorityCount;
    SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
    ULONG SubAuthority[1];
} SID, *PISID;

typedef struct _SID_AND_ATTRIBUTES {
    SID *Sid;
    DWORD Attributes;
} SID_AND_ATTRIBUTES, *PSID_AND_ATTRIBUTES;
#endif

extern RPC_IF_HANDLE __MIDL_itf_wtypesbase_0000_0001_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wtypesbase_0000_0001_v0_0_s_ifspec;

#ifdef __cplusplus
}
#endif

#endif /* _WTYPESBASE_H */
