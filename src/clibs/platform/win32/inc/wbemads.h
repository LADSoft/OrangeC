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

#ifndef _WBEMADS_H
#define _WBEMADS_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IWMIExtension_FWD_DEFINED__
#define __IWMIExtension_FWD_DEFINED__
typedef interface IWMIExtension IWMIExtension;
#endif

#ifndef __WMIExtension_FWD_DEFINED__
#define __WMIExtension_FWD_DEFINED__
#ifdef __cplusplus
typedef class WMIExtension WMIExtension;
#else
typedef struct WMIExtension WMIExtension;
#endif
#endif

#ifndef __IWMIExtension_FWD_DEFINED__
#define __IWMIExtension_FWD_DEFINED__
typedef interface IWMIExtension IWMIExtension;
#endif

#include "oaidl.h"
#include "ocidl.h"
#include "wbemdisp.h"

#ifdef __cplusplus
extern "C" {
#endif

void *__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void *);

#ifndef __WMIEXTENSIONLib_LIBRARY_DEFINED__
#define __WMIEXTENSIONLib_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_WMIEXTENSIONLib;

#ifndef __IWMIExtension_INTERFACE_DEFINED__
#define __IWMIExtension_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWMIExtension;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("adc1f06e-5c7e-11d2-8b74-00104b2afb41") IWMIExtension:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_WMIObjectPath(BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetWMIObject(ISWbemObject**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetWMIServices(ISWbemServices**) = 0;
};
#else
typedef struct IWMIExtensionVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IWMIExtension*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWMIExtension*);
    ULONG(STDMETHODCALLTYPE *Release)(IWMIExtension*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IWMIExtension*,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IWMIExtension*,UINT,LCID,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IWMIExtension*,REFIID,LPOLESTR*,UINT,LCID,DISPID*);
    HRESULT(STDMETHODCALLTYPE *Invoke)(IWMIExtension*,DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
    HRESULT(STDMETHODCALLTYPE *get_WMIObjectPath)(IWMIExtension*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *GetWMIObject)(IWMIExtension*,ISWbemObject**);
    HRESULT(STDMETHODCALLTYPE *GetWMIServices)(IWMIExtension*,ISWbemServices**);
    END_INTERFACE
} IWMIExtensionVtbl;

interface IWMIExtension {
    CONST_VTBL struct IWMIExtensionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWMIExtension_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWMIExtension_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IWMIExtension_Release(This)  (This)->lpVtbl->Release(This)
#define IWMIExtension_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IWMIExtension_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IWMIExtension_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IWMIExtension_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IWMIExtension_get_WMIObjectPath(This,strWMIObjectPath)  (This)->lpVtbl->get_WMIObjectPath(This,strWMIObjectPath)
#define IWMIExtension_GetWMIObject(This,objWMIObject)  (This)->lpVtbl->GetWMIObject(This,objWMIObject)
#define IWMIExtension_GetWMIServices(This,objWMIServices)  (This)->lpVtbl->GetWMIServices(This,objWMIServices)
#endif

#endif

HRESULT STDMETHODCALLTYPE IWMIExtension_get_WMIObjectPath_Proxy(IWMIExtension*,BSTR*);
void __RPC_STUB IWMIExtension_get_WMIObjectPath_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IWMIExtension_GetWMIObject_Proxy(IWMIExtension*,ISWbemObject**);
void __RPC_STUB IWMIExtension_GetWMIObject_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IWMIExtension_GetWMIServices_Proxy(IWMIExtension*,ISWbemServices**);
void __RPC_STUB IWMIExtension_GetWMIServices_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

EXTERN_C const CLSID CLSID_WMIExtension;

#ifdef __cplusplus
class DECLSPEC_UUID("f0975afe-5c7f-11d2-8b74-00104b2afb41") WMIExtension;
#endif
#endif

unsigned long __RPC_USER BSTR_UserSize(unsigned long *,unsigned long,BSTR *);
unsigned char *__RPC_USER BSTR_UserMarshal(unsigned long *,unsigned char *,BSTR *);
unsigned char *__RPC_USER BSTR_UserUnmarshal(unsigned long *,unsigned char *,BSTR *);
void __RPC_USER BSTR_UserFree(unsigned long *,BSTR *);

#ifdef __cplusplus
}
#endif

#endif /* _WBEMADS_H */
