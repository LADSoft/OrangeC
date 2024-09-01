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

#ifndef _CERTMOD_H
#define _CERTMOD_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __ICertManageModule_FWD_DEFINED__
#define __ICertManageModule_FWD_DEFINED__
typedef interface ICertManageModule ICertManageModule;
#endif

#include "wtypes.h"
#include "oaidl.h"

#ifdef __cplusplus
extern "C" {
#endif

void *__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void*);

#define CMM_REFRESHONLY (0x1)

const WCHAR wszCMM_PROP_NAME[] = L"Name";
const WCHAR wszCMM_PROP_DESCRIPTION[] = L"Description";
const WCHAR wszCMM_PROP_COPYRIGHT[] = L"Copyright";
const WCHAR wszCMM_PROP_FILEVER[] = L"File Version";
const WCHAR wszCMM_PROP_PRODUCTVER[] = L"Product Version";
const WCHAR wszCMM_PROP_DISPLAY_HWND[] = L"HWND";

extern RPC_IF_HANDLE __MIDL_itf_certmod_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_certmod_0000_v0_0_s_ifspec;

#ifndef __ICertManageModule_INTERFACE_DEFINED__
#define __ICertManageModule_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICertManageModule;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("e7d7ad42-bd3d-11d1-9a4d-00c04fc297eb") ICertManageModule:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE GetProperty(const BSTR,BSTR,BSTR,LONG,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProperty(const BSTR,BSTR,BSTR,LONG,const VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Configure(const BSTR,BSTR,LONG) = 0;
};
#else /* C style interface */
typedef struct ICertManageModuleVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICertManageModule*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICertManageModule*);
    ULONG(STDMETHODCALLTYPE *Release)(ICertManageModule*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ICertManageModule*,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ICertManageModule*,UINT,LCID,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ICertManageModule*,REFIID,LPOLESTR*,UINT,LCID,DISPID*);
    HRESULT(STDMETHODCALLTYPE *Invoke)(ICertManageModule*,DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetProperty)(ICertManageModule*,const BSTR,BSTR,BSTR,LONG,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *SetProperty)(ICertManageModule*,const BSTR,BSTR,BSTR,LONG,const VARIANT*);
    HRESULT(STDMETHODCALLTYPE *Configure)(ICertManageModule*,const BSTR,BSTR,LONG);
    END_INTERFACE
} ICertManageModuleVtbl;

interface ICertManageModule {
    CONST_VTBL struct ICertManageModuleVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICertManageModule_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ICertManageModule_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ICertManageModule_Release(This)  (This)->lpVtbl->Release(This)
#define ICertManageModule_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define ICertManageModule_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define ICertManageModule_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define ICertManageModule_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define ICertManageModule_GetProperty(This,strConfig,strStorageLocation,strPropertyName,Flags,pvarProperty)  (This)->lpVtbl->GetProperty(This,strConfig,strStorageLocation,strPropertyName,Flags,pvarProperty)
#define ICertManageModule_SetProperty(This,strConfig,strStorageLocation,strPropertyName,Flags,pvarProperty)  (This)->lpVtbl->SetProperty(This,strConfig,strStorageLocation,strPropertyName,Flags,pvarProperty)
#define ICertManageModule_Configure(This,strConfig,strStorageLocation,Flags)  (This)->lpVtbl->Configure(This,strConfig,strStorageLocation,Flags)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE ICertManageModule_GetProperty_Proxy(ICertManageModule*,const BSTR,BSTR,BSTR,LONG,VARIANT*);
void __RPC_STUB ICertManageModule_GetProperty_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertManageModule_SetProperty_Proxy(ICertManageModule*,const BSTR,BSTR,BSTR,LONG,const VARIANT*);
void __RPC_STUB ICertManageModule_SetProperty_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertManageModule_Configure_Proxy(ICertManageModule*,const BSTR,BSTR,LONG);
void __RPC_STUB ICertManageModule_Configure_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __ICertManageModule_INTERFACE_DEFINED__ */

unsigned long __RPC_USER BSTR_UserSize(unsigned long*,unsigned long,BSTR*);
unsigned char *__RPC_USER BSTR_UserMarshal(unsigned long*, unsigned char*,BSTR*);
unsigned char *__RPC_USER BSTR_UserUnmarshal(unsigned long*, unsigned char*,BSTR*);
void __RPC_USER BSTR_UserFree(unsigned long*,BSTR*);

unsigned long __RPC_USER VARIANT_UserSize(unsigned long*,unsigned long,VARIANT*);
unsigned char *__RPC_USER VARIANT_UserMarshal(unsigned long*,unsigned char*,VARIANT*);
unsigned char *__RPC_USER VARIANT_UserUnmarshal(unsigned long*,unsigned char*,VARIANT*);
void __RPC_USER VARIANT_UserFree(unsigned long*,VARIANT*);

#ifdef __cplusplus
}
#endif

#endif /* _CERTMOD_H */
