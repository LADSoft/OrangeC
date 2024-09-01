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
#endif /* COM_NO_WINDOWS_H */

#ifndef _PORTABLEDEVICECONNECTAPI_H
#define _PORTABLEDEVICECONNECTAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Forward Declarations */ 

#ifndef __IEnumPortableDeviceConnectors_FWD_DEFINED__
#define __IEnumPortableDeviceConnectors_FWD_DEFINED__
typedef interface IEnumPortableDeviceConnectors IEnumPortableDeviceConnectors;
#endif /* __IEnumPortableDeviceConnectors_FWD_DEFINED__ */

#ifndef __IPortableDeviceConnector_FWD_DEFINED__
#define __IPortableDeviceConnector_FWD_DEFINED__
typedef interface IPortableDeviceConnector IPortableDeviceConnector;
#endif /* __IPortableDeviceConnector_FWD_DEFINED__ */

#ifndef __IConnectionRequestCallback_FWD_DEFINED__
#define __IConnectionRequestCallback_FWD_DEFINED__
typedef interface IConnectionRequestCallback IConnectionRequestCallback;
#endif /* __IConnectionRequestCallback_FWD_DEFINED__ */

#ifndef __EnumBthMtpConnectors_FWD_DEFINED__
#define __EnumBthMtpConnectors_FWD_DEFINED__

#ifdef __cplusplus
typedef class EnumBthMtpConnectors EnumBthMtpConnectors;
#else
typedef struct EnumBthMtpConnectors EnumBthMtpConnectors;
#endif /* __cplusplus */

#endif /* __EnumBthMtpConnectors_FWD_DEFINED__ */

#include "wtypes.h"
#include "propsys.h"
/* #include "PortableDeviceConnectImports.h" */

#ifdef __cplusplus
extern "C"{
#endif 

DEFINE_DEVPROPKEY(DEVPKEY_MTPBTH_IsConnected, 0xea1237fa, 0x589d, 0x4472, 0x84, 0xe4, 0x0a, 0xbe, 0x36, 0xfd, 0x62, 0xef, 2);

extern RPC_IF_HANDLE __MIDL_itf_PortableDeviceConnectApi_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_PortableDeviceConnectApi_0000_0000_v0_0_s_ifspec;

#ifndef __IEnumPortableDeviceConnectors_INTERFACE_DEFINED__
#define __IEnumPortableDeviceConnectors_INTERFACE_DEFINED__

EXTERN_C const IID IID_IEnumPortableDeviceConnectors;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("bfdef549-9247-454f-bd82-06fe80853faa") IEnumPortableDeviceConnectors:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Next(UINT32, IPortableDeviceConnector **, UINT32 *) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(UINT32) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumPortableDeviceConnectors **) = 0;
};
#else /* C style interface */
typedef struct IEnumPortableDeviceConnectorsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IEnumPortableDeviceConnectors *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IEnumPortableDeviceConnectors *);
    ULONG (STDMETHODCALLTYPE *Release)(IEnumPortableDeviceConnectors *);
    HRESULT (STDMETHODCALLTYPE *Next)(IEnumPortableDeviceConnectors *, UINT32, IPortableDeviceConnector **, UINT32 *);
    HRESULT (STDMETHODCALLTYPE *Skip)(IEnumPortableDeviceConnectors *, UINT32);
    HRESULT (STDMETHODCALLTYPE *Reset)(IEnumPortableDeviceConnectors *);
    HRESULT (STDMETHODCALLTYPE *Clone)(IEnumPortableDeviceConnectors *, IEnumPortableDeviceConnectors **);
    END_INTERFACE
} IEnumPortableDeviceConnectorsVtbl;

interface IEnumPortableDeviceConnectors {
    CONST_VTBL struct IEnumPortableDeviceConnectorsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEnumPortableDeviceConnectors_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IEnumPortableDeviceConnectors_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IEnumPortableDeviceConnectors_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IEnumPortableDeviceConnectors_Next(This,cRequested,pConnectors,pcFetched)  ((This)->lpVtbl->Next(This,cRequested,pConnectors,pcFetched)) 
#define IEnumPortableDeviceConnectors_Skip(This,cConnectors)  ((This)->lpVtbl->Skip(This,cConnectors)) 
#define IEnumPortableDeviceConnectors_Reset(This)  ((This)->lpVtbl->Reset(This)) 
#define IEnumPortableDeviceConnectors_Clone(This,ppEnum)  ((This)->lpVtbl->Clone(This,ppEnum)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IEnumPortableDeviceConnectors_INTERFACE_DEFINED__ */


#ifndef __IPortableDeviceConnector_INTERFACE_DEFINED__
#define __IPortableDeviceConnector_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPortableDeviceConnector;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("625e2df8-6392-4cf0-9ad1-3cfa5f17775c") IPortableDeviceConnector:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Connect(IConnectionRequestCallback *) = 0;
    virtual HRESULT STDMETHODCALLTYPE Disconnect(IConnectionRequestCallback *) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(IConnectionRequestCallback *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperty(const DEVPROPKEY *, DEVPROPTYPE *, BYTE **, UINT32 *) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProperty(const DEVPROPKEY *, DEVPROPTYPE, const BYTE *, UINT32) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPnPID(LPWSTR *) = 0;
};
#else /* C style interface */
typedef struct IPortableDeviceConnectorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IPortableDeviceConnector *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IPortableDeviceConnector *);
    ULONG (STDMETHODCALLTYPE *Release)(IPortableDeviceConnector *);
    HRESULT (STDMETHODCALLTYPE *Connect)(IPortableDeviceConnector *, IConnectionRequestCallback *);
    HRESULT (STDMETHODCALLTYPE *Disconnect)(IPortableDeviceConnector *, IConnectionRequestCallback *);
    HRESULT (STDMETHODCALLTYPE *Cancel)(IPortableDeviceConnector *, IConnectionRequestCallback *);
    HRESULT (STDMETHODCALLTYPE *GetProperty)(IPortableDeviceConnector *, const DEVPROPKEY *, DEVPROPTYPE *, BYTE **, UINT32 *);
    HRESULT (STDMETHODCALLTYPE *SetProperty)(IPortableDeviceConnector *, const DEVPROPKEY *, DEVPROPTYPE, const BYTE *, UINT32);
    HRESULT (STDMETHODCALLTYPE *GetPnPID)(IPortableDeviceConnector *, LPWSTR *);
    END_INTERFACE
} IPortableDeviceConnectorVtbl;

interface IPortableDeviceConnector {
    CONST_VTBL struct IPortableDeviceConnectorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPortableDeviceConnector_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IPortableDeviceConnector_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IPortableDeviceConnector_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IPortableDeviceConnector_Connect(This,pCallback)  ((This)->lpVtbl->Connect(This,pCallback)) 
#define IPortableDeviceConnector_Disconnect(This,pCallback)  ((This)->lpVtbl->Disconnect(This,pCallback)) 
#define IPortableDeviceConnector_Cancel(This,pCallback)  ((This)->lpVtbl->Cancel(This,pCallback)) 
#define IPortableDeviceConnector_GetProperty(This,pPropertyKey,pPropertyType,ppData,pcbData)  ((This)->lpVtbl->GetProperty(This,pPropertyKey,pPropertyType,ppData,pcbData)) 
#define IPortableDeviceConnector_SetProperty(This,pPropertyKey,PropertyType,pData,cbData)  ((This)->lpVtbl->SetProperty(This,pPropertyKey,PropertyType,pData,cbData)) 
#define IPortableDeviceConnector_GetPnPID(This,ppwszPnPID)  ((This)->lpVtbl->GetPnPID(This,ppwszPnPID)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IPortableDeviceConnector_INTERFACE_DEFINED__ */


#ifndef __IConnectionRequestCallback_INTERFACE_DEFINED__
#define __IConnectionRequestCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_IConnectionRequestCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("272c9ae0-7161-4ae0-91bd-9f448ee9c427") IConnectionRequestCallback:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnComplete(HRESULT) = 0;

};
#else /* C style interface */

typedef struct IConnectionRequestCallbackVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IConnectionRequestCallback *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IConnectionRequestCallback *);
    ULONG (STDMETHODCALLTYPE *Release)(IConnectionRequestCallback *);
    HRESULT (STDMETHODCALLTYPE *OnComplete)(IConnectionRequestCallback *, HRESULT);
    END_INTERFACE
} IConnectionRequestCallbackVtbl;

interface IConnectionRequestCallback {
    CONST_VTBL struct IConnectionRequestCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IConnectionRequestCallback_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IConnectionRequestCallback_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IConnectionRequestCallback_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IConnectionRequestCallback_OnComplete(This,hrStatus)  ((This)->lpVtbl->OnComplete(This,hrStatus)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IConnectionRequestCallback_INTERFACE_DEFINED__ */


#ifndef __PortableDeviceConnectApiLib_LIBRARY_DEFINED__
#define __PortableDeviceConnectApiLib_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_PortableDeviceConnectApiLib;

EXTERN_C const CLSID CLSID_EnumBthMtpConnectors;

#ifdef __cplusplus
class DECLSPEC_UUID("a1570149-e645-4f43-8b0d-409b061db2fc") EnumBthMtpConnectors;
#endif
#endif /* __PortableDeviceConnectApiLib_LIBRARY_DEFINED__ */

#ifdef __cplusplus
}
#endif

#endif /* _PORTABLEDEVICECONNECTAPI_H */
