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
#endif /*COM_NO_WINDOWS_H */

#ifndef _WBEMPROV_H
#define _WBEMPROV_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IWbemPropertyProvider_FWD_DEFINED__
#define __IWbemPropertyProvider_FWD_DEFINED__
typedef interface IWbemPropertyProvider IWbemPropertyProvider;
#endif

#ifndef __IWbemUnboundObjectSink_FWD_DEFINED__
#define __IWbemUnboundObjectSink_FWD_DEFINED__
typedef interface IWbemUnboundObjectSink IWbemUnboundObjectSink;
#endif

#ifndef __IWbemEventProvider_FWD_DEFINED__
#define __IWbemEventProvider_FWD_DEFINED__
typedef interface IWbemEventProvider IWbemEventProvider;
#endif

#ifndef __IWbemEventProviderQuerySink_FWD_DEFINED__
#define __IWbemEventProviderQuerySink_FWD_DEFINED__
typedef interface IWbemEventProviderQuerySink IWbemEventProviderQuerySink;
#endif

#ifndef __IWbemEventConsumerProvider_FWD_DEFINED__
#define __IWbemEventConsumerProvider_FWD_DEFINED__
typedef interface IWbemEventConsumerProvider IWbemEventConsumerProvider;
#endif

#ifndef __IWbemProviderInitSink_FWD_DEFINED__
#define __IWbemProviderInitSink_FWD_DEFINED__
typedef interface IWbemProviderInitSink IWbemProviderInitSink;
#endif

#ifndef __IWbemProviderInit_FWD_DEFINED__
#define __IWbemProviderInit_FWD_DEFINED__
typedef interface IWbemProviderInit IWbemProviderInit;
#endif

#ifndef __IWbemHiPerfProvider_FWD_DEFINED__
#define __IWbemHiPerfProvider_FWD_DEFINED__
typedef interface IWbemHiPerfProvider IWbemHiPerfProvider;
#endif

#ifndef __WbemAdministrativeLocator_FWD_DEFINED__
#define __WbemAdministrativeLocator_FWD_DEFINED__
#ifdef __cplusplus
typedef class WbemAdministrativeLocator WbemAdministrativeLocator;
#else
typedef struct WbemAdministrativeLocator WbemAdministrativeLocator;
#endif /* __cplusplus */
#endif /* __WbemAdministrativeLocator_FWD_DEFINED__ */

#ifndef __WbemAuthenticatedLocator_FWD_DEFINED__
#define __WbemAuthenticatedLocator_FWD_DEFINED__
#ifdef __cplusplus
typedef class WbemAuthenticatedLocator WbemAuthenticatedLocator;
#else
typedef struct WbemAuthenticatedLocator WbemAuthenticatedLocator;
#endif /* __cplusplus */
#endif /* __WbemAuthenticatedLocator_FWD_DEFINED__ */

#ifndef __WbemUnauthenticatedLocator_FWD_DEFINED__
#define __WbemUnauthenticatedLocator_FWD_DEFINED__
#ifdef __cplusplus
typedef class WbemUnauthenticatedLocator WbemUnauthenticatedLocator;
#else
typedef struct WbemUnauthenticatedLocator WbemUnauthenticatedLocator;
#endif /* __cplusplus */
#endif /* __WbemUnauthenticatedLocator_FWD_DEFINED__ */

#ifndef __IWbemUnboundObjectSink_FWD_DEFINED__
#define __IWbemUnboundObjectSink_FWD_DEFINED__
typedef interface IWbemUnboundObjectSink IWbemUnboundObjectSink;
#endif

#ifndef __IWbemPropertyProvider_FWD_DEFINED__
#define __IWbemPropertyProvider_FWD_DEFINED__
typedef interface IWbemPropertyProvider IWbemPropertyProvider;
#endif

#ifndef __IWbemEventProvider_FWD_DEFINED__
#define __IWbemEventProvider_FWD_DEFINED__
typedef interface IWbemEventProvider IWbemEventProvider;
#endif

#ifndef __IWbemEventProviderQuerySink_FWD_DEFINED__
#define __IWbemEventProviderQuerySink_FWD_DEFINED__
typedef interface IWbemEventProviderQuerySink IWbemEventProviderQuerySink;
#endif

#ifndef __IWbemEventProviderSecurity_FWD_DEFINED__
#define __IWbemEventProviderSecurity_FWD_DEFINED__
typedef interface IWbemEventProviderSecurity IWbemEventProviderSecurity;
#endif

#ifndef __IWbemProviderIdentity_FWD_DEFINED__
#define __IWbemProviderIdentity_FWD_DEFINED__
typedef interface IWbemProviderIdentity IWbemProviderIdentity;
#endif

#ifndef __IWbemEventConsumerProvider_FWD_DEFINED__
#define __IWbemEventConsumerProvider_FWD_DEFINED__
typedef interface IWbemEventConsumerProvider IWbemEventConsumerProvider;
#endif

#ifndef __IWbemProviderInitSink_FWD_DEFINED__
#define __IWbemProviderInitSink_FWD_DEFINED__
typedef interface IWbemProviderInitSink IWbemProviderInitSink;
#endif

#ifndef __IWbemProviderInit_FWD_DEFINED__
#define __IWbemProviderInit_FWD_DEFINED__
typedef interface IWbemProviderInit IWbemProviderInit;
#endif

#ifndef __IWbemHiPerfProvider_FWD_DEFINED__
#define __IWbemHiPerfProvider_FWD_DEFINED__
typedef interface IWbemHiPerfProvider IWbemHiPerfProvider;
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

typedef VARIANT WBEM_VARIANT;

typedef WCHAR *WBEM_WSTR;

typedef const WCHAR *WBEM_CWSTR;

typedef enum tag_WBEM_PROVIDER_REQUIREMENTS_TYPE {
    WBEM_REQUIREMENTS_START_POSTFILTER = 0,
    WBEM_REQUIREMENTS_STOP_POSTFILTER = 1,
    WBEM_REQUIREMENTS_RECHECK_SUBSCRIPTIONS = 2
} WBEM_PROVIDER_REQUIREMENTS_TYPE;

extern RPC_IF_HANDLE __MIDL_itf_wbemprov_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wbemprov_0000_v0_0_s_ifspec;

#ifndef __WbemProviders_v1_LIBRARY_DEFINED__
#define __WbemProviders_v1_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_WbemProviders_v1;

#ifndef __IWbemPropertyProvider_INTERFACE_DEFINED__
#define __IWbemPropertyProvider_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWbemPropertyProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("CE61E841-65BC-11d0-B6BD-00AA003240C7") IWbemPropertyProvider:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetProperty(long lFlags, const BSTR strLocale, const BSTR strClassMapping, const BSTR strInstMapping, const BSTR strPropMapping, VARIANT * pvValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE PutProperty(long lFlags, const BSTR strLocale, const BSTR strClassMapping, const BSTR strInstMapping, const BSTR strPropMapping, const VARIANT * pvValue) = 0;
};
#else /* C style interface */
typedef struct IWbemPropertyProviderVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IWbemPropertyProvider * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IWbemPropertyProvider * This);
    ULONG(STDMETHODCALLTYPE * Release) (IWbemPropertyProvider * This);
    HRESULT(STDMETHODCALLTYPE * GetProperty) (IWbemPropertyProvider * This, long lFlags, const BSTR strLocale, const BSTR strClassMapping, const BSTR strInstMapping, const BSTR strPropMapping, VARIANT * pvValue);
    HRESULT(STDMETHODCALLTYPE * PutProperty) (IWbemPropertyProvider * This, long lFlags, const BSTR strLocale, const BSTR strClassMapping, const BSTR strInstMapping, const BSTR strPropMapping, const VARIANT * pvValue);
    END_INTERFACE
} IWbemPropertyProviderVtbl;

interface IWbemPropertyProvider {
    CONST_VTBL struct IWbemPropertyProviderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemPropertyProvider_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IWbemPropertyProvider_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IWbemPropertyProvider_Release(This)  (This)->lpVtbl -> Release(This)
#define IWbemPropertyProvider_GetProperty(This,lFlags,strLocale,strClassMapping,strInstMapping,strPropMapping,pvValue)  (This)->lpVtbl -> GetProperty(This,lFlags,strLocale,strClassMapping,strInstMapping,strPropMapping,pvValue)
#define IWbemPropertyProvider_PutProperty(This,lFlags,strLocale,strClassMapping,strInstMapping,strPropMapping,pvValue)  (This)->lpVtbl -> PutProperty(This,lFlags,strLocale,strClassMapping,strInstMapping,strPropMapping,pvValue)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IWbemPropertyProvider_GetProperty_Proxy(IWbemPropertyProvider * This, long lFlags, const BSTR strLocale, const BSTR strClassMapping, const BSTR strInstMapping, const BSTR strPropMapping, VARIANT * pvValue);
void __RPC_STUB IWbemPropertyProvider_GetProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWbemPropertyProvider_PutProperty_Proxy(IWbemPropertyProvider * This, long lFlags, const BSTR strLocale, const BSTR strClassMapping, const BSTR strInstMapping, const BSTR strPropMapping, const VARIANT * pvValue);
void __RPC_STUB IWbemPropertyProvider_PutProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __IWbemPropertyProvider_INTERFACE_DEFINED__ */

#ifndef __IWbemUnboundObjectSink_INTERFACE_DEFINED__
#define __IWbemUnboundObjectSink_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWbemUnboundObjectSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("e246107b-b06e-11d0-ad61-00c04fd8fdff") IWbemUnboundObjectSink:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE IndicateToConsumer(IWbemClassObject * pLogicalConsumer, long lNumObjects, IWbemClassObject * *apObjects) = 0;

};
#else /* C style interface */
typedef struct IWbemUnboundObjectSinkVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IWbemUnboundObjectSink * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IWbemUnboundObjectSink * This);
    ULONG (STDMETHODCALLTYPE * Release) (IWbemUnboundObjectSink * This);
    HRESULT (STDMETHODCALLTYPE * IndicateToConsumer) (IWbemUnboundObjectSink * This, IWbemClassObject * pLogicalConsumer, long lNumObjects, IWbemClassObject * *apObjects);
    END_INTERFACE
} IWbemUnboundObjectSinkVtbl;

interface IWbemUnboundObjectSink {
    CONST_VTBL struct IWbemUnboundObjectSinkVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemUnboundObjectSink_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IWbemUnboundObjectSink_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IWbemUnboundObjectSink_Release(This)  (This)->lpVtbl -> Release(This)
#define IWbemUnboundObjectSink_IndicateToConsumer(This,pLogicalConsumer,lNumObjects,apObjects)  (This)->lpVtbl -> IndicateToConsumer(This,pLogicalConsumer,lNumObjects,apObjects)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IWbemUnboundObjectSink_IndicateToConsumer_Proxy(IWbemUnboundObjectSink * This, IWbemClassObject * pLogicalConsumer, long lNumObjects, IWbemClassObject * *apObjects);
void __RPC_STUB IWbemUnboundObjectSink_IndicateToConsumer_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __IWbemUnboundObjectSink_INTERFACE_DEFINED__ */

#ifndef __IWbemEventProvider_INTERFACE_DEFINED__
#define __IWbemEventProvider_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWbemEventProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("e245105b-b06e-11d0-ad61-00c04fd8fdff") IWbemEventProvider:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE ProvideEvents(IWbemObjectSink * pSink, long lFlags) = 0;
};
#else /* C style interface */
typedef struct IWbemEventProviderVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IWbemEventProvider * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IWbemEventProvider * This);
    ULONG(STDMETHODCALLTYPE * Release) (IWbemEventProvider * This);
    HRESULT(STDMETHODCALLTYPE * ProvideEvents) (IWbemEventProvider * This, IWbemObjectSink * pSink, long lFlags);
    END_INTERFACE
} IWbemEventProviderVtbl;

interface IWbemEventProvider {
    CONST_VTBL struct IWbemEventProviderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemEventProvider_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IWbemEventProvider_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IWbemEventProvider_Release(This)  (This)->lpVtbl -> Release(This)
#define IWbemEventProvider_ProvideEvents(This,pSink,lFlags)  (This)->lpVtbl -> ProvideEvents(This,pSink,lFlags)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IWbemEventProvider_ProvideEvents_Proxy(IWbemEventProvider * This, IWbemObjectSink * pSink, long lFlags);
void __RPC_STUB IWbemEventProvider_ProvideEvents_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __IWbemEventProvider_INTERFACE_DEFINED__ */

#ifndef __IWbemEventProviderQuerySink_INTERFACE_DEFINED__
#define __IWbemEventProviderQuerySink_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWbemEventProviderQuerySink;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("580acaf8-fa1c-11d0-ad72-00c04fd8fdff") IWbemEventProviderQuerySink:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE NewQuery(unsigned long dwId, WBEM_WSTR wszQueryLanguage, WBEM_WSTR wszQuery) = 0;
    virtual HRESULT STDMETHODCALLTYPE CancelQuery(unsigned long dwId) = 0;
};
#else /* C style interface */
typedef struct IWbemEventProviderQuerySinkVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IWbemEventProviderQuerySink * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IWbemEventProviderQuerySink * This);
    ULONG (STDMETHODCALLTYPE * Release) (IWbemEventProviderQuerySink * This);
    HRESULT (STDMETHODCALLTYPE * NewQuery) (IWbemEventProviderQuerySink * This, unsigned long dwId, WBEM_WSTR wszQueryLanguage, WBEM_WSTR wszQuery);
    HRESULT (STDMETHODCALLTYPE * CancelQuery) (IWbemEventProviderQuerySink * This, unsigned long dwId);
    END_INTERFACE
} IWbemEventProviderQuerySinkVtbl;

interface IWbemEventProviderQuerySink {
    CONST_VTBL struct IWbemEventProviderQuerySinkVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemEventProviderQuerySink_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IWbemEventProviderQuerySink_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IWbemEventProviderQuerySink_Release(This)  (This)->lpVtbl -> Release(This)
#define IWbemEventProviderQuerySink_NewQuery(This,dwId,wszQueryLanguage,wszQuery)  (This)->lpVtbl -> NewQuery(This,dwId,wszQueryLanguage,wszQuery)
#define IWbemEventProviderQuerySink_CancelQuery(This,dwId)  (This)->lpVtbl -> CancelQuery(This,dwId)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IWbemEventProviderQuerySink_NewQuery_Proxy(IWbemEventProviderQuerySink * This, unsigned long dwId, WBEM_WSTR wszQueryLanguage, WBEM_WSTR wszQuery);
void __RPC_STUB IWbemEventProviderQuerySink_NewQuery_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWbemEventProviderQuerySink_CancelQuery_Proxy(IWbemEventProviderQuerySink * This, unsigned long dwId);
void __RPC_STUB IWbemEventProviderQuerySink_CancelQuery_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __IWbemEventProviderQuerySink_INTERFACE_DEFINED__ */

#ifndef __IWbemEventConsumerProvider_INTERFACE_DEFINED__
#define __IWbemEventConsumerProvider_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWbemEventConsumerProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("e246107a-b06e-11d0-ad61-00c04fd8fdff") IWbemEventConsumerProvider:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE FindConsumer(IWbemClassObject * pLogicalConsumer, IWbemUnboundObjectSink * *ppConsumer) = 0;
};
#else /* C style interface */
typedef struct IWbemEventConsumerProviderVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IWbemEventConsumerProvider * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IWbemEventConsumerProvider * This);
    ULONG (STDMETHODCALLTYPE * Release) (IWbemEventConsumerProvider * This);
    HRESULT (STDMETHODCALLTYPE * FindConsumer) (IWbemEventConsumerProvider * This, IWbemClassObject * pLogicalConsumer, IWbemUnboundObjectSink * *ppConsumer);
    END_INTERFACE
} IWbemEventConsumerProviderVtbl;

interface IWbemEventConsumerProvider {
    CONST_VTBL struct IWbemEventConsumerProviderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemEventConsumerProvider_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IWbemEventConsumerProvider_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IWbemEventConsumerProvider_Release(This)  (This)->lpVtbl -> Release(This)
#define IWbemEventConsumerProvider_FindConsumer(This,pLogicalConsumer,ppConsumer)  (This)->lpVtbl -> FindConsumer(This,pLogicalConsumer,ppConsumer)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IWbemEventConsumerProvider_FindConsumer_Proxy(IWbemEventConsumerProvider * This, IWbemClassObject * pLogicalConsumer, IWbemUnboundObjectSink * *ppConsumer);
void __RPC_STUB IWbemEventConsumerProvider_FindConsumer_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __IWbemEventConsumerProvider_INTERFACE_DEFINED__ */

#ifndef __IWbemProviderInitSink_INTERFACE_DEFINED__
#define __IWbemProviderInitSink_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWbemProviderInitSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("1be41571-91dd-11d1-aeb2-00c04fb68820") IWbemProviderInitSink:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetStatus(LONG lStatus, LONG lFlags) = 0;
};
#else /* C style interface */
typedef struct IWbemProviderInitSinkVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IWbemProviderInitSink * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IWbemProviderInitSink * This);
    ULONG (STDMETHODCALLTYPE * Release) (IWbemProviderInitSink * This);
    HRESULT (STDMETHODCALLTYPE * SetStatus) (IWbemProviderInitSink * This, LONG lStatus, LONG lFlags);
    END_INTERFACE
} IWbemProviderInitSinkVtbl;

interface IWbemProviderInitSink {
    CONST_VTBL struct IWbemProviderInitSinkVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemProviderInitSink_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IWbemProviderInitSink_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IWbemProviderInitSink_Release(This)  (This)->lpVtbl -> Release(This)
#define IWbemProviderInitSink_SetStatus(This,lStatus,lFlags)  (This)->lpVtbl -> SetStatus(This,lStatus,lFlags)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IWbemProviderInitSink_SetStatus_Proxy(IWbemProviderInitSink * This, LONG lStatus, LONG lFlags);
void __RPC_STUB IWbemProviderInitSink_SetStatus_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __IWbemProviderInitSink_INTERFACE_DEFINED__ */

#ifndef __IWbemProviderInit_INTERFACE_DEFINED__
#define __IWbemProviderInit_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWbemProviderInit;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("1be41572-91dd-11d1-aeb2-00c04fb68820") IWbemProviderInit:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Initialize(LPWSTR wszUser, LONG lFlags, LPWSTR wszNamespace, LPWSTR wszLocale, IWbemServices * pNamespace, IWbemContext * pCtx, IWbemProviderInitSink * pInitSink) = 0;
};
#else /* C style interface */
typedef struct IWbemProviderInitVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IWbemProviderInit * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IWbemProviderInit * This);
    ULONG (STDMETHODCALLTYPE * Release) (IWbemProviderInit * This);
    HRESULT (STDMETHODCALLTYPE * Initialize) (IWbemProviderInit * This, LPWSTR wszUser, LONG lFlags, LPWSTR wszNamespace, LPWSTR wszLocale, IWbemServices * pNamespace, IWbemContext * pCtx, IWbemProviderInitSink * pInitSink);
    END_INTERFACE
} IWbemProviderInitVtbl;

interface IWbemProviderInit {
    CONST_VTBL struct IWbemProviderInitVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemProviderInit_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IWbemProviderInit_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IWbemProviderInit_Release(This)  (This)->lpVtbl -> Release(This)
#define IWbemProviderInit_Initialize(This,wszUser,lFlags,wszNamespace,wszLocale,pNamespace,pCtx,pInitSink)  (This)->lpVtbl -> Initialize(This,wszUser,lFlags,wszNamespace,wszLocale,pNamespace,pCtx,pInitSink)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IWbemProviderInit_Initialize_Proxy(IWbemProviderInit * This, LPWSTR wszUser, LONG lFlags, LPWSTR wszNamespace, LPWSTR wszLocale, IWbemServices * pNamespace, IWbemContext * pCtx, IWbemProviderInitSink * pInitSink);
void __RPC_STUB IWbemProviderInit_Initialize_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __IWbemProviderInit_INTERFACE_DEFINED__ */

#ifndef __IWbemHiPerfProvider_INTERFACE_DEFINED__
#define __IWbemHiPerfProvider_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWbemHiPerfProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("49353c93-516b-11d1-aea6-00c04fb68820") IWbemHiPerfProvider:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE QueryInstances(IWbemServices * pNamespace, WCHAR * wszClass, long lFlags, IWbemContext * pCtx, IWbemObjectSink * pSink) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateRefresher(IWbemServices * pNamespace, long lFlags, IWbemRefresher * *ppRefresher) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateRefreshableObject(IWbemServices * pNamespace, IWbemObjectAccess * pTemplate, IWbemRefresher * pRefresher, long lFlags, IWbemContext * pContext, IWbemObjectAccess * *ppRefreshable, long *plId) = 0;
    virtual HRESULT STDMETHODCALLTYPE StopRefreshing(IWbemRefresher * pRefresher, long lId, long lFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateRefreshableEnum(IWbemServices * pNamespace, LPCWSTR wszClass, IWbemRefresher * pRefresher, long lFlags, IWbemContext * pContext, IWbemHiPerfEnum * pHiPerfEnum, long *plId) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetObjects(IWbemServices * pNamespace, long lNumObjects, IWbemObjectAccess * *apObj, long lFlags, IWbemContext * pContext) = 0;
};
#else /* C style interface */
typedef struct IWbemHiPerfProviderVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IWbemHiPerfProvider * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IWbemHiPerfProvider * This);
    ULONG (STDMETHODCALLTYPE * Release) (IWbemHiPerfProvider * This);
    HRESULT (STDMETHODCALLTYPE * QueryInstances) (IWbemHiPerfProvider * This, IWbemServices * pNamespace, WCHAR * wszClass, long lFlags, IWbemContext * pCtx, IWbemObjectSink * pSink);
    HRESULT (STDMETHODCALLTYPE * CreateRefresher) (IWbemHiPerfProvider * This, IWbemServices * pNamespace, long lFlags, IWbemRefresher * *ppRefresher);
    HRESULT (STDMETHODCALLTYPE * CreateRefreshableObject) (IWbemHiPerfProvider * This, IWbemServices * pNamespace, IWbemObjectAccess * pTemplate, IWbemRefresher * pRefresher, long lFlags, IWbemContext * pContext, IWbemObjectAccess * *ppRefreshable, long *plId);
    HRESULT (STDMETHODCALLTYPE * StopRefreshing) (IWbemHiPerfProvider * This, IWbemRefresher * pRefresher, long lId, long lFlags);
    HRESULT (STDMETHODCALLTYPE * CreateRefreshableEnum) (IWbemHiPerfProvider * This, IWbemServices * pNamespace, LPCWSTR wszClass, IWbemRefresher * pRefresher, long lFlags, IWbemContext * pContext, IWbemHiPerfEnum * pHiPerfEnum, long *plId);
    HRESULT (STDMETHODCALLTYPE * GetObjects) (IWbemHiPerfProvider * This, IWbemServices * pNamespace, long lNumObjects, IWbemObjectAccess * *apObj, long lFlags, IWbemContext * pContext);
    END_INTERFACE
} IWbemHiPerfProviderVtbl;

interface IWbemHiPerfProvider {
    CONST_VTBL struct IWbemHiPerfProviderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemHiPerfProvider_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IWbemHiPerfProvider_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IWbemHiPerfProvider_Release(This)  (This)->lpVtbl -> Release(This)
#define IWbemHiPerfProvider_QueryInstances(This,pNamespace,wszClass,lFlags,pCtx,pSink)  (This)->lpVtbl -> QueryInstances(This,pNamespace,wszClass,lFlags,pCtx,pSink)
#define IWbemHiPerfProvider_CreateRefresher(This,pNamespace,lFlags,ppRefresher)  (This)->lpVtbl -> CreateRefresher(This,pNamespace,lFlags,ppRefresher)
#define IWbemHiPerfProvider_CreateRefreshableObject(This,pNamespace,pTemplate,pRefresher,lFlags,pContext,ppRefreshable,plId)  (This)->lpVtbl -> CreateRefreshableObject(This,pNamespace,pTemplate,pRefresher,lFlags,pContext,ppRefreshable,plId)
#define IWbemHiPerfProvider_StopRefreshing(This,pRefresher,lId,lFlags)  (This)->lpVtbl -> StopRefreshing(This,pRefresher,lId,lFlags)
#define IWbemHiPerfProvider_CreateRefreshableEnum(This,pNamespace,wszClass,pRefresher,lFlags,pContext,pHiPerfEnum,plId)  (This)->lpVtbl -> CreateRefreshableEnum(This,pNamespace,wszClass,pRefresher,lFlags,pContext,pHiPerfEnum,plId)
#define IWbemHiPerfProvider_GetObjects(This,pNamespace,lNumObjects,apObj,lFlags,pContext)  (This)->lpVtbl -> GetObjects(This,pNamespace,lNumObjects,apObj,lFlags,pContext)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IWbemHiPerfProvider_QueryInstances_Proxy(IWbemHiPerfProvider * This, IWbemServices * pNamespace, WCHAR * wszClass, long lFlags, IWbemContext * pCtx, IWbemObjectSink * pSink);
void __RPC_STUB IWbemHiPerfProvider_QueryInstances_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWbemHiPerfProvider_CreateRefresher_Proxy(IWbemHiPerfProvider * This, IWbemServices * pNamespace, long lFlags, IWbemRefresher * *ppRefresher);
void __RPC_STUB IWbemHiPerfProvider_CreateRefresher_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWbemHiPerfProvider_CreateRefreshableObject_Proxy(IWbemHiPerfProvider * This, IWbemServices * pNamespace, IWbemObjectAccess * pTemplate, IWbemRefresher * pRefresher, long lFlags, IWbemContext * pContext, IWbemObjectAccess * *ppRefreshable, long *plId);
void __RPC_STUB IWbemHiPerfProvider_CreateRefreshableObject_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWbemHiPerfProvider_StopRefreshing_Proxy(IWbemHiPerfProvider * This, IWbemRefresher * pRefresher, long lId, long lFlags);
void __RPC_STUB IWbemHiPerfProvider_StopRefreshing_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWbemHiPerfProvider_CreateRefreshableEnum_Proxy(IWbemHiPerfProvider * This, IWbemServices * pNamespace, LPCWSTR wszClass, IWbemRefresher * pRefresher, long lFlags, IWbemContext * pContext, IWbemHiPerfEnum * pHiPerfEnum, long *plId);
void __RPC_STUB IWbemHiPerfProvider_CreateRefreshableEnum_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWbemHiPerfProvider_GetObjects_Proxy(IWbemHiPerfProvider * This, IWbemServices * pNamespace, long lNumObjects, IWbemObjectAccess * *apObj, long lFlags, IWbemContext * pContext);
void __RPC_STUB IWbemHiPerfProvider_GetObjects_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __IWbemHiPerfProvider_INTERFACE_DEFINED__ */

EXTERN_C const CLSID CLSID_WbemAdministrativeLocator;

#ifdef __cplusplus
class DECLSPEC_UUID("cb8555cc-9128-11d1-ad9b-00c04fd8fdff") WbemAdministrativeLocator;
#endif

EXTERN_C const CLSID CLSID_WbemAuthenticatedLocator;

#ifdef __cplusplus
class DECLSPEC_UUID("cd184336-9128-11d1-ad9b-00c04fd8fdff") WbemAuthenticatedLocator;
#endif

EXTERN_C const CLSID CLSID_WbemUnauthenticatedLocator;

#ifdef __cplusplus
class DECLSPEC_UUID("443E7B79-DE31-11d2-B340-00104BCC4B4A") WbemUnauthenticatedLocator;
#endif
#endif /* __WbemProviders_v1_LIBRARY_DEFINED__ */

#ifndef __IWbemEventProviderSecurity_INTERFACE_DEFINED__
#define __IWbemEventProviderSecurity_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWbemEventProviderSecurity;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("631f7d96-d993-11d2-b339-00105a1f4aaf") IWbemEventProviderSecurity:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE AccessCheck(WBEM_CWSTR wszQueryLanguage, WBEM_CWSTR wszQuery, long lSidLength, const BYTE * pSid) = 0;
};
#else /* C style interface */
typedef struct IWbemEventProviderSecurityVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IWbemEventProviderSecurity * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IWbemEventProviderSecurity * This);
    ULONG (STDMETHODCALLTYPE * Release) (IWbemEventProviderSecurity * This);
    HRESULT (STDMETHODCALLTYPE * AccessCheck) (IWbemEventProviderSecurity * This, WBEM_CWSTR wszQueryLanguage, WBEM_CWSTR wszQuery, long lSidLength, const BYTE * pSid);
    END_INTERFACE
} IWbemEventProviderSecurityVtbl;

interface IWbemEventProviderSecurity {
    CONST_VTBL struct IWbemEventProviderSecurityVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemEventProviderSecurity_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IWbemEventProviderSecurity_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IWbemEventProviderSecurity_Release(This)  (This)->lpVtbl -> Release(This)
#define IWbemEventProviderSecurity_AccessCheck(This,wszQueryLanguage,wszQuery,lSidLength,pSid)  (This)->lpVtbl -> AccessCheck(This,wszQueryLanguage,wszQuery,lSidLength,pSid)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IWbemEventProviderSecurity_AccessCheck_Proxy(IWbemEventProviderSecurity * This, WBEM_CWSTR wszQueryLanguage, WBEM_CWSTR wszQuery, long lSidLength, const BYTE * pSid);
void __RPC_STUB IWbemEventProviderSecurity_AccessCheck_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __IWbemEventProviderSecurity_INTERFACE_DEFINED__ */

#ifndef __IWbemProviderIdentity_INTERFACE_DEFINED__
#define __IWbemProviderIdentity_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWbemProviderIdentity;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("631f7d97-d993-11d2-b339-00105a1f4aaf") IWbemProviderIdentity:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetRegistrationObject(long lFlags, IWbemClassObject * pProvReg) = 0;
};
#else /* C style interface */
typedef struct IWbemProviderIdentityVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IWbemProviderIdentity * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IWbemProviderIdentity * This);
    ULONG (STDMETHODCALLTYPE * Release) (IWbemProviderIdentity * This);
    HRESULT (STDMETHODCALLTYPE * SetRegistrationObject) (IWbemProviderIdentity * This, long lFlags, IWbemClassObject * pProvReg);
    END_INTERFACE
} IWbemProviderIdentityVtbl;

interface IWbemProviderIdentity {
    CONST_VTBL struct IWbemProviderIdentityVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWbemProviderIdentity_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IWbemProviderIdentity_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IWbemProviderIdentity_Release(This)  (This)->lpVtbl -> Release(This)
#define IWbemProviderIdentity_SetRegistrationObject(This,lFlags,pProvReg)  (This)->lpVtbl -> SetRegistrationObject(This,lFlags,pProvReg)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IWbemProviderIdentity_SetRegistrationObject_Proxy(IWbemProviderIdentity * This, long lFlags, IWbemClassObject * pProvReg);
void __RPC_STUB IWbemProviderIdentity_SetRegistrationObject_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __IWbemProviderIdentity_INTERFACE_DEFINED__ */

typedef enum tag_WBEM_EXTRA_RETURN_CODES {
    WBEM_S_INITIALIZED = 0,
    WBEM_S_LIMITED_SERVICE = 0x43001,
    WBEM_S_INDIRECTLY_UPDATED = WBEM_S_LIMITED_SERVICE+1,
    WBEM_E_RETRY_LATER = 0x80043001,
    WBEM_E_RESOURCE_CONTENTION = WBEM_E_RETRY_LATER+1
} WBEM_EXTRA_RETURN_CODES;

typedef enum tag_WBEM_PROVIDER_FLAGS {
    WBEM_FLAG_OWNER_UPDATE = 0x10000
} WBEM_PROVIDER_FLAGS;

extern RPC_IF_HANDLE __MIDL_itf_wbemprov_0155_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wbemprov_0155_v0_0_s_ifspec;


#ifdef __cplusplus
}
#endif

#endif /* _WBEMPROV_H */
