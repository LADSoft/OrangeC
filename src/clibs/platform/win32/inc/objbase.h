#include <rpc.h>
#include <rpcndr.h>

#ifndef _OBJBASE_H
#define _OBJBASE_H

/* Windows Component Object Model defintions */

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#include <pshpack8.h>

#include <combaseapi.h>
#include <coml2api.h>

typedef enum tagCOINIT {
    COINIT_APARTMENTTHREADED = 0x2,
#if (_WIN32_WINNT >= 0x0400) || defined(_WIN32_DCOM)
    COINIT_MULTITHREADED = COINITBASE_MULTITHREADED,
    COINIT_DISABLE_OLE1DDE = 0x4,
    COINIT_SPEED_OVER_MEMORY = 0x8,
#endif /* _WIN32_WINNT >= 0x0400 || _WIN32_DCOM */
} COINIT;

#define MARSHALINTERFACE_MIN  500

#define ASYNC_MODE_COMPATIBILITY  0x00000001L
#define ASYNC_MODE_DEFAULT        0x00000000L

#define STGTY_REPEAT              0x00000100L
#define STG_TOEND                 0xFFFFFFFFL

#define STG_LAYOUT_SEQUENTIAL     0x00000000L
#define STG_LAYOUT_INTERLEAVED    0x00000001L

#include <objidl.h>

#ifdef _OLE32_
#ifdef _OLE32PRIV_
BOOL _fastcall wIsEqualGUID(REFGUID, REFGUID);
#define IsEqualGUID(rguid1,rguid2)  wIsEqualGUID(rguid1, rguid2)
#else
#define __INLINE_ISEQUAL_GUID
#endif  // _OLE32PRIV_
#endif  // _OLE32_


WINOLEAPI_(DWORD) CoBuildVersion(VOID);

WINOLEAPI CoInitialize(LPVOID);

#define COM_SUPPORT_MALLOC_SPIES
WINOLEAPI CoRegisterMallocSpy(LPMALLOCSPY);
WINOLEAPI CoRevokeMallocSpy(void);
WINOLEAPI CoCreateStandardMalloc(DWORD, IMalloc **);

#if (_WIN32_WINNT >= 0x0501)

WINOLEAPI CoRegisterInitializeSpy(IInitializeSpy *, ULARGE_INTEGER *);
WINOLEAPI CoRevokeInitializeSpy(ULARGE_INTEGER);

typedef enum tagCOMSD {
    SD_LAUNCHPERMISSIONS = 0,
    SD_ACCESSPERMISSIONS = 1,
    SD_LAUNCHRESTRICTIONS = 2,
    SD_ACCESSRESTRICTIONS = 3
} COMSD;

WINOLEAPI CoGetSystemSecurityPermissions(COMSD comSDType, PSECURITY_DESCRIPTOR *ppSD);

#endif /* _WIN32_WINNT >= 0x0501 */

WINOLEAPI_(HINSTANCE) CoLoadLibrary(LPOLESTR, BOOL);
WINOLEAPI_(void) CoFreeLibrary(HINSTANCE);
WINOLEAPI_(void) CoFreeAllLibraries(void);

#if (_WIN32_WINNT >= 0x0400) || defined(_WIN32_DCOM)
WINOLEAPI CoGetInstanceFromFile(COSERVERINFO *, CLSID *, IUnknown *, DWORD, DWORD, OLECHAR *, DWORD, MULTI_QI *);
WINOLEAPI CoGetInstanceFromIStorage(COSERVERINFO *, CLSID *, IUnknown *, DWORD, struct IStorage *, DWORD, MULTI_QI *);
#endif /* _WIN32_WINNT >= 0x0400 || _WIN32_DCOM */

#if (_WIN32_WINNT >= 0x0500) || defined(_WIN32_DCOM)
WINOLEAPI CoAllowSetForegroundWindow(IUnknown *, LPVOID);
WINOLEAPI DcomChannelSetHResult(LPVOID, ULONG *, HRESULT);
#endif /* _WIN32_WINNT >= 0x0500 || _WIN32_DCOM */

WINOLEAPI_(BOOL) CoIsOle1Class(REFCLSID);
WINOLEAPI CLSIDFromProgIDEx(LPCOLESTR, LPCLSID);

WINOLEAPI_(BOOL) CoFileTimeToDosDateTime(FILETIME *, LPWORD, LPWORD);
WINOLEAPI_(BOOL) CoDosDateTimeToFileTime(WORD, WORD, FILETIME *);
WINOLEAPI CoFileTimeNow(FILETIME *);

WINOLEAPI CoRegisterMessageFilter(LPMESSAGEFILTER, LPMESSAGEFILTER *);

#if (_WIN32_WINNT >= 0x0400) || defined(_WIN32_DCOM)
WINOLEAPI CoRegisterChannelHook(REFGUID, IChannelHook *);
#endif /* _WIN32_WINNT >= 0x0400 || _WIN32_DCOM */

WINOLEAPI CoTreatAsClass(REFCLSID, REFCLSID);

WINOLEAPI CreateDataAdviseHolder(LPDATAADVISEHOLDER *);
WINOLEAPI CreateDataCache(LPUNKNOWN, REFCLSID, REFIID, LPVOID *);

WINOLEAPI StgOpenAsyncDocfileOnIFillLockBytes(IFillLockBytes *, DWORD, DWORD, IStorage **);
WINOLEAPI StgGetIFillLockBytesOnILockBytes(ILockBytes *, IFillLockBytes **);
WINOLEAPI StgGetIFillLockBytesOnFile(OLECHAR const *, IFillLockBytes **);
WINOLEAPI StgOpenLayoutDocfile(OLECHAR const *, DWORD, DWORD, IStorage **);

WINOLEAPI CoInstall(IBindCtx *, DWORD, uCLSSPEC *, QUERYCONTEXT *, LPWSTR);

WINOLEAPI BindMoniker(LPMONIKER, DWORD, REFIID, LPVOID *);
WINOLEAPI CoGetObject(LPCWSTR, BIND_OPTS *, REFIID, void **);
WINOLEAPI MkParseDisplayName(LPBC, LPCOLESTR, ULONG *, LPMONIKER *);
WINOLEAPI MonikerRelativePathTo(LPMONIKER, LPMONIKER, LPMONIKER *, BOOL);
WINOLEAPI MonikerCommonPrefixWith(LPMONIKER, LPMONIKER, LPMONIKER *);
WINOLEAPI CreateBindCtx(DWORD, LPBC *);
WINOLEAPI CreateGenericComposite(LPMONIKER, LPMONIKER,LPMONIKER *);
WINOLEAPI GetClassFile(LPCOLESTR, CLSID *);

WINOLEAPI CreateClassMoniker(REFCLSID, LPMONIKER *);
WINOLEAPI CreateFileMoniker(LPCOLESTR, LPMONIKER *);
WINOLEAPI CreateItemMoniker(LPCOLESTR, LPCOLESTR, LPMONIKER *);
WINOLEAPI CreateAntiMoniker(LPMONIKER *);
WINOLEAPI CreatePointerMoniker(LPUNKNOWN, LPMONIKER *);
WINOLEAPI CreateObjrefMoniker(LPUNKNOWN, LPMONIKER *);

WINOLEAPI GetRunningObjectTable(DWORD, LPRUNNINGOBJECTTABLE *);

#include <urlmon.h>
#include <propidl.h>

WINOLEAPI CreateStdProgressIndicator(HWND, LPCOLESTR, IBindStatusCallback *, IBindStatusCallback **);

#include <poppack.h>

#endif /* _OBJBASE_H */
