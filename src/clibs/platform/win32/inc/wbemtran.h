#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__  500
#endif

#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__  100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif

#ifndef _WBEMTRAN_H
#define _WBEMTRAN_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IWbemTransport_FWD_DEFINED__
#define __IWbemTransport_FWD_DEFINED__
typedef interface IWbemTransport IWbemTransport;
#endif

#ifndef __IWbemLevel1Login_FWD_DEFINED__
#define __IWbemLevel1Login_FWD_DEFINED__
typedef interface IWbemLevel1Login IWbemLevel1Login;
#endif

#ifndef __IWbemConnectorLogin_FWD_DEFINED__
#define __IWbemConnectorLogin_FWD_DEFINED__
typedef interface IWbemConnectorLogin IWbemConnectorLogin;
#endif

#ifndef __IWbemAddressResolution_FWD_DEFINED__
#define __IWbemAddressResolution_FWD_DEFINED__
typedef interface IWbemAddressResolution IWbemAddressResolution;
#endif

#ifndef __WbemLevel1Login_FWD_DEFINED__
#define __WbemLevel1Login_FWD_DEFINED__
#ifdef __cplusplus
typedef class WbemLevel1Login WbemLevel1Login;
#else
typedef struct WbemLevel1Login WbemLevel1Login;
#endif
#endif

#ifndef __WbemLocalAddrRes_FWD_DEFINED__
#define __WbemLocalAddrRes_FWD_DEFINED__
#ifdef __cplusplus
typedef class WbemLocalAddrRes WbemLocalAddrRes;
#else
typedef struct WbemLocalAddrRes WbemLocalAddrRes;
#endif
#endif

#ifndef __WbemUninitializedClassObject_FWD_DEFINED__
#define __WbemUninitializedClassObject_FWD_DEFINED__
#ifdef __cplusplus
typedef class WbemUninitializedClassObject WbemUninitializedClassObject;
#else
typedef struct WbemUninitializedClassObject WbemUninitializedClassObject;
#endif
#endif

#ifndef __IWbemClientTransport_FWD_DEFINED__
#define __IWbemClientTransport_FWD_DEFINED__
typedef interface IWbemClientTransport IWbemClientTransport;
#endif

#ifndef __IWbemClientConnectionTransport_FWD_DEFINED__
#define __IWbemClientConnectionTransport_FWD_DEFINED__
typedef interface IWbemClientConnectionTransport IWbemClientConnectionTransport;
#endif

#ifndef __WbemDCOMTransport_FWD_DEFINED__
#define __WbemDCOMTransport_FWD_DEFINED__
#ifdef __cplusplus
typedef class WbemDCOMTransport WbemDCOMTransport;
#else
typedef struct WbemDCOMTransport WbemDCOMTransport;
#endif
#endif

#ifndef __IWbemLevel1Login_FWD_DEFINED__
#define __IWbemLevel1Login_FWD_DEFINED__
typedef interface IWbemLevel1Login IWbemLevel1Login;
#endif

#ifndef __IWbemConnectorLogin_FWD_DEFINED__
#define __IWbemConnectorLogin_FWD_DEFINED__
typedef interface IWbemConnectorLogin IWbemConnectorLogin;
#endif

#ifndef __IWbemAddressResolution_FWD_DEFINED__
#define __IWbemAddressResolution_FWD_DEFINED__
typedef interface IWbemAddressResolution IWbemAddressResolution;
#endif

#ifndef __IWbemTransport_FWD_DEFINED__
#define __IWbemTransport_FWD_DEFINED__
typedef interface IWbemTransport IWbemTransport;
#endif

#ifndef __IWbemConstructClassObject_FWD_DEFINED__
#define __IWbemConstructClassObject_FWD_DEFINED__
typedef interface IWbemConstructClassObject IWbemConstructClassObject;
#endif

#ifndef __IWbemClientTransport_FWD_DEFINED__
#define __IWbemClientTransport_FWD_DEFINED__
typedef interface IWbemClientTransport IWbemClientTransport;
#endif

#ifndef __IWbemClientConnectionTransport_FWD_DEFINED__
#define __IWbemClientConnectionTransport_FWD_DEFINED__
typedef interface IWbemClientConnectionTransport IWbemClientConnectionTransport;
#endif

#include "objidl.h"
#include "oleidl.h"
#include "oaidl.h"
#include "wbemcli.h"

#ifdef __cplusplus
extern "C" {
#endif

void *__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void *);

#ifndef __WbemTransports_v1_LIBRARY_DEFINED__
#define __WbemTransports_v1_LIBRARY_DEFINED__

typedef enum tag_WBEM_LOGIN_TYPE {
    WBEM_FLAG_INPROC_LOGIN = 0,
    WBEM_FLAG_LOCAL_LOGIN = 1,
    WBEM_FLAG_REMOTE_LOGIN = 2,
    WBEM_AUTHENTICATION_METHOD_MASK = 0xf,
    WBEM_FLAG_USE_MULTIPLE_CHALLENGES = 0x10
} WBEM_LOGIN_TYPE;

typedef BYTE *WBEM_128BITS;

EXTERN_C const IID LIBID_WbemTransports_v1;

#ifndef __IWbemTransport_INTERFACE_DEFINED__
#define __IWbemTransport_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWbemTransport;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("553fe584-2156-11d0-b6ae-00aa003240c7") IWbemTransport:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Initialize(void) = 0;
};
#else
typedef struct IWbemTransportVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWbemTransport*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWbemTransport*);
    ULONG (STDMETHODCALLTYPE *Release)(IWbemTransport*);
    HRESULT (STDMETHODCALLTYPE *Initialize)(IWbemTransport*);
    END_INTERFACE
} IWbemTransportVtbl;

interface IWbemTransport {
    CONST_VTBL struct IWbemTransportVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemTransport_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWbemTransport_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IWbemTransport_Release(This)  (This)->lpVtbl->Release(This)
#define IWbemTransport_Initialize(This)  (This)->lpVtbl->Initialize(This)
#endif

#endif

HRESULT STDMETHODCALLTYPE IWbemTransport_Initialize_Proxy(IWbemTransport*);
void __RPC_STUB IWbemTransport_Initialize_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif

#ifndef __IWbemLevel1Login_INTERFACE_DEFINED__
#define __IWbemLevel1Login_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWbemLevel1Login;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("F309AD18-D86A-11d0-A075-00C04FB68820") IWbemLevel1Login:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE EstablishPosition(LPWSTR,DWORD,DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE RequestChallenge(LPWSTR,LPWSTR,WBEM_128BITS) = 0;
    virtual HRESULT STDMETHODCALLTYPE WBEMLogin(LPWSTR,WBEM_128BITS,long,IWbemContext*,IWbemServices**) = 0;
    virtual HRESULT STDMETHODCALLTYPE NTLMLogin(LPWSTR,LPWSTR,long,IWbemContext*,IWbemServices**) = 0;
};
#else
typedef struct IWbemLevel1LoginVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWbemLevel1Login*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWbemLevel1Login*);
    ULONG (STDMETHODCALLTYPE *Release)(IWbemLevel1Login*);
    HRESULT (STDMETHODCALLTYPE *EstablishPosition)(IWbemLevel1Login*,LPWSTR,DWORD,DWORD*);
    HRESULT (STDMETHODCALLTYPE *RequestChallenge)(IWbemLevel1Login*,LPWSTR,LPWSTR,WBEM_128BITS);
    HRESULT (STDMETHODCALLTYPE *WBEMLogin)(IWbemLevel1Login*,LPWSTR,WBEM_128BITS,long,IWbemContext*,IWbemServices**);
    HRESULT (STDMETHODCALLTYPE *NTLMLogin)(IWbemLevel1Login*,LPWSTR,LPWSTR,long,IWbemContext*,IWbemServices**);
    END_INTERFACE
} IWbemLevel1LoginVtbl;

interface IWbemLevel1Login {
    CONST_VTBL struct IWbemLevel1LoginVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemLevel1Login_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWbemLevel1Login_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IWbemLevel1Login_Release(This)  (This)->lpVtbl->Release(This)
#define IWbemLevel1Login_EstablishPosition(This,wszClientMachineName,dwProcessId,phAuthEventHandle)  (This)->lpVtbl->EstablishPosition(This,wszClientMachineName,dwProcessId,phAuthEventHandle)
#define IWbemLevel1Login_RequestChallenge(This,wszNetworkResource,wszUser,Nonce)  (This)->lpVtbl->RequestChallenge(This,wszNetworkResource,wszUser,Nonce)
#define IWbemLevel1Login_WBEMLogin(This,wszPreferredLocale,AccessToken,lFlags,pCtx,ppNamespace)  (This)->lpVtbl->WBEMLogin(This,wszPreferredLocale,AccessToken,lFlags,pCtx,ppNamespace)
#define IWbemLevel1Login_NTLMLogin(This,wszNetworkResource,wszPreferredLocale,lFlags,pCtx,ppNamespace)  (This)->lpVtbl->NTLMLogin(This,wszNetworkResource,wszPreferredLocale,lFlags,pCtx,ppNamespace)
#endif

#endif

HRESULT STDMETHODCALLTYPE IWbemLevel1Login_EstablishPosition_Proxy(IWbemLevel1Login*,LPWSTR,DWORD,DWORD*);
void __RPC_STUB IWbemLevel1Login_EstablishPosition_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IWbemLevel1Login_RequestChallenge_Proxy(IWbemLevel1Login*,LPWSTR,LPWSTR,WBEM_128BITS);
void __RPC_STUB IWbemLevel1Login_RequestChallenge_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IWbemLevel1Login_WBEMLogin_Proxy(IWbemLevel1Login*,LPWSTR,WBEM_128BITS,long,IWbemContext*,IWbemServices**);
void __RPC_STUB IWbemLevel1Login_WBEMLogin_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IWbemLevel1Login_NTLMLogin_Proxy(IWbemLevel1Login*,LPWSTR,LPWSTR,long,IWbemContext*,IWbemServices**);
void __RPC_STUB IWbemLevel1Login_NTLMLogin_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IWbemConnectorLogin_INTERFACE_DEFINED__
#define __IWbemConnectorLogin_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWbemConnectorLogin;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("d8ec9cb1-b135-4f10-8b1b-c7188bb0d186") IWbemConnectorLogin:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE ConnectorLogin(LPWSTR,LPWSTR,long,IWbemContext*,REFIID,void**) = 0;
};
#else
typedef struct IWbemConnectorLoginVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWbemConnectorLogin*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWbemConnectorLogin*);
    ULONG (STDMETHODCALLTYPE *Release)(IWbemConnectorLogin*);
    HRESULT (STDMETHODCALLTYPE *ConnectorLogin)(IWbemConnectorLogin*,LPWSTR,LPWSTR,long,IWbemContext*,REFIID,void**);
    END_INTERFACE
} IWbemConnectorLoginVtbl;

interface IWbemConnectorLogin {
    CONST_VTBL struct IWbemConnectorLoginVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemConnectorLogin_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWbemConnectorLogin_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IWbemConnectorLogin_Release(This)  (This)->lpVtbl->Release(This)
#define IWbemConnectorLogin_ConnectorLogin(This,wszNetworkResource,wszPreferredLocale,lFlags,pCtx,riid,pInterface)  (This)->lpVtbl->ConnectorLogin(This,wszNetworkResource,wszPreferredLocale,lFlags,pCtx,riid,pInterface)
#endif

#endif

HRESULT STDMETHODCALLTYPE IWbemConnectorLogin_ConnectorLogin_Proxy(IWbemConnectorLogin*,LPWSTR,LPWSTR,long,IWbemContext*,REFIID,void**);
void __RPC_STUB IWbemConnectorLogin_ConnectorLogin_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif

#ifndef __IWbemAddressResolution_INTERFACE_DEFINED__
#define __IWbemAddressResolution_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWbemAddressResolution;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("F7CE2E12-8C90-11d1-9E7B-00C04FC324A8") IWbemAddressResolution:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Resolve(LPWSTR,LPWSTR,DWORD*,BYTE**) = 0;
};
#else
typedef struct IWbemAddressResolutionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWbemAddressResolution*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWbemAddressResolution*);
    ULONG (STDMETHODCALLTYPE *Release)(IWbemAddressResolution*);
    HRESULT (STDMETHODCALLTYPE *Resolve)(IWbemAddressResolution*,LPWSTR,LPWSTR,DWORD*,BYTE**);
    END_INTERFACE
} IWbemAddressResolutionVtbl;

interface IWbemAddressResolution {
    CONST_VTBL struct IWbemAddressResolutionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemAddressResolution_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWbemAddressResolution_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IWbemAddressResolution_Release(This)  (This)->lpVtbl->Release(This)
#define IWbemAddressResolution_Resolve(This,wszNamespacePath,wszAddressType,pdwAddressLength,pabBinaryAddress)  (This)->lpVtbl->Resolve(This,wszNamespacePath,wszAddressType,pdwAddressLength,pabBinaryAddress)
#endif

#endif

HRESULT STDMETHODCALLTYPE IWbemAddressResolution_Resolve_Proxy(IWbemAddressResolution*,LPWSTR,LPWSTR,DWORD*,BYTE**);
void __RPC_STUB IWbemAddressResolution_Resolve_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif

EXTERN_C const CLSID CLSID_WbemLevel1Login;

#ifdef __cplusplus
class DECLSPEC_UUID("8BC3F05E-D86B-11d0-A075-00C04FB68820") WbemLevel1Login;
#endif

EXTERN_C const CLSID CLSID_WbemLocalAddrRes;

#ifdef __cplusplus
class DECLSPEC_UUID("A1044801-8F7E-11d1-9E7C-00C04FC324A8") WbemLocalAddrRes;
#endif

EXTERN_C const CLSID CLSID_WbemUninitializedClassObject;

#ifdef __cplusplus
class DECLSPEC_UUID("7a0227f6-7108-11d1-ad90-00c04fd8fdff") WbemUninitializedClassObject;
#endif

#ifndef __IWbemClientTransport_INTERFACE_DEFINED__
#define __IWbemClientTransport_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWbemClientTransport;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("F7CE2E11-8C90-11d1-9E7B-00C04FC324A8") IWbemClientTransport:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE ConnectServer(BSTR,DWORD,BYTE*,BSTR,BSTR,BSTR,BSTR,long,BSTR,IWbemContext*,IWbemServices**) = 0;
};
#else
typedef struct IWbemClientTransportVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWbemClientTransport*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWbemClientTransport*);
    ULONG (STDMETHODCALLTYPE *Release)(IWbemClientTransport*);
    HRESULT (STDMETHODCALLTYPE *ConnectServer)(IWbemClientTransport*,BSTR,DWORD,BYTE*,BSTR,BSTR,BSTR,BSTR,long,BSTR,IWbemContext*,IWbemServices**);
    END_INTERFACE
} IWbemClientTransportVtbl;

interface IWbemClientTransport {
    CONST_VTBL struct IWbemClientTransportVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemClientTransport_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWbemClientTransport_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IWbemClientTransport_Release(This)  (This)->lpVtbl->Release(This)
#define IWbemClientTransport_ConnectServer(This,strAddressType,dwBinaryAddressLength,abBinaryAddress,strNetworkResource,strUser,strPassword,strLocale,lSecurityFlags,strAuthority,pCtx,ppNamespace)  (This)->lpVtbl->ConnectServer(This,strAddressType,dwBinaryAddressLength,abBinaryAddress,strNetworkResource,strUser,strPassword,strLocale,lSecurityFlags,strAuthority,pCtx,ppNamespace)
#endif

#endif

HRESULT STDMETHODCALLTYPE IWbemClientTransport_ConnectServer_Proxy(IWbemClientTransport*,BSTR,DWORD,BYTE*,BSTR,BSTR,BSTR,BSTR,long,BSTR,IWbemContext*,IWbemServices**);
void __RPC_STUB IWbemClientTransport_ConnectServer_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif

#ifndef __IWbemClientConnectionTransport_INTERFACE_DEFINED__
#define __IWbemClientConnectionTransport_INTERFACE_DEFINED__

    EXTERN_C const IID IID_IWbemClientConnectionTransport;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a889c72a-fcc1-4a9e-af61-ed071333fb5b") IWbemClientConnectionTransport:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Open(BSTR,DWORD,BYTE*,const BSTR,const BSTR,const BSTR,const BSTR,long,IWbemContext*,REFIID,void**,IWbemCallResult**) = 0;
    virtual HRESULT STDMETHODCALLTYPE OpenAsync(BSTR,DWORD,BYTE*,const BSTR,const BSTR,const BSTR,const BSTR,long,IWbemContext*,REFIID,IWbemObjectSink*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(long,IWbemObjectSink*) = 0;
};
#else
typedef struct IWbemClientConnectionTransportVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWbemClientConnectionTransport*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWbemClientConnectionTransport*);
    ULONG (STDMETHODCALLTYPE *Release)(IWbemClientConnectionTransport*);
    HRESULT (STDMETHODCALLTYPE *Open)(IWbemClientConnectionTransport*,BSTR,DWORD,BYTE*,const BSTR,const BSTR,const BSTR,const BSTR,long,IWbemContext*,REFIID,void**,IWbemCallResult**);
    HRESULT (STDMETHODCALLTYPE *OpenAsync)(IWbemClientConnectionTransport*,BSTR,DWORD,BYTE*,const BSTR,const BSTR,const BSTR,const BSTR,long,IWbemContext*,REFIID,IWbemObjectSink*);
    HRESULT (STDMETHODCALLTYPE *Cancel)(IWbemClientConnectionTransport*,long,IWbemObjectSink*);
    END_INTERFACE
} IWbemClientConnectionTransportVtbl;

interface IWbemClientConnectionTransport {
    CONST_VTBL struct IWbemClientConnectionTransportVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemClientConnectionTransport_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWbemClientConnectionTransport_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IWbemClientConnectionTransport_Release(This)  (This)->lpVtbl->Release(This)
#define IWbemClientConnectionTransport_Open(This,strAddressType,dwBinaryAddressLength,abBinaryAddress,strObject,strUser,strPassword,strLocale,lFlags,pCtx,riid,pInterface,pCallRes)  (This)->lpVtbl->Open(This,strAddressType,dwBinaryAddressLength,abBinaryAddress,strObject,strUser,strPassword,strLocale,lFlags,pCtx,riid,pInterface,pCallRes)
#define IWbemClientConnectionTransport_OpenAsync(This,strAddressType,dwBinaryAddressLength,abBinaryAddress,strObject,strUser,strPassword,strLocale,lFlags,pCtx,riid,pResponseHandler)  (This)->lpVtbl->OpenAsync(This,strAddressType,dwBinaryAddressLength,abBinaryAddress,strObject,strUser,strPassword,strLocale,lFlags,pCtx,riid,pResponseHandler)
#define IWbemClientConnectionTransport_Cancel(This,lFlags,pHandler)  (This)->lpVtbl->Cancel(This,lFlags,pHandler)
#endif

#endif

HRESULT STDMETHODCALLTYPE IWbemClientConnectionTransport_Open_Proxy(IWbemClientConnectionTransport*,BSTR,DWORD,BYTE*,const BSTR,const BSTR,const BSTR,const BSTR,long,IWbemContext*,REFIID,void**,IWbemCallResult**);
void __RPC_STUB IWbemClientConnectionTransport_Open_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IWbemClientConnectionTransport_OpenAsync_Proxy(IWbemClientConnectionTransport*,BSTR,DWORD,BYTE*,const BSTR,const BSTR,const BSTR,const BSTR,long,IWbemContext*,REFIID,IWbemObjectSink*);
void __RPC_STUB IWbemClientConnectionTransport_OpenAsync_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IWbemClientConnectionTransport_Cancel_Proxy(IWbemClientConnectionTransport*,long,IWbemObjectSink*);
void __RPC_STUB IWbemClientConnectionTransport_Cancel_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif

EXTERN_C const CLSID CLSID_WbemDCOMTransport;

#ifdef __cplusplus
class DECLSPEC_UUID("F7CE2E13-8C90-11d1-9E7B-00C04FC324A8") WbemDCOMTransport;
#endif
#endif

#ifndef __IWbemConstructClassObject_INTERFACE_DEFINED__
#define __IWbemConstructClassObject_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWbemConstructClassObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("9ef76194-70d5-11d1-ad90-00c04fd8fdff") IWbemConstructClassObject:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE SetInheritanceChain(long,LPWSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyOrigin(LPCWSTR,long) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMethodOrigin(LPCWSTR,long) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetServerNamespace(LPCWSTR,LPCWSTR) = 0;
};
#else
typedef struct IWbemConstructClassObjectVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWbemConstructClassObject*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWbemConstructClassObject*);
    ULONG (STDMETHODCALLTYPE *Release)(IWbemConstructClassObject*);
    HRESULT (STDMETHODCALLTYPE *SetInheritanceChain)(IWbemConstructClassObject*,long,LPWSTR*);
    HRESULT (STDMETHODCALLTYPE *SetPropertyOrigin)(IWbemConstructClassObject*,LPCWSTR,long);
    HRESULT (STDMETHODCALLTYPE *SetMethodOrigin)(IWbemConstructClassObject*,LPCWSTR,long);
    HRESULT (STDMETHODCALLTYPE *SetServerNamespace)(IWbemConstructClassObject*,LPCWSTR,LPCWSTR);
    END_INTERFACE
} IWbemConstructClassObjectVtbl;

interface IWbemConstructClassObject {
    CONST_VTBL struct IWbemConstructClassObjectVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemConstructClassObject_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWbemConstructClassObject_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IWbemConstructClassObject_Release(This)  (This)->lpVtbl->Release(This)
#define IWbemConstructClassObject_SetInheritanceChain(This,lNumAntecedents,awszAntecedents)  (This)->lpVtbl->SetInheritanceChain(This,lNumAntecedents,awszAntecedents)
#define IWbemConstructClassObject_SetPropertyOrigin(This,wszPropertyName,lOriginIndex)  (This)->lpVtbl->SetPropertyOrigin(This,wszPropertyName,lOriginIndex)
#define IWbemConstructClassObject_SetMethodOrigin(This,wszMethodName,lOriginIndex)  (This)->lpVtbl->SetMethodOrigin(This,wszMethodName,lOriginIndex)
#define IWbemConstructClassObject_SetServerNamespace(This,wszServer,wszNamespace)  (This)->lpVtbl->SetServerNamespace(This,wszServer,wszNamespace)
#endif

#endif

HRESULT STDMETHODCALLTYPE IWbemConstructClassObject_SetInheritanceChain_Proxy(IWbemConstructClassObject*,long,LPWSTR*);
void __RPC_STUB IWbemConstructClassObject_SetInheritanceChain_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IWbemConstructClassObject_SetPropertyOrigin_Proxy(IWbemConstructClassObject*,LPCWSTR,long);
void __RPC_STUB IWbemConstructClassObject_SetPropertyOrigin_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IWbemConstructClassObject_SetMethodOrigin_Proxy(IWbemConstructClassObject*,LPCWSTR,long);
void __RPC_STUB IWbemConstructClassObject_SetMethodOrigin_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IWbemConstructClassObject_SetServerNamespace_Proxy(IWbemConstructClassObject*,LPCWSTR,LPCWSTR);
void __RPC_STUB IWbemConstructClassObject_SetServerNamespace_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif

#ifdef __cplusplus
}
#endif

#endif /* _WBEMTRAN_H */
