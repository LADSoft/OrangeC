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

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef _WCNDEVICE_H
#define _WCNDEVICE_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IWCNDevice_FWD_DEFINED__
#define __IWCNDevice_FWD_DEFINED__
typedef interface IWCNDevice IWCNDevice;
#endif /* __IWCNDevice_FWD_DEFINED__ */

#ifndef __IWCNConnectNotify_FWD_DEFINED__
#define __IWCNConnectNotify_FWD_DEFINED__
typedef interface IWCNConnectNotify IWCNConnectNotify;
#endif /* __IWCNConnectNotify_FWD_DEFINED__ */

#ifndef __WCNDeviceObject_FWD_DEFINED__
#define __WCNDeviceObject_FWD_DEFINED__
#ifdef __cplusplus
typedef class WCNDeviceObject WCNDeviceObject;
#else
typedef struct WCNDeviceObject WCNDeviceObject;
#endif /* __cplusplus */
#endif  /* __WCNDeviceObject_FWD_DEFINED__ */

#include "objidl.h"
#include "wcntypes.h"

#ifdef __cplusplus
extern "C"{
#endif 

#if NTDDI_VERSION >= NTDDI_WIN7

#define WCN_API_MAX_BUFFER_SIZE  (2096)

typedef enum tagWCN_PASSWORD_TYPE {
    WCN_PASSWORD_TYPE_PUSH_BUTTON = 0,
    WCN_PASSWORD_TYPE_PIN = (WCN_PASSWORD_TYPE_PUSH_BUTTON + 1)
} WCN_PASSWORD_TYPE;

typedef enum tagWCN_SESSION_STATUS {
    WCN_SESSION_STATUS_SUCCESS = 0,
    WCN_SESSION_STATUS_FAILURE_GENERIC = (WCN_SESSION_STATUS_SUCCESS + 1),
    WCN_SESSION_STATUS_FAILURE_TIMEOUT = (WCN_SESSION_STATUS_FAILURE_GENERIC + 1)
} WCN_SESSION_STATUS;

typedef struct tagWCN_VENDOR_EXTENSION_SPEC {
    DWORD VendorId;
    DWORD SubType;
    DWORD Index;
    DWORD Flags;
} WCN_VENDOR_EXTENSION_SPEC;

#define WCN_MICROSOFT_VENDOR_ID  (311)

#define WCN_NO_SUBTYPE  (0xfffffffe)

#define WCN_FLAG_DISCOVERY_VE  (0x1)
#define WCN_FLAG_AUTHENTICATED_VE  (0x2)
#define WCN_FLAG_ENCRYPTED_VE  (0x4)

extern RPC_IF_HANDLE __MIDL_itf_wcndevice_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wcndevice_0000_0000_v0_0_s_ifspec;

#ifndef __IWCNDevice_INTERFACE_DEFINED__
#define __IWCNDevice_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWCNDevice;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("C100BE9C-D33A-4a4b-BF23-BBEF4663D017") IWCNDevice : public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetPassword(WCN_PASSWORD_TYPE Type, DWORD dwPasswordLength, const BYTE pbPassword[]) = 0;
    virtual HRESULT STDMETHODCALLTYPE Connect(IWCNConnectNotify *pNotify) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAttribute(WCN_ATTRIBUTE_TYPE AttributeType, DWORD dwMaxBufferSize, BYTE pbBuffer[], DWORD *pdwBufferUsed) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetIntegerAttribute(WCN_ATTRIBUTE_TYPE AttributeType, UINT *puInteger) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStringAttribute(WCN_ATTRIBUTE_TYPE AttributeType, DWORD cchMaxString, WCHAR wszString[]) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNetworkProfile(DWORD cchMaxStringLength, LPWSTR wszProfile) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetNetworkProfile(LPCWSTR pszProfileXml) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVendorExtension(const WCN_VENDOR_EXTENSION_SPEC *pVendorExtSpec, DWORD dwMaxBufferSize, BYTE pbBuffer[], DWORD *pdwBufferUsed) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetVendorExtension(const WCN_VENDOR_EXTENSION_SPEC *pVendorExtSpec, DWORD cbBuffer, const BYTE pbBuffer[]) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unadvise( void) = 0;
};
#else /* C style interface */
typedef struct IWCNDeviceVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IWCNDevice * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IWCNDevice * This);
    ULONG (STDMETHODCALLTYPE * Release) (IWCNDevice * This);
    HRESULT (STDMETHODCALLTYPE * SetPassword) (IWCNDevice * This, WCN_PASSWORD_TYPE Type, DWORD dwPasswordLength, const BYTE pbPassword[]);
    HRESULT (STDMETHODCALLTYPE * Connect) (IWCNDevice * This, IWCNConnectNotify * pNotify);
    HRESULT (STDMETHODCALLTYPE * GetAttribute) (IWCNDevice * This, WCN_ATTRIBUTE_TYPE AttributeType, DWORD dwMaxBufferSize, BYTE pbBuffer[], DWORD * pdwBufferUsed);
    HRESULT (STDMETHODCALLTYPE * GetIntegerAttribute) (IWCNDevice * This, WCN_ATTRIBUTE_TYPE AttributeType, UINT * puInteger);
    HRESULT (STDMETHODCALLTYPE * GetStringAttribute) (IWCNDevice * This, WCN_ATTRIBUTE_TYPE AttributeType, DWORD cchMaxString, WCHAR wszString[]);
    HRESULT (STDMETHODCALLTYPE * GetNetworkProfile) (IWCNDevice * This, DWORD cchMaxStringLength, LPWSTR wszProfile);
    HRESULT (STDMETHODCALLTYPE * SetNetworkProfile) (IWCNDevice * This, LPCWSTR pszProfileXml);
    HRESULT (STDMETHODCALLTYPE * GetVendorExtension) (IWCNDevice * This, const WCN_VENDOR_EXTENSION_SPEC * pVendorExtSpec, DWORD dwMaxBufferSize, BYTE pbBuffer[], DWORD * pdwBufferUsed);
    HRESULT (STDMETHODCALLTYPE * SetVendorExtension) (IWCNDevice * This, const WCN_VENDOR_EXTENSION_SPEC * pVendorExtSpec, DWORD cbBuffer, const BYTE pbBuffer[]);
    HRESULT (STDMETHODCALLTYPE * Unadvise) (IWCNDevice * This);
    END_INTERFACE
} IWCNDeviceVtbl;

interface IWCNDevice {
    CONST_VTBL struct IWCNDeviceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWCNDevice_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWCNDevice_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWCNDevice_Release(This)  ((This)->lpVtbl->Release(This))
#define IWCNDevice_SetPassword(This,Type,dwPasswordLength,pbPassword)  ((This)->lpVtbl->SetPassword(This,Type,dwPasswordLength,pbPassword))
#define IWCNDevice_Connect(This,pNotify)  ((This)->lpVtbl->Connect(This,pNotify))
#define IWCNDevice_GetAttribute(This,AttributeType,dwMaxBufferSize,pbBuffer,pdwBufferUsed)  ((This)->lpVtbl->GetAttribute(This,AttributeType,dwMaxBufferSize,pbBuffer,pdwBufferUsed))
#define IWCNDevice_GetIntegerAttribute(This,AttributeType,puInteger)  ((This)->lpVtbl->GetIntegerAttribute(This,AttributeType,puInteger))
#define IWCNDevice_GetStringAttribute(This,AttributeType,cchMaxString,wszString)  ((This)->lpVtbl->GetStringAttribute(This,AttributeType,cchMaxString,wszString))
#define IWCNDevice_GetNetworkProfile(This,cchMaxStringLength,wszProfile)  ((This)->lpVtbl->GetNetworkProfile(This,cchMaxStringLength,wszProfile)) 
#define IWCNDevice_SetNetworkProfile(This,pszProfileXml)  ((This)->lpVtbl->SetNetworkProfile(This,pszProfileXml))
#define IWCNDevice_GetVendorExtension(This,pVendorExtSpec,dwMaxBufferSize,pbBuffer,pdwBufferUsed)  ((This)->lpVtbl->GetVendorExtension(This,pVendorExtSpec,dwMaxBufferSize,pbBuffer,pdwBufferUsed))
#define IWCNDevice_SetVendorExtension(This,pVendorExtSpec,cbBuffer,pbBuffer)  ((This)->lpVtbl->SetVendorExtension(This,pVendorExtSpec,cbBuffer,pbBuffer))
#define IWCNDevice_Unadvise(This)  ((This)->lpVtbl->Unadvise(This)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWCNDevice_INTERFACE_DEFINED__ */

#ifndef __IWCNConnectNotify_INTERFACE_DEFINED__
#define __IWCNConnectNotify_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWCNConnectNotify;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("C100BE9F-D33A-4a4b-BF23-BBEF4663D017") IWCNConnectNotify : public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE ConnectSucceeded( void) = 0;
    virtual HRESULT STDMETHODCALLTYPE ConnectFailed(HRESULT hrFailure) = 0;
};
#else /* C style interface */
typedef struct IWCNConnectNotifyVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IWCNConnectNotify *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWCNConnectNotify *This);
    ULONG (STDMETHODCALLTYPE *Release)(IWCNConnectNotify *This);
    HRESULT (STDMETHODCALLTYPE *ConnectSucceeded)(IWCNConnectNotify *This);
    HRESULT (STDMETHODCALLTYPE *ConnectFailed)(IWCNConnectNotify *This, HRESULT hrFailure);
    END_INTERFACE
} IWCNConnectNotifyVtbl;

interface IWCNConnectNotify {
    CONST_VTBL struct IWCNConnectNotifyVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWCNConnectNotify_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWCNConnectNotify_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWCNConnectNotify_Release(This)  ((This)->lpVtbl->Release(This))
#define IWCNConnectNotify_ConnectSucceeded(This)  ((This)->lpVtbl->ConnectSucceeded(This))
#define IWCNConnectNotify_ConnectFailed(This,hrFailure)  ((This)->lpVtbl->ConnectFailed(This,hrFailure))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWCNConnectNotify_INTERFACE_DEFINED__ */

#ifndef __WCNDeviceObjectLibrary_LIBRARY_DEFINED__
#define __WCNDeviceObjectLibrary_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_WCNDeviceObjectLibrary;
EXTERN_C const CLSID CLSID_WCNDeviceObject;

#ifdef __cplusplus

class DECLSPEC_UUID("C100BEA7-D33A-4a4b-BF23-BBEF4663D017") WCNDeviceObject;
#endif
#endif /* __WCNDeviceObjectLibrary_LIBRARY_DEFINED__ */

#endif /* NTDDI_VERSION >= NTDDI_WIN7 */

extern RPC_IF_HANDLE __MIDL_itf_wcndevice_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wcndevice_0000_0002_v0_0_s_ifspec;

#ifdef __cplusplus
}
#endif

#endif /* _WCNDEVICE_H */
