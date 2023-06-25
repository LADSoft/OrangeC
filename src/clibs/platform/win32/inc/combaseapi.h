/* #include <apiset.h> */
#include <apisetcconv.h>

/* ApiSet api-ms-win-core-com-l1 */

#include <rpc.h>
#include <rpcndr.h>

#if (NTDDI_VERSION >= NTDDI_VISTA && !defined(_WIN32_WINNT))
#define _WIN32_WINNT 0x0600
#endif

#if (NTDDI_VERSION >= NTDDI_WS03 && !defined(_WIN32_WINNT))
#define _WIN32_WINNT 0x0502
#endif

#if (NTDDI_VERSION >= NTDDI_WINXP && !defined(_WIN32_WINNT))
#define _WIN32_WINNT 0x0501
#endif

#if (NTDDI_VERSION >= NTDDI_WIN2K && !defined(_WIN32_WINNT))
#define _WIN32_WINNT 0x0500
#endif

#ifndef _COMBASEAPI_H
#define _COMBASEAPI_H

#ifdef __ORANGEC__ 
#pragma once
#endif

/* Windows Base Component Object Model defintions */

#include <pshpack8.h>

//TODO change _OLE32_ to _COMBASEAPI_
#ifdef _OLE32_
#define WINOLEAPI  STDAPI
#define WINOLEAPI_(type)  STDAPI_(type)
#else /* !_OLE32_ */
#define WINOLEAPI  EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define WINOLEAPI_(type)  EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#endif /* !_OLE32_ */

#define interface  struct

#if defined(__cplusplus) && !defined(CINTERFACE)
#define interface struct
#define STDMETHOD(method)  virtual HRESULT STDMETHODCALLTYPE method
#define STDMETHOD_(type,method)  virtual type STDMETHODCALLTYPE method
#define STDMETHODV(method)  virtual HRESULT STDMETHODVCALLTYPE method
#define STDMETHODV_(type,method)  virtual type STDMETHODVCALLTYPE method
#define PURE  = 0
#define THIS_
#define THIS  void
#define DECLARE_INTERFACE(iface)  interface iface
#define DECLARE_INTERFACE_(iface,baseiface)  interface iface : public baseiface
#else /* defined(__cplusplus) && !defined(CINTERFACE) */
#define interface struct
#define STDMETHOD(method)  HRESULT (STDMETHODCALLTYPE *method)
#define STDMETHOD_(type,method)  type (STDMETHODCALLTYPE *method)
#define STDMETHODV(method)  HRESULT (STDMETHODVCALLTYPE *method)
#define STDMETHODV_(type,method)  type (STDMETHODVCALLTYPE *method)
#define PURE
#define THIS_ INTERFACE *This,
#define THIS INTERFACE *This
#ifdef CONST_VTABLE
#undef CONST_VTBL
#define CONST_VTBL const
#define DECLARE_INTERFACE(iface)  typedef interface iface { const struct iface##Vtbl *lpVtbl; } iface; typedef const struct iface##Vtbl iface##Vtbl; const struct iface##Vtbl
#else
#undef CONST_VTBL
#define CONST_VTBL
#define DECLARE_INTERFACE(iface)  typedef interface iface { struct iface##Vtbl *lpVtbl; } iface; typedef struct iface##Vtbl iface##Vtbl; struct iface##Vtbl
#endif /* CONST_VTABLE */
#define DECLARE_INTERFACE_(iface,baseiface)  DECLARE_INTERFACE(iface)
#endif /* defined(__cplusplus) && !defined(CINTERFACE) */

#define IFACEMETHOD(method)  STDMETHOD(method)
#define IFACEMETHOD_(type,method)  STDMETHOD_(type,method)
#define IFACEMETHODV(method)  STDMETHODV(method)
#define IFACEMETHODV_(type,method)  STDMETHODV_(type,method)

#if !defined(BEGIN_INTERFACE)
#define BEGIN_INTERFACE
#define END_INTERFACE
#endif /* BEGIN_INTERFACE */

#define DECLARE_INTERFACE_IID(iface,iid)  DECLARE_INTERFACE(iface)
#define DECLARE_INTERFACE_IID_(iface,baseiface,iid)  DECLARE_INTERFACE_(iface, baseiface)

#include <stdlib.h>

#define LISet32(li,v)  ((li).HighPart = ((LONG) (v)) < 0 ? -1 : 0, (li).LowPart = (v))

#define ULISet32(li,v)  ((li).HighPart = 0, (li).LowPart = (v))

#define CLSCTX_INPROC  (CLSCTX_INPROC_SERVER|CLSCTX_INPROC_HANDLER)

#if (_WIN32_WINNT >= 0x0400) || defined(_WIN32_DCOM)
#define CLSCTX_ALL  (CLSCTX_INPROC_SERVER|CLSCTX_INPROC_HANDLER|CLSCTX_LOCAL_SERVER|CLSCTX_REMOTE_SERVER)
#define CLSCTX_SERVER  (CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER|CLSCTX_REMOTE_SERVER)
#else
#define CLSCTX_ALL  (CLSCTX_INPROC_SERVER|CLSCTX_INPROC_HANDLER|CLSCTX_LOCAL_SERVER)
#define CLSCTX_SERVER  (CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER)
#endif

typedef enum tagREGCLS {
    REGCLS_SINGLEUSE = 0,
    REGCLS_MULTIPLEUSE = 1,
    REGCLS_MULTI_SEPARATE = 2,
    REGCLS_SUSPENDED = 4,
    REGCLS_SURROGATE = 8,
#if (NTDDI_VERSION >= NTDDI_WINTHRESHOLD)
    REGCLS_AGILE = 0x10,
#endif /* NTDDI_VERSION >= NTDDI_WINTHRESHOLD */
} REGCLS;

typedef interface IRpcStubBuffer IRpcStubBuffer;
typedef interface IRpcChannelBuffer IRpcChannelBuffer;

typedef enum tagCOINITBASE {
#if (_WIN32_WINNT >= 0x0400) || defined(_WIN32_DCOM)
    COINITBASE_MULTITHREADED = 0x0,
#endif /* _WIN32_WINNT >= 0x0400 || _WIN32_DCOM */
} COINITBASE;

#include <wtypesbase.h>
#include <unknwnbase.h>

#include <objidlbase.h>
#include <guiddef.h>

#ifndef INITGUID
#include <cguid.h>
#endif

WINOLEAPI CoGetMalloc(DWORD, LPMALLOC *);

WINOLEAPI CreateStreamOnHGlobal(HGLOBAL, BOOL, LPSTREAM *);

WINOLEAPI GetHGlobalFromStream(LPSTREAM, HGLOBAL *);

WINOLEAPI_(void) CoUninitialize(void);

WINOLEAPI_(DWORD) CoGetCurrentProcess(void);

#if (_WIN32_WINNT >= 0x0400) || defined(_WIN32_DCOM)
WINOLEAPI CoInitializeEx(LPVOID, DWORD);
WINOLEAPI CoGetCallerTID(LPDWORD);
WINOLEAPI CoGetCurrentLogicalThreadId(GUID *);
#endif /* _WIN32_WINNT >= 0x0400 || _WIN32_DCOM */

#if (_WIN32_WINNT >= 0x0501)
WINOLEAPI CoGetContextToken(ULONG_PTR *);
WINOLEAPI CoGetDefaultContext(APTTYPE, REFIID, void **);
#endif /* _WIN32_WINNT >= 0x0501 */

#if (NTDDI_VERSION >= NTDDI_WIN7)
WINOLEAPI CoGetApartmentType(APTTYPE *, APTTYPEQUALIFIER *);
#endif /* NTDDI_VERSION >= NTDDI_WIN7 */

#if (NTDDI_VERSION >= NTDDI_WIN8)

typedef struct tagServerInformation {
    DWORD dwServerPid;
    DWORD dwServerTid;
    UINT64 ui64ServerAddress;
} ServerInformation, *PServerInformation;

WINOLEAPI CoDecodeProxy(DWORD, UINT64, PServerInformation);

DECLARE_HANDLE(CO_MTA_USAGE_COOKIE);

WINOLEAPI CoIncrementMTAUsage(CO_MTA_USAGE_COOKIE *);
WINOLEAPI CoDecrementMTAUsage(CO_MTA_USAGE_COOKIE);
WINOLEAPI CoAllowUnmarshalerCLSID(REFCLSID);

#endif /* NTDDI_VERSION >= NTDDI_WIN8 */

WINOLEAPI CoGetObjectContext(REFIID, LPVOID *);
WINOLEAPI CoGetClassObject(REFCLSID, DWORD, LPVOID, REFIID, LPVOID *);
WINOLEAPI CoRegisterClassObject(REFCLSID, LPUNKNOWN, DWORD, DWORD, LPDWORD);
WINOLEAPI CoRevokeClassObject(DWORD);
WINOLEAPI CoResumeClassObjects(void);
WINOLEAPI CoSuspendClassObjects(void);
WINOLEAPI_(ULONG) CoAddRefServerProcess(void);
WINOLEAPI_(ULONG) CoReleaseServerProcess(void);
WINOLEAPI CoGetPSClsid(REFIID, CLSID *);
WINOLEAPI CoRegisterPSClsid(REFIID, REFCLSID);
WINOLEAPI CoRegisterSurrogate(LPSURROGATE);
WINOLEAPI CoGetMarshalSizeMax(ULONG *, REFIID, LPUNKNOWN, DWORD, LPVOID, DWORD);
WINOLEAPI CoMarshalInterface(LPSTREAM, REFIID, LPUNKNOWN, DWORD, LPVOID, DWORD);
WINOLEAPI CoUnmarshalInterface(LPSTREAM, REFIID, LPVOID *);
WINOLEAPI CoMarshalHresult(LPSTREAM, HRESULT);
WINOLEAPI CoUnmarshalHresult(LPSTREAM, HRESULT *);
WINOLEAPI CoReleaseMarshalData(LPSTREAM);
WINOLEAPI CoDisconnectObject(LPUNKNOWN, DWORD);
WINOLEAPI CoLockObjectExternal(LPUNKNOWN, BOOL, BOOL);
WINOLEAPI CoGetStandardMarshal(REFIID, LPUNKNOWN, DWORD, LPVOID, DWORD, LPMARSHAL *);
WINOLEAPI CoGetStdMarshalEx(LPUNKNOWN, DWORD, LPUNKNOWN *);

typedef enum tagSTDMSHLFLAGS {
    SMEXF_SERVER = 0x01,
    SMEXF_HANDLER = 0x02
} STDMSHLFLAGS;

WINOLEAPI_(BOOL) CoIsHandlerConnected(LPUNKNOWN);
WINOLEAPI CoMarshalInterThreadInterfaceInStream(REFIID, LPUNKNOWN, LPSTREAM *);
WINOLEAPI CoGetInterfaceAndReleaseStream(LPSTREAM, REFIID, LPVOID *);
WINOLEAPI CoCreateFreeThreadedMarshaler(LPUNKNOWN, LPUNKNOWN *);
WINOLEAPI_(void) CoFreeUnusedLibraries(void);

#if (_WIN32_WINNT >= 0x0501)
WINOLEAPI_(void) CoFreeUnusedLibrariesEx(DWORD, DWORD);
#endif/* _WIN32_WINNT >= 0x0501 */

#if (_WIN32_WINNT >= 0x0600)
WINOLEAPI CoDisconnectContext(DWORD);
#endif /* _WIN32_WINNT >= 0x0600 */

#if (_WIN32_WINNT >= 0x0400) || defined(_WIN32_DCOM)
WINOLEAPI CoInitializeSecurity(PSECURITY_DESCRIPTOR, LONG, SOLE_AUTHENTICATION_SERVICE *, void *, DWORD, DWORD, void *, DWORD, void *);
WINOLEAPI CoGetCallContext(REFIID, void **);
WINOLEAPI CoQueryProxyBlanket(IUnknown *, DWORD *, DWORD *, LPOLESTR *, DWORD *, DWORD *, RPC_AUTH_IDENTITY_HANDLE *, DWORD *);
WINOLEAPI CoSetProxyBlanket(IUnknown *, DWORD, DWORD, OLECHAR *, DWORD, DWORD, RPC_AUTH_IDENTITY_HANDLE, DWORD);
WINOLEAPI CoCopyProxy(IUnknown *, IUnknown **);
WINOLEAPI CoQueryClientBlanket(DWORD *, DWORD *, LPOLESTR *, DWORD *, DWORD *, RPC_AUTHZ_HANDLE *, DWORD *);
WINOLEAPI CoImpersonateClient(void);
WINOLEAPI CoRevertToSelf(void);
WINOLEAPI CoQueryAuthenticationServices(DWORD *, SOLE_AUTHENTICATION_SERVICE **);
WINOLEAPI CoSwitchCallContext(IUnknown *, IUnknown **);

#define COM_RIGHTS_EXECUTE  1
#define COM_RIGHTS_EXECUTE_LOCAL  2
#define COM_RIGHTS_EXECUTE_REMOTE  4
#define COM_RIGHTS_ACTIVATE_LOCAL  8
#define COM_RIGHTS_ACTIVATE_REMOTE  16
#endif /* _WIN32_WINNT >= 0x0400 || defined(_WIN32_DCOM */

WINOLEAPI CoCreateInstance(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID *);

#if (_WIN32_WINNT >= 0x0400) || defined(_WIN32_DCOM)
WINOLEAPI CoCreateInstanceEx(REFCLSID, IUnknown *, DWORD, COSERVERINFO *, DWORD, MULTI_QI *);
#endif /* _WIN32_WINNT >= 0x0400 || defined(_WIN32_DCOM */

WINOLEAPI CoRegisterActivationFilter(IActivationFilter *);

#if (_WIN32_WINNT >= 0x0602)
WINOLEAPI CoCreateInstanceFromApp(REFCLSID, IUnknown *, DWORD, PVOID, DWORD, MULTI_QI *);
#endif /* _WIN32_WINNT >= 0x0602 */

#if (_WIN32_WINNT >= 0x0500) || defined(_WIN32_DCOM)
WINOLEAPI CoGetCancelObject(DWORD, REFIID, void **);
WINOLEAPI CoSetCancelObject(IUnknown *);
WINOLEAPI CoCancelCall(DWORD, ULONG);
WINOLEAPI CoTestCancel(void);
WINOLEAPI CoEnableCallCancellation(LPVOID);
WINOLEAPI CoDisableCallCancellation(LPVOID);
#endif /* _WIN32_WINNT >= 0x0500 || defined(_WIN32_DCOM */

WINOLEAPI StringFromCLSID(REFCLSID, LPOLESTR *);
WINOLEAPI CLSIDFromString(LPCOLESTR, LPCLSID);
WINOLEAPI StringFromIID(REFIID, LPOLESTR *);
WINOLEAPI IIDFromString(LPCOLESTR, LPIID);

WINOLEAPI ProgIDFromCLSID(REFCLSID, LPOLESTR *);
WINOLEAPI CLSIDFromProgID(LPCOLESTR, LPCLSID);
WINOLEAPI_(int) StringFromGUID2(REFGUID, LPOLESTR, int);
WINOLEAPI CoCreateGuid(GUID *);

typedef struct tagPROPVARIANT PROPVARIANT;

WINOLEAPI PropVariantCopy(PROPVARIANT *, const PROPVARIANT *);
WINOLEAPI PropVariantClear(PROPVARIANT *);
WINOLEAPI FreePropVariantArray(ULONG, PROPVARIANT *);

#if (_WIN32_WINNT >= 0x0400) || defined(_WIN32_DCOM)

WINOLEAPI CoWaitForMultipleHandles(DWORD, DWORD, ULONG, LPHANDLE, LPDWORD);

typedef enum tagCOWAIT_FLAGS {
    COWAIT_DEFAULT = 0,
    COWAIT_WAITALL = 1,
    COWAIT_ALERTABLE = 2,
    COWAIT_INPUTAVAILABLE = 4,
    COWAIT_DISPATCH_CALLS = 8,
    COWAIT_DISPATCH_WINDOW_MESSAGES = 0x10,
} COWAIT_FLAGS;

#if (NTDDI_VERSION >= NTDDI_WIN8)
typedef enum CWMO_FLAGS {
    CWMO_DEFAULT = 0,
    CWMO_DISPATCH_CALLS = 1,
    CWMO_DISPATCH_WINDOW_MESSAGES = 2,
} CWMO_FLAGS;

WINOLEAPI  CoWaitForMultipleObjects(DWORD, DWORD, ULONG, const HANDLE *, LPDWORD);
#endif  /* NTDDI_VERSION >= NTDDI_WIN8 */

#define CWMO_MAX_HANDLES  56

#endif /* _WIN32_WINNT >= 0x0400) || _WIN32_DCOM */

WINOLEAPI CoGetTreatAsClass(REFCLSID, LPCLSID);

#if (_WIN32_WINNT >= 0x0501)
WINOLEAPI CoInvalidateRemoteMachineBindings(LPOLESTR);
#endif /* _WIN32_WINNT >= 0x0501 */

#if (NTDDI_VERSION >= NTDDI_WINBLUE)
enum AgileReferenceOptions {
    AGILEREFERENCE_DEFAULT = 0,
    AGILEREFERENCE_DELAYEDMARSHAL = 1,
};

WINOLEAPI RoGetAgileReference(enum AgileReferenceOptions, REFIID, IUnknown *, IAgileReference **);
#endif /* NTDDI_VERSION >= NTDDI_WINBLUE */

typedef HRESULT (STDAPICALLTYPE *LPFNGETCLASSOBJECT)(REFCLSID, REFIID, LPVOID *);
typedef HRESULT (STDAPICALLTYPE *LPFNCANUNLOADNOW)(void);
 
STDAPI  DllGetClassObject(REFCLSID, REFIID, LPVOID *);
STDAPI DllCanUnloadNow(void);

WINOLEAPI_(LPVOID) CoTaskMemAlloc(SIZE_T);
WINOLEAPI_(LPVOID) CoTaskMemRealloc(LPVOID, SIZE_T);
WINOLEAPI_(void) CoTaskMemFree(LPVOID);

#include <poppack.h>

#endif /* _COMBASEAPI_H */
