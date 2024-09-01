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

#ifndef _DISPEX_H
#define _DISPEX_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IDispatchEx_FWD_DEFINED__
#define __IDispatchEx_FWD_DEFINED__
typedef interface IDispatchEx IDispatchEx;
#endif

#ifndef __IDispError_FWD_DEFINED__
#define __IDispError_FWD_DEFINED__
typedef interface IDispError IDispError;
#endif

#ifndef __IVariantChangeType_FWD_DEFINED__
#define __IVariantChangeType_FWD_DEFINED__
typedef interface IVariantChangeType IVariantChangeType;
#endif

#ifndef __IObjectIdentity_FWD_DEFINED__
#define __IObjectIdentity_FWD_DEFINED__
typedef interface IObjectIdentity IObjectIdentity;
#endif

#include "ocidl.h"

#ifdef __cplusplus
extern "C" {
#endif

void *__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void *);

#pragma comment(lib,"uuid.lib")

#ifndef DISPEX_H_
#define DISPEX_H_

#include "servprov.h"

#ifndef _NO_DISPATCHEX_GUIDS
DEFINE_GUID(IID_IDispatchEx,0xa6ef9860,0xc720,0x11d0,0x93,0x37,0x0,0xa0,0xc9,0xd,0xca,0xa9);
DEFINE_GUID(IID_IDispError,0xa6ef9861,0xc720,0x11d0,0x93,0x37,0x0,0xa0,0xc9,0xd,0xca,0xa9);
DEFINE_GUID(IID_IVariantChangeType,0xa6ef9862,0xc720,0x11d0,0x93,0x37,0x0,0xa0,0xc9,0xd,0xca,0xa9);
DEFINE_GUID(SID_VariantConversion,0x1f101481,0xbccd,0x11d0,0x93,0x36,0x0,0xa0,0xc9,0xd,0xca,0xa9);
DEFINE_GUID(SID_GetCaller,0x4717cc40,0xbcb9,0x11d0,0x93,0x36,0x0,0xa0,0xc9,0xd,0xca,0xa9);
DEFINE_GUID(IID_IObjectIdentity,0xca04b7e6,0xd21,0x11d1,0x8c,0xc5,0x0,0xc0,0x4f,0xc2,0xb0,0x85);
#define SID_GetScriptSite IID_IActiveScriptSite
#endif /* _NO_DISPATCHEX_GUIDS */

#ifndef _NO_DISPATCHEX_CONSTS
#define fdexNameCaseSensitive  0x00000001L
#define fdexNameEnsure  0x00000002L
#define fdexNameImplicit  0x00000004L
#define fdexNameCaseInsensitive  0x00000008L
#define fdexNameInternal  0x00000010L
#define fdexNameNoDynamicProperties 0x00000020L

#define fdexPropCanGet  0x00000001L
#define fdexPropCannotGet  0x00000002L
#define fdexPropCanPut  0x00000004L
#define fdexPropCannotPut  0x00000008L
#define fdexPropCanPutRef  0x00000010L
#define fdexPropCannotPutRef  0x00000020L
#define fdexPropNoSideEffects  0x00000040L
#define fdexPropDynamicType  0x00000080L
#define fdexPropCanCall  0x00000100L
#define fdexPropCannotCall  0x00000200L
#define fdexPropCanConstruct  0x00000400L
#define fdexPropCannotConstruct  0x00000800L
#define fdexPropCanSourceEvents  0x00001000L
#define fdexPropCannotSourceEvents  0x00002000L

#define grfdexPropCanAll  (fdexPropCanGet|fdexPropCanPut|fdexPropCanPutRef|fdexPropCanCall|fdexPropCanConstruct|fdexPropCanSourceEvents)
#define grfdexPropCannotAll  (fdexPropCannotGet|fdexPropCannotPut|fdexPropCannotPutRef|fdexPropCannotCall|fdexPropCannotConstruct|fdexPropCannotSourceEvents)
#define grfdexPropExtraAll  (fdexPropNoSideEffects|fdexPropDynamicType)
#define grfdexPropAll  (grfdexPropCanAll|grfdexPropCannotAll|grfdexPropExtraAll)

#define fdexEnumDefault  0x00000001L
#define fdexEnumAll  0x00000002L

#define DISPATCH_CONSTRUCT 0x4000

#define DISPID_THIS (-613)
#define DISPID_STARTENUM DISPID_UNKNOWN
#endif /* _NO_DISPATCHEX_CONSTS */

extern RPC_IF_HANDLE __MIDL_itf_dispex_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dispex_0000_v0_0_s_ifspec;

#ifndef __IDispatchEx_INTERFACE_DEFINED__
#define __IDispatchEx_INTERFACE_DEFINED__

EXTERN_C const IID IID_IDispatchEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A6EF9860-C720-11d0-9337-00A0C90DCAA9") IDispatchEx:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE GetDispID(BSTR,DWORD,DISPID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE InvokeEx(DISPID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,IServiceProvider*) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeleteMemberByName(BSTR,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeleteMemberByDispID(DISPID) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMemberProperties(DISPID,DWORD,DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMemberName(DISPID,BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNextDispID(DWORD,DISPID,DISPID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNameSpaceParent(IUnknown**) = 0;
};
#else
typedef struct IDispatchExVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IDispatchEx*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IDispatchEx*);
    ULONG (STDMETHODCALLTYPE *Release)(IDispatchEx*);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IDispatchEx*,UINT*);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IDispatchEx*,UINT,LCID,ITypeInfo**);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IDispatchEx*,REFIID,LPOLESTR*,UINT,LCID,DISPID*);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IDispatchEx*,DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
    HRESULT (STDMETHODCALLTYPE *GetDispID)(IDispatchEx*,BSTR,DWORD,DISPID*);
    HRESULT (STDMETHODCALLTYPE *InvokeEx)(IDispatchEx*,DISPID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,IServiceProvider*);
    HRESULT (STDMETHODCALLTYPE *DeleteMemberByName)(IDispatchEx*,BSTR,DWORD);
    HRESULT (STDMETHODCALLTYPE *DeleteMemberByDispID)(IDispatchEx*,DISPID);
    HRESULT (STDMETHODCALLTYPE *GetMemberProperties)(IDispatchEx*,DISPID,DWORD,DWORD*);
    HRESULT (STDMETHODCALLTYPE *GetMemberName)(IDispatchEx*,DISPID,BSTR*);
    HRESULT (STDMETHODCALLTYPE *GetNextDispID)(IDispatchEx*,DWORD,DISPID,DISPID*);
    HRESULT (STDMETHODCALLTYPE *GetNameSpaceParent)(IDispatchEx*,IUnknown**);
    END_INTERFACE
} IDispatchExVtbl;

interface IDispatchEx {
    CONST_VTBL struct IDispatchExVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDispatchEx_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IDispatchEx_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IDispatchEx_Release(This)  (This)->lpVtbl->Release(This)
#define IDispatchEx_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IDispatchEx_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IDispatchEx_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IDispatchEx_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IDispatchEx_GetDispID(This,bstrName,grfdex,pid)  (This)->lpVtbl->GetDispID(This,bstrName,grfdex,pid)
#define IDispatchEx_InvokeEx(This,id,lcid,wFlags,pdp,pvarRes,pei,pspCaller)  (This)->lpVtbl->InvokeEx(This,id,lcid,wFlags,pdp,pvarRes,pei,pspCaller)
#define IDispatchEx_DeleteMemberByName(This,bstrName,grfdex)  (This)->lpVtbl->DeleteMemberByName(This,bstrName,grfdex)
#define IDispatchEx_DeleteMemberByDispID(This,id)  (This)->lpVtbl->DeleteMemberByDispID(This,id)
#define IDispatchEx_GetMemberProperties(This,id,grfdexFetch,pgrfdex)  (This)->lpVtbl->GetMemberProperties(This,id,grfdexFetch,pgrfdex)
#define IDispatchEx_GetMemberName(This,id,pbstrName)  (This)->lpVtbl->GetMemberName(This,id,pbstrName)
#define IDispatchEx_GetNextDispID(This,grfdex,id,pid)  (This)->lpVtbl->GetNextDispID(This,grfdex,id,pid)
#define IDispatchEx_GetNameSpaceParent(This,ppunk)  (This)->lpVtbl->GetNameSpaceParent(This,ppunk)
#endif

#endif

HRESULT STDMETHODCALLTYPE IDispatchEx_GetDispID_Proxy(IDispatchEx*,BSTR,DWORD,DISPID*);
void __RPC_STUB IDispatchEx_GetDispID_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDispatchEx_RemoteInvokeEx_Proxy(IDispatchEx*,DISPID,LCID,DWORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,IServiceProvider*,UINT,UINT*,VARIANT*);
void __RPC_STUB IDispatchEx_RemoteInvokeEx_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDispatchEx_DeleteMemberByName_Proxy(IDispatchEx*,BSTR,DWORD);
void __RPC_STUB IDispatchEx_DeleteMemberByName_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDispatchEx_DeleteMemberByDispID_Proxy(IDispatchEx*,DISPID);
void __RPC_STUB IDispatchEx_DeleteMemberByDispID_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDispatchEx_GetMemberProperties_Proxy(IDispatchEx*,DISPID,DWORD,DWORD*);
void __RPC_STUB IDispatchEx_GetMemberProperties_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDispatchEx_GetMemberName_Proxy(IDispatchEx*,DISPID,BSTR*);
void __RPC_STUB IDispatchEx_GetMemberName_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDispatchEx_GetNextDispID_Proxy(IDispatchEx*,DWORD,DISPID,DISPID*);
void __RPC_STUB IDispatchEx_GetNextDispID_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDispatchEx_GetNameSpaceParent_Proxy(IDispatchEx*,IUnknown**);
void __RPC_STUB IDispatchEx_GetNameSpaceParent_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IDispError_INTERFACE_DEFINED__
#define __IDispError_INTERFACE_DEFINED__

EXTERN_C const IID IID_IDispError;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A6EF9861-C720-11d0-9337-00A0C90DCAA9") IDispError:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE QueryErrorInfo(GUID,IDispError**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNext(IDispError**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHresult(HRESULT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSource(BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHelpInfo(BSTR*,DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDescription(BSTR*) = 0;
};
#else
typedef struct IDispErrorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IDispError*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IDispError*);
    ULONG (STDMETHODCALLTYPE *Release)(IDispError*);
    HRESULT (STDMETHODCALLTYPE *QueryErrorInfo)(IDispError*,GUID,IDispError**);
    HRESULT (STDMETHODCALLTYPE *GetNext)(IDispError*,IDispError**);
    HRESULT (STDMETHODCALLTYPE *GetHresult)(IDispError*,HRESULT*);
    HRESULT (STDMETHODCALLTYPE *GetSource)(IDispError*,BSTR*);
    HRESULT (STDMETHODCALLTYPE *GetHelpInfo)(IDispError*,BSTR*,DWORD*);
    HRESULT (STDMETHODCALLTYPE *GetDescription)(IDispError*,BSTR*);
    END_INTERFACE
} IDispErrorVtbl;

interface IDispError {
    CONST_VTBL struct IDispErrorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDispError_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IDispError_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IDispError_Release(This)  (This)->lpVtbl->Release(This)
#define IDispError_QueryErrorInfo(This,guidErrorType,ppde)  (This)->lpVtbl->QueryErrorInfo(This,guidErrorType,ppde)
#define IDispError_GetNext(This,ppde)  (This)->lpVtbl->GetNext(This,ppde)
#define IDispError_GetHresult(This,phr)  (This)->lpVtbl->GetHresult(This,phr)
#define IDispError_GetSource(This,pbstrSource)  (This)->lpVtbl->GetSource(This,pbstrSource)
#define IDispError_GetHelpInfo(This,pbstrFileName,pdwContext)  (This)->lpVtbl->GetHelpInfo(This,pbstrFileName,pdwContext)
#define IDispError_GetDescription(This,pbstrDescription)  (This)->lpVtbl->GetDescription(This,pbstrDescription)
#endif

#endif

HRESULT STDMETHODCALLTYPE IDispError_QueryErrorInfo_Proxy(IDispError*,GUID,IDispError**);
void __RPC_STUB IDispError_QueryErrorInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDispError_GetNext_Proxy(IDispError*,IDispError**);
void __RPC_STUB IDispError_GetNext_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDispError_GetHresult_Proxy(IDispError*,HRESULT*);
void __RPC_STUB IDispError_GetHresult_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDispError_GetSource_Proxy(IDispError*,BSTR*);
void __RPC_STUB IDispError_GetSource_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDispError_GetHelpInfo_Proxy(IDispError*,BSTR*,DWORD*);
void __RPC_STUB IDispError_GetHelpInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDispError_GetDescription_Proxy(IDispError*,BSTR*);
void __RPC_STUB IDispError_GetDescription_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IVariantChangeType_INTERFACE_DEFINED__
#define __IVariantChangeType_INTERFACE_DEFINED__

EXTERN_C const IID IID_IVariantChangeType;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A6EF9862-C720-11d0-9337-00A0C90DCAA9") IVariantChangeType:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE ChangeType(VARIANT*,VARIANT*,LCID,VARTYPE) = 0;
};
#else
typedef struct IVariantChangeTypeVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IVariantChangeType*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IVariantChangeType*);
    ULONG (STDMETHODCALLTYPE *Release)(IVariantChangeType*);
    HRESULT (STDMETHODCALLTYPE *ChangeType)(IVariantChangeType*,VARIANT*,VARIANT*,LCID,VARTYPE vtNew);
    END_INTERFACE
} IVariantChangeTypeVtbl;

interface IVariantChangeType {
    CONST_VTBL struct IVariantChangeTypeVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IVariantChangeType_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IVariantChangeType_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IVariantChangeType_Release(This)  (This)->lpVtbl->Release(This)
#define IVariantChangeType_ChangeType(This,pvarDst,pvarSrc,lcid,vtNew)  (This)->lpVtbl->ChangeType(This,pvarDst,pvarSrc,lcid,vtNew)
#endif

#endif

HRESULT STDMETHODCALLTYPE IVariantChangeType_ChangeType_Proxy(IVariantChangeType*,VARIANT*,VARIANT*,LCID,VARTYPE);
void __RPC_STUB IVariantChangeType_ChangeType_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif

#ifndef __IObjectIdentity_INTERFACE_DEFINED__
#define __IObjectIdentity_INTERFACE_DEFINED__

EXTERN_C const IID IID_IObjectIdentity;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("CA04B7E6-0D21-11d1-8CC5-00C04FC2B085") IObjectIdentity:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE IsEqualObject(IUnknown*) = 0;
};
#else
typedef struct IObjectIdentityVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IObjectIdentity*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IObjectIdentity*);
    ULONG (STDMETHODCALLTYPE *Release)(IObjectIdentity*);
    HRESULT (STDMETHODCALLTYPE *IsEqualObject)(IObjectIdentity*,IUnknown*);
    END_INTERFACE
} IObjectIdentityVtbl;

interface IObjectIdentity {
    CONST_VTBL struct IObjectIdentityVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IObjectIdentity_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IObjectIdentity_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IObjectIdentity_Release(This)  (This)->lpVtbl->Release(This)
#define IObjectIdentity_IsEqualObject(This,punk)  (This)->lpVtbl->IsEqualObject(This,punk)
#endif

#endif

HRESULT STDMETHODCALLTYPE IObjectIdentity_IsEqualObject_Proxy(IObjectIdentity*,IUnknown*);
void __RPC_STUB IObjectIdentity_IsEqualObject_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif

#endif

extern RPC_IF_HANDLE __MIDL_itf_dispex_0257_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dispex_0257_v0_0_s_ifspec;

unsigned long __RPC_USER BSTR_UserSize(unsigned long*,unsigned long,BSTR*);
unsigned char *__RPC_USER BSTR_UserMarshal(unsigned long*,unsigned char *,BSTR*);
unsigned char *__RPC_USER BSTR_UserUnmarshal(unsigned long*,unsigned char *,BSTR*);
void __RPC_USER BSTR_UserFree(unsigned long*,BSTR*);

unsigned long __RPC_USER VARIANT_UserSize(unsigned long*,unsigned long,VARIANT*);
unsigned char *__RPC_USER VARIANT_UserMarshal(unsigned long*,unsigned char *,VARIANT*);
unsigned char *__RPC_USER VARIANT_UserUnmarshal(unsigned long*,unsigned char *,VARIANT*);
void __RPC_USER VARIANT_UserFree(unsigned long*,VARIANT*);

HRESULT STDMETHODCALLTYPE IDispatchEx_InvokeEx_Proxy(IDispatchEx*,DISPID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,IServiceProvider*);
HRESULT STDMETHODCALLTYPE IDispatchEx_InvokeEx_Stub(IDispatchEx*,DISPID,LCID,DWORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,IServiceProvider*,UINT,UINT*,VARIANT*);

#ifdef __cplusplus
}
#endif

#endif /* _DISPEX_H */
