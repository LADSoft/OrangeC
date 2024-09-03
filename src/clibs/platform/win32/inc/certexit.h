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

#ifndef _CERTEXIT_H
#define _CERTEXIT_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __ICertExit_FWD_DEFINED__
#define __ICertExit_FWD_DEFINED__
typedef interface ICertExit ICertExit;
#endif

#include "wtypes.h"
#include "certmod.h"

#ifdef __cplusplus
extern "C" {
#endif

void *__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void*);

#define EXITEVENT_INVALID  (0)
#define EXITEVENT_CERTISSUED  (0x1)
#define EXITEVENT_CERTPENDING  (0x2)
#define EXITEVENT_CERTDENIED  (0x4)
#define EXITEVENT_CERTREVOKED  (0x8)
#define EXITEVENT_CERTRETRIEVEPENDING  (0x10)
#define EXITEVENT_CRLISSUED  (0x20)
#define EXITEVENT_SHUTDOWN  (0x40)

extern RPC_IF_HANDLE __MIDL_itf_certexit_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_certexit_0000_v0_0_s_ifspec;

#ifndef __ICertExit_INTERFACE_DEFINED__
#define __ICertExit_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICertExit;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("e19ae1a0-7364-11d0-8816-00a0c903b83c") ICertExit:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE Initialize(const BSTR,LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Notify(LONG,LONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDescription(BSTR*) = 0;
};
#else /* C style interface */
typedef struct ICertExitVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICertExit*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICertExit*);
    ULONG(STDMETHODCALLTYPE *Release)(ICertExit*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ICertExit*,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ICertExit*,UINT,LCID,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ICertExit*,REFIID,LPOLESTR*,UINT,LCID,DISPID*);
    HRESULT(STDMETHODCALLTYPE *Invoke)(ICertExit*,DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
    HRESULT(STDMETHODCALLTYPE *Initialize)(ICertExit*,const BSTR,LONG*);
    HRESULT(STDMETHODCALLTYPE *Notify)(ICertExit*,LONG,LONG);
    HRESULT(STDMETHODCALLTYPE *GetDescription)(ICertExit*,BSTR*);
    END_INTERFACE
} ICertExitVtbl;

interface ICertExit {
    CONST_VTBL struct ICertExitVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICertExit_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ICertExit_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ICertExit_Release(This)  (This)->lpVtbl->Release(This)
#define ICertExit_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define ICertExit_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define ICertExit_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define ICertExit_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define ICertExit_Initialize(This,strConfig,pEventMask)  (This)->lpVtbl->Initialize(This,strConfig,pEventMask)
#define ICertExit_Notify(This,ExitEvent,Context)  (This)->lpVtbl->Notify(This,ExitEvent,Context)
#define ICertExit_GetDescription(This,pstrDescription)  (This)->lpVtbl->GetDescription(This,pstrDescription)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE ICertExit_Initialize_Proxy(ICertExit*,const BSTR,LONG*);
void __RPC_STUB ICertExit_Initialize_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertExit_Notify_Proxy(ICertExit*,LONG,LONG);
void __RPC_STUB ICertExit_Notify_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertExit_GetDescription_Proxy(ICertExit*,BSTR*);
void __RPC_STUB ICertExit_GetDescription_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __ICertExit_INTERFACE_DEFINED__ */

unsigned long __RPC_USER BSTR_UserSize(unsigned long*,unsigned long,BSTR*);
unsigned char *__RPC_USER BSTR_UserMarshal(unsigned long*,unsigned char*,BSTR*);
unsigned char *__RPC_USER BSTR_UserUnmarshal(unsigned long*,unsigned char*,BSTR*);
void __RPC_USER BSTR_UserFree(unsigned long*,BSTR*);

#ifdef __cplusplus
}
#endif

#endif /* _CERTEXIT_H */
