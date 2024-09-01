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

#ifndef _CERTPOL_H
#define _CERTPOL_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __ICertPolicy_FWD_DEFINED__
#define __ICertPolicy_FWD_DEFINED__
typedef interface ICertPolicy ICertPolicy;
#endif

#include "wtypes.h"
#include "oaidl.h"

#ifdef __cplusplus
extern "C" {
#endif

void *__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void*);

#ifndef __ICertPolicy_INTERFACE_DEFINED__
#define __ICertPolicy_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICertPolicy;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("38bb5a00-7636-11d0-b413-00a0c91bbf8c") ICertPolicy:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE Initialize(const BSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE VerifyRequest(const BSTR,LONG,LONG,LONG,LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDescription(BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE ShutDown(void) = 0;
};
#else /* C style interface */

typedef struct ICertPolicyVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICertPolicy*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICertPolicy*);
    ULONG(STDMETHODCALLTYPE *Release)(ICertPolicy*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ICertPolicy*,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ICertPolicy*,UINT,LCID,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ICertPolicy*,REFIID,LPOLESTR*,UINT,LCID,DISPID*);
    HRESULT(STDMETHODCALLTYPE *Invoke)(ICertPolicy*,DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
    HRESULT(STDMETHODCALLTYPE *Initialize)(ICertPolicy*,const BSTR);
    HRESULT(STDMETHODCALLTYPE *VerifyRequest)(ICertPolicy*,const BSTR,LONG,LONG,LONG,LONG*);
    HRESULT(STDMETHODCALLTYPE *GetDescription)(ICertPolicy*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *ShutDown)(ICertPolicy*);
    END_INTERFACE
} ICertPolicyVtbl;

interface ICertPolicy {
    CONST_VTBL struct ICertPolicyVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICertPolicy_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ICertPolicy_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ICertPolicy_Release(This)  (This)->lpVtbl->Release(This)
#define ICertPolicy_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define ICertPolicy_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define ICertPolicy_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define ICertPolicy_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define ICertPolicy_Initialize(This,strConfig)  (This)->lpVtbl->Initialize(This,strConfig)
#define ICertPolicy_VerifyRequest(This,strConfig,Context,bNewRequest,Flags,pDisposition)  (This)->lpVtbl->VerifyRequest(This,strConfig,Context,bNewRequest,Flags,pDisposition)
#define ICertPolicy_GetDescription(This,pstrDescription)  (This)->lpVtbl->GetDescription(This,pstrDescription)
#define ICertPolicy_ShutDown(This)  (This)->lpVtbl->ShutDown(This)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE ICertPolicy_Initialize_Proxy(ICertPolicy*,const BSTR);
void __RPC_STUB ICertPolicy_Initialize_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertPolicy_VerifyRequest_Proxy(ICertPolicy*,const BSTR,LONG,LONG,LONG,LONG*);
void __RPC_STUB ICertPolicy_VerifyRequest_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertPolicy_GetDescription_Proxy(ICertPolicy*,BSTR*);
void __RPC_STUB ICertPolicy_GetDescription_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertPolicy_ShutDown_Proxy(ICertPolicy*);
void __RPC_STUB ICertPolicy_ShutDown_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __ICertPolicy_INTERFACE_DEFINED__ */

unsigned long __RPC_USER BSTR_UserSize(unsigned long*,unsigned long,BSTR*);
unsigned char *__RPC_USER BSTR_UserMarshal(unsigned long*,unsigned char*,BSTR*);
unsigned char *__RPC_USER BSTR_UserUnmarshal(unsigned long*,unsigned char*,BSTR*);
void __RPC_USER BSTR_UserFree(unsigned long*,BSTR*);

#ifdef __cplusplus
}
#endif

#endif /* _CERTPOL_H */
