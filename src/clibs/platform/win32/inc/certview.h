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
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H */

#ifndef _CERTVIEW_H
#define _CERTVIEW_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IEnumCERTVIEWCOLUMN_FWD_DEFINED__
#define __IEnumCERTVIEWCOLUMN_FWD_DEFINED__
typedef interface IEnumCERTVIEWCOLUMN IEnumCERTVIEWCOLUMN;
#endif

#ifndef __IEnumCERTVIEWATTRIBUTE_FWD_DEFINED__
#define __IEnumCERTVIEWATTRIBUTE_FWD_DEFINED__
typedef interface IEnumCERTVIEWATTRIBUTE IEnumCERTVIEWATTRIBUTE;
#endif

#ifndef __IEnumCERTVIEWEXTENSION_FWD_DEFINED__
#define __IEnumCERTVIEWEXTENSION_FWD_DEFINED__
typedef interface IEnumCERTVIEWEXTENSION IEnumCERTVIEWEXTENSION;
#endif

#ifndef __IEnumCERTVIEWROW_FWD_DEFINED__
#define __IEnumCERTVIEWROW_FWD_DEFINED__
typedef interface IEnumCERTVIEWROW IEnumCERTVIEWROW;
#endif

#ifndef __ICertView_FWD_DEFINED__
#define __ICertView_FWD_DEFINED__
typedef interface ICertView ICertView;
#endif

#include "wtypes.h"
#include "oaidl.h"

#ifdef __cplusplus
extern "C" {
#endif

void *__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void*);

#define CV_OUT_BASE64HEADER  (0)
#define CV_OUT_BASE64  (0x1)
#define CV_OUT_BINARY  (0x2)
#define CV_OUT_BASE64REQUESTHEADER  (0x3)
#define CV_OUT_HEX  (0x4)
#define CV_OUT_HEXASCII (0x5)
#define CV_OUT_BASE64X509CRLHEADER  (0x9)
#define CV_OUT_HEXADDR  (0xa)
#define CV_OUT_HEXASCIIADDR  (0xb)
#define CV_OUT_ENCODEMASK  (0xff)
#define CVR_SEEK_NONE  (0)
#define CVR_SEEK_EQ  (0x1)
#define CVR_SEEK_LT  (0x2)
#define CVR_SEEK_LE  (0x4)
#define CVR_SEEK_GE  (0x8)
#define CVR_SEEK_GT  (0x10)
#define CVR_SEEK_MASK  (0xff)
#define CVR_SEEK_NODELTA  (0x1000)
#define CVR_SORT_NONE  (0)
#define CVR_SORT_ASCEND (0x1)
#define CVR_SORT_DESCEND  (0x2)
#define CV_COLUMN_QUEUE_DEFAULT (-1)
#define CV_COLUMN_LOG_DEFAULT  (-2)
#define CV_COLUMN_LOG_FAILED_DEFAULT  (-3)

extern RPC_IF_HANDLE __MIDL_itf_certview_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_certview_0000_v0_0_s_ifspec;

#ifndef __IEnumCERTVIEWCOLUMN_INTERFACE_DEFINED__
#define __IEnumCERTVIEWCOLUMN_INTERFACE_DEFINED__

EXTERN_C const IID IID_IEnumCERTVIEWCOLUMN;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("9c735be2-57a5-11d1-9bdb-00c04fb683fa") IEnumCERTVIEWCOLUMN:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE Next(LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetName(BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDisplayName(BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetType(LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsIndexed(LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMaxLength(LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetValue(LONG,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(LONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumCERTVIEWCOLUMN**) = 0;
};
#else /* C style interface */
typedef struct IEnumCERTVIEWCOLUMNVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IEnumCERTVIEWCOLUMN*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IEnumCERTVIEWCOLUMN*);
    ULONG(STDMETHODCALLTYPE *Release)(IEnumCERTVIEWCOLUMN*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IEnumCERTVIEWCOLUMN*,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IEnumCERTVIEWCOLUMN*,UINT,LCID,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IEnumCERTVIEWCOLUMN*,REFIID,LPOLESTR*,UINT,LCID,DISPID*);
    HRESULT(STDMETHODCALLTYPE *Invoke)(IEnumCERTVIEWCOLUMN*,DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
    HRESULT(STDMETHODCALLTYPE *Next)(IEnumCERTVIEWCOLUMN*,LONG*);
    HRESULT(STDMETHODCALLTYPE *GetName)(IEnumCERTVIEWCOLUMN*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *GetDisplayName)(IEnumCERTVIEWCOLUMN*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *GetType)(IEnumCERTVIEWCOLUMN*,LONG*);
    HRESULT(STDMETHODCALLTYPE *IsIndexed)(IEnumCERTVIEWCOLUMN*,LONG*);
    HRESULT(STDMETHODCALLTYPE *GetMaxLength)(IEnumCERTVIEWCOLUMN*,LONG*);
    HRESULT(STDMETHODCALLTYPE *GetValue)(IEnumCERTVIEWCOLUMN*,LONG,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *Skip)(IEnumCERTVIEWCOLUMN*,LONG);
    HRESULT(STDMETHODCALLTYPE *Reset)(IEnumCERTVIEWCOLUMN*);
    HRESULT(STDMETHODCALLTYPE *Clone)(IEnumCERTVIEWCOLUMN*,IEnumCERTVIEWCOLUMN**);
    END_INTERFACE
} IEnumCERTVIEWCOLUMNVtbl;

interface IEnumCERTVIEWCOLUMN {
    CONST_VTBL struct IEnumCERTVIEWCOLUMNVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEnumCERTVIEWCOLUMN_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IEnumCERTVIEWCOLUMN_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IEnumCERTVIEWCOLUMN_Release(This)  (This)->lpVtbl->Release(This)
#define IEnumCERTVIEWCOLUMN_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IEnumCERTVIEWCOLUMN_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IEnumCERTVIEWCOLUMN_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IEnumCERTVIEWCOLUMN_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IEnumCERTVIEWCOLUMN_Next(This,pIndex)  (This)->lpVtbl->Next(This,pIndex)
#define IEnumCERTVIEWCOLUMN_GetName(This,pstrOut)  (This)->lpVtbl->GetName(This,pstrOut)
#define IEnumCERTVIEWCOLUMN_GetDisplayName(This,pstrOut)  (This)->lpVtbl->GetDisplayName(This,pstrOut)
#define IEnumCERTVIEWCOLUMN_GetType(This,pType)  (This)->lpVtbl->GetType(This,pType)
#define IEnumCERTVIEWCOLUMN_IsIndexed(This,pIndexed)  (This)->lpVtbl->IsIndexed(This,pIndexed)
#define IEnumCERTVIEWCOLUMN_GetMaxLength(This,pMaxLength)  (This)->lpVtbl->GetMaxLength(This,pMaxLength)
#define IEnumCERTVIEWCOLUMN_GetValue(This,Flags,pvarValue)  (This)->lpVtbl->GetValue(This,Flags,pvarValue)
#define IEnumCERTVIEWCOLUMN_Skip(This,celt)  (This)->lpVtbl->Skip(This,celt)
#define IEnumCERTVIEWCOLUMN_Reset(This)  (This)->lpVtbl->Reset(This)
#define IEnumCERTVIEWCOLUMN_Clone(This,ppenum)  (This)->lpVtbl->Clone(This,ppenum)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_Next_Proxy(IEnumCERTVIEWCOLUMN*,LONG*);
void __RPC_STUB IEnumCERTVIEWCOLUMN_Next_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_GetName_Proxy(IEnumCERTVIEWCOLUMN*,BSTR*);
void __RPC_STUB IEnumCERTVIEWCOLUMN_GetName_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_GetDisplayName_Proxy(IEnumCERTVIEWCOLUMN*,BSTR*);
void __RPC_STUB IEnumCERTVIEWCOLUMN_GetDisplayName_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_GetType_Proxy(IEnumCERTVIEWCOLUMN*,LONG*);
void __RPC_STUB IEnumCERTVIEWCOLUMN_GetType_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_IsIndexed_Proxy(IEnumCERTVIEWCOLUMN*,LONG*);
void __RPC_STUB IEnumCERTVIEWCOLUMN_IsIndexed_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_GetMaxLength_Proxy(IEnumCERTVIEWCOLUMN*,LONG*);
void __RPC_STUB IEnumCERTVIEWCOLUMN_GetMaxLength_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_GetValue_Proxy(IEnumCERTVIEWCOLUMN*,LONG,VARIANT*);
void __RPC_STUB IEnumCERTVIEWCOLUMN_GetValue_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_Skip_Proxy(IEnumCERTVIEWCOLUMN*,LONG);
void __RPC_STUB IEnumCERTVIEWCOLUMN_Skip_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_Reset_Proxy(IEnumCERTVIEWCOLUMN*);
void __RPC_STUB IEnumCERTVIEWCOLUMN_Reset_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_Clone_Proxy(IEnumCERTVIEWCOLUMN*,IEnumCERTVIEWCOLUMN**);
void __RPC_STUB IEnumCERTVIEWCOLUMN_Clone_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IEnumCERTVIEWCOLUMN_INTERFACE_DEFINED__ */

#ifndef __IEnumCERTVIEWATTRIBUTE_INTERFACE_DEFINED__
#define __IEnumCERTVIEWATTRIBUTE_INTERFACE_DEFINED__

EXTERN_C const IID IID_IEnumCERTVIEWATTRIBUTE;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("e77db656-7653-11d1-9bde-00c04fb683fa") IEnumCERTVIEWATTRIBUTE : public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE Next(LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetName(BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetValue(BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(LONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumCERTVIEWATTRIBUTE**) = 0;
};
#else /* C style interface */
typedef struct IEnumCERTVIEWATTRIBUTEVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IEnumCERTVIEWATTRIBUTE*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IEnumCERTVIEWATTRIBUTE*);
    ULONG(STDMETHODCALLTYPE *Release)(IEnumCERTVIEWATTRIBUTE*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IEnumCERTVIEWATTRIBUTE*,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IEnumCERTVIEWATTRIBUTE*,UINT,LCID,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IEnumCERTVIEWATTRIBUTE*,REFIID,LPOLESTR*,UINT,LCID,DISPID*);
    HRESULT(STDMETHODCALLTYPE *Invoke)(IEnumCERTVIEWATTRIBUTE*,DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
    HRESULT(STDMETHODCALLTYPE *Next)(IEnumCERTVIEWATTRIBUTE*,LONG*);
    HRESULT(STDMETHODCALLTYPE *GetName)(IEnumCERTVIEWATTRIBUTE*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *GetValue)(IEnumCERTVIEWATTRIBUTE*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *Skip)(IEnumCERTVIEWATTRIBUTE*,LONG);
    HRESULT(STDMETHODCALLTYPE *Reset)(IEnumCERTVIEWATTRIBUTE*);
    HRESULT(STDMETHODCALLTYPE *Clone)(IEnumCERTVIEWATTRIBUTE*,IEnumCERTVIEWATTRIBUTE**);
    END_INTERFACE
} IEnumCERTVIEWATTRIBUTEVtbl;

interface IEnumCERTVIEWATTRIBUTE {
    CONST_VTBL struct IEnumCERTVIEWATTRIBUTEVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEnumCERTVIEWATTRIBUTE_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IEnumCERTVIEWATTRIBUTE_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IEnumCERTVIEWATTRIBUTE_Release(This)  (This)->lpVtbl->Release(This)
#define IEnumCERTVIEWATTRIBUTE_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IEnumCERTVIEWATTRIBUTE_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IEnumCERTVIEWATTRIBUTE_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IEnumCERTVIEWATTRIBUTE_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IEnumCERTVIEWATTRIBUTE_Next(This,pIndex)  (This)->lpVtbl->Next(This,pIndex)
#define IEnumCERTVIEWATTRIBUTE_GetName(This,pstrOut)  (This)->lpVtbl->GetName(This,pstrOut)
#define IEnumCERTVIEWATTRIBUTE_GetValue(This,pstrOut)  (This)->lpVtbl->GetValue(This,pstrOut)
#define IEnumCERTVIEWATTRIBUTE_Skip(This,celt)  (This)->lpVtbl->Skip(This,celt)
#define IEnumCERTVIEWATTRIBUTE_Reset(This)  (This)->lpVtbl->Reset(This)
#define IEnumCERTVIEWATTRIBUTE_Clone(This,ppenum)  (This)->lpVtbl->Clone(This,ppenum)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IEnumCERTVIEWATTRIBUTE_Next_Proxy(IEnumCERTVIEWATTRIBUTE*,LONG*);
void __RPC_STUB IEnumCERTVIEWATTRIBUTE_Next_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWATTRIBUTE_GetName_Proxy(IEnumCERTVIEWATTRIBUTE*,BSTR*);
void __RPC_STUB IEnumCERTVIEWATTRIBUTE_GetName_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWATTRIBUTE_GetValue_Proxy(IEnumCERTVIEWATTRIBUTE*,BSTR*);
void __RPC_STUB IEnumCERTVIEWATTRIBUTE_GetValue_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWATTRIBUTE_Skip_Proxy(IEnumCERTVIEWATTRIBUTE*,LONG);
void __RPC_STUB IEnumCERTVIEWATTRIBUTE_Skip_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWATTRIBUTE_Reset_Proxy(IEnumCERTVIEWATTRIBUTE*);
void __RPC_STUB IEnumCERTVIEWATTRIBUTE_Reset_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWATTRIBUTE_Clone_Proxy(IEnumCERTVIEWATTRIBUTE*,IEnumCERTVIEWATTRIBUTE**);
void __RPC_STUB IEnumCERTVIEWATTRIBUTE_Clone_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IEnumCERTVIEWATTRIBUTE_INTERFACE_DEFINED__ */

#ifndef __IEnumCERTVIEWEXTENSION_INTERFACE_DEFINED__
#define __IEnumCERTVIEWEXTENSION_INTERFACE_DEFINED__

EXTERN_C const IID IID_IEnumCERTVIEWEXTENSION;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("e7dd1466-7653-11d1-9bde-00c04fb683fa") IEnumCERTVIEWEXTENSION:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE Next(LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetName(BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFlags(LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetValue(LONG,LONG,VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(LONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumCERTVIEWEXTENSION**) = 0;
};
#else /* C style interface */
typedef struct IEnumCERTVIEWEXTENSIONVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IEnumCERTVIEWEXTENSION*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IEnumCERTVIEWEXTENSION*);
    ULONG(STDMETHODCALLTYPE *Release)(IEnumCERTVIEWEXTENSION*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IEnumCERTVIEWEXTENSION*,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IEnumCERTVIEWEXTENSION*,UINT,LCID,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IEnumCERTVIEWEXTENSION*,REFIID,LPOLESTR*,UINT,LCID,DISPID*);
    HRESULT(STDMETHODCALLTYPE *Invoke)(IEnumCERTVIEWEXTENSION*,DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
    HRESULT(STDMETHODCALLTYPE *Next)(IEnumCERTVIEWEXTENSION*,LONG*);
    HRESULT(STDMETHODCALLTYPE *GetName)(IEnumCERTVIEWEXTENSION*,BSTR*);
    HRESULT(STDMETHODCALLTYPE *GetFlags)(IEnumCERTVIEWEXTENSION*,LONG*);
    HRESULT(STDMETHODCALLTYPE *GetValue)(IEnumCERTVIEWEXTENSION*,LONG,LONG,VARIANT*);
    HRESULT(STDMETHODCALLTYPE *Skip)(IEnumCERTVIEWEXTENSION*,LONG);
    HRESULT(STDMETHODCALLTYPE *Reset)(IEnumCERTVIEWEXTENSION*);
    HRESULT(STDMETHODCALLTYPE *Clone)(IEnumCERTVIEWEXTENSION*,IEnumCERTVIEWEXTENSION**);
    END_INTERFACE
} IEnumCERTVIEWEXTENSIONVtbl;

interface IEnumCERTVIEWEXTENSION {
    CONST_VTBL struct IEnumCERTVIEWEXTENSIONVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEnumCERTVIEWEXTENSION_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IEnumCERTVIEWEXTENSION_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IEnumCERTVIEWEXTENSION_Release(This)  (This)->lpVtbl->Release(This)
#define IEnumCERTVIEWEXTENSION_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IEnumCERTVIEWEXTENSION_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IEnumCERTVIEWEXTENSION_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IEnumCERTVIEWEXTENSION_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IEnumCERTVIEWEXTENSION_Next(This,pIndex)  (This)->lpVtbl->Next(This,pIndex)
#define IEnumCERTVIEWEXTENSION_GetName(This,pstrOut)  (This)->lpVtbl->GetName(This,pstrOut)
#define IEnumCERTVIEWEXTENSION_GetFlags(This,pFlags)  (This)->lpVtbl->GetFlags(This,pFlags)
#define IEnumCERTVIEWEXTENSION_GetValue(This,Type,Flags,pvarValue)  (This)->lpVtbl->GetValue(This,Type,Flags,pvarValue)
#define IEnumCERTVIEWEXTENSION_Skip(This,celt)  (This)->lpVtbl->Skip(This,celt)
#define IEnumCERTVIEWEXTENSION_Reset(This)  (This)->lpVtbl->Reset(This)
#define IEnumCERTVIEWEXTENSION_Clone(This,ppenum)  (This)->lpVtbl->Clone(This,ppenum)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IEnumCERTVIEWEXTENSION_Next_Proxy(IEnumCERTVIEWEXTENSION*,LONG*);
void __RPC_STUB IEnumCERTVIEWEXTENSION_Next_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWEXTENSION_GetName_Proxy(IEnumCERTVIEWEXTENSION*,BSTR*);
void __RPC_STUB IEnumCERTVIEWEXTENSION_GetName_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWEXTENSION_GetFlags_Proxy(IEnumCERTVIEWEXTENSION*,LONG*);
void __RPC_STUB IEnumCERTVIEWEXTENSION_GetFlags_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWEXTENSION_GetValue_Proxy(IEnumCERTVIEWEXTENSION*,LONG,LONG,VARIANT*);
void __RPC_STUB IEnumCERTVIEWEXTENSION_GetValue_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWEXTENSION_Skip_Proxy(IEnumCERTVIEWEXTENSION*,LONG);
void __RPC_STUB IEnumCERTVIEWEXTENSION_Skip_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWEXTENSION_Reset_Proxy(IEnumCERTVIEWEXTENSION*);
void __RPC_STUB IEnumCERTVIEWEXTENSION_Reset_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWEXTENSION_Clone_Proxy(IEnumCERTVIEWEXTENSION*,IEnumCERTVIEWEXTENSION**);
void __RPC_STUB IEnumCERTVIEWEXTENSION_Clone_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IEnumCERTVIEWEXTENSION_INTERFACE_DEFINED__ */

#ifndef __IEnumCERTVIEWROW_INTERFACE_DEFINED__
#define __IEnumCERTVIEWROW_INTERFACE_DEFINED__

EXTERN_C const IID IID_IEnumCERTVIEWROW;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("d1157f4c-5af2-11d1-9bdc-00c04fb683fa") IEnumCERTVIEWROW:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE Next(LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumCertViewColumn(IEnumCERTVIEWCOLUMN**) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumCertViewAttribute(LONG,IEnumCERTVIEWATTRIBUTE**) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumCertViewExtension(LONG,IEnumCERTVIEWEXTENSION**) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(LONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumCERTVIEWROW**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMaxIndex(LONG*) = 0;
};
#else /* C style interface */
typedef struct IEnumCERTVIEWROWVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IEnumCERTVIEWROW*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IEnumCERTVIEWROW*);
    ULONG(STDMETHODCALLTYPE *Release)(IEnumCERTVIEWROW*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IEnumCERTVIEWROW*,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IEnumCERTVIEWROW*,UINT,LCID,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IEnumCERTVIEWROW*,REFIID,LPOLESTR*,UINT,LCID,DISPID*);
    HRESULT(STDMETHODCALLTYPE *Invoke)(IEnumCERTVIEWROW*,DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
    HRESULT(STDMETHODCALLTYPE *Next)(IEnumCERTVIEWROW*,LONG*);
    HRESULT(STDMETHODCALLTYPE *EnumCertViewColumn)(IEnumCERTVIEWROW*,IEnumCERTVIEWCOLUMN**);
    HRESULT(STDMETHODCALLTYPE *EnumCertViewAttribute)(IEnumCERTVIEWROW*,LONG,IEnumCERTVIEWATTRIBUTE**);
    HRESULT(STDMETHODCALLTYPE *EnumCertViewExtension)(IEnumCERTVIEWROW*,LONG,IEnumCERTVIEWEXTENSION**);
    HRESULT(STDMETHODCALLTYPE *Skip)(IEnumCERTVIEWROW*,LONG);
    HRESULT(STDMETHODCALLTYPE *Reset)(IEnumCERTVIEWROW*);
    HRESULT(STDMETHODCALLTYPE *Clone)(IEnumCERTVIEWROW*,IEnumCERTVIEWROW**);
    HRESULT(STDMETHODCALLTYPE *GetMaxIndex)(IEnumCERTVIEWROW*,LONG*);
    END_INTERFACE
} IEnumCERTVIEWROWVtbl;

interface IEnumCERTVIEWROW {
    CONST_VTBL struct IEnumCERTVIEWROWVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEnumCERTVIEWROW_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IEnumCERTVIEWROW_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IEnumCERTVIEWROW_Release(This)  (This)->lpVtbl->Release(This)
#define IEnumCERTVIEWROW_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IEnumCERTVIEWROW_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IEnumCERTVIEWROW_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IEnumCERTVIEWROW_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IEnumCERTVIEWROW_Next(This,pIndex)  (This)->lpVtbl->Next(This,pIndex)
#define IEnumCERTVIEWROW_EnumCertViewColumn(This,ppenum)  (This)->lpVtbl->EnumCertViewColumn(This,ppenum)
#define IEnumCERTVIEWROW_EnumCertViewAttribute(This,Flags,ppenum)  (This)->lpVtbl->EnumCertViewAttribute(This,Flags,ppenum)
#define IEnumCERTVIEWROW_EnumCertViewExtension(This,Flags,ppenum)  (This)->lpVtbl->EnumCertViewExtension(This,Flags,ppenum)
#define IEnumCERTVIEWROW_Skip(This,celt)  (This)->lpVtbl->Skip(This,celt)
#define IEnumCERTVIEWROW_Reset(This)  (This)->lpVtbl->Reset(This)
#define IEnumCERTVIEWROW_Clone(This,ppenum)  (This)->lpVtbl->Clone(This,ppenum)
#define IEnumCERTVIEWROW_GetMaxIndex(This,pIndex)  (This)->lpVtbl->GetMaxIndex(This,pIndex)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IEnumCERTVIEWROW_Next_Proxy(IEnumCERTVIEWROW*,LONG*);
void __RPC_STUB IEnumCERTVIEWROW_Next_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWROW_EnumCertViewColumn_Proxy(IEnumCERTVIEWROW*,IEnumCERTVIEWCOLUMN**);
void __RPC_STUB IEnumCERTVIEWROW_EnumCertViewColumn_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWROW_EnumCertViewAttribute_Proxy(IEnumCERTVIEWROW*,LONG,IEnumCERTVIEWATTRIBUTE**);
void __RPC_STUB IEnumCERTVIEWROW_EnumCertViewAttribute_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWROW_EnumCertViewExtension_Proxy(IEnumCERTVIEWROW*,LONG,IEnumCERTVIEWEXTENSION**);
void __RPC_STUB IEnumCERTVIEWROW_EnumCertViewExtension_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWROW_Skip_Proxy(IEnumCERTVIEWROW*,LONG);
void __RPC_STUB IEnumCERTVIEWROW_Skip_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWROW_Reset_Proxy(IEnumCERTVIEWROW*);
void __RPC_STUB IEnumCERTVIEWROW_Reset_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWROW_Clone_Proxy(IEnumCERTVIEWROW*,IEnumCERTVIEWROW**);
void __RPC_STUB IEnumCERTVIEWROW_Clone_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumCERTVIEWROW_GetMaxIndex_Proxy(IEnumCERTVIEWROW*,LONG*);
void __RPC_STUB IEnumCERTVIEWROW_GetMaxIndex_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IEnumCERTVIEWROW_INTERFACE_DEFINED__ */

#ifndef __ICertView_INTERFACE_DEFINED__
#define __ICertView_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICertView;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("c3fac344-1e84-11d1-9bd6-00c04fb683fa") ICertView:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE OpenConnection(const BSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumCertViewColumn(LONG,IEnumCERTVIEWCOLUMN**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetColumnCount(LONG,LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetColumnIndex(LONG,const BSTR,LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetResultColumnCount(LONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetResultColumn(LONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetRestriction(LONG,LONG,LONG,const VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE OpenView(IEnumCERTVIEWROW**) = 0;
};
#else /* C style interface */
typedef struct ICertViewVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICertView*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICertView*);
    ULONG(STDMETHODCALLTYPE *Release)(ICertView*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ICertView*,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ICertView*,UINT,LCID,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ICertView*,REFIID,LPOLESTR*,UINT,LCID,DISPID*);
    HRESULT(STDMETHODCALLTYPE *Invoke)(ICertView*,DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
    HRESULT(STDMETHODCALLTYPE *OpenConnection)(ICertView*,const BSTR);
    HRESULT(STDMETHODCALLTYPE *EnumCertViewColumn)(ICertView*,LONG,IEnumCERTVIEWCOLUMN**);
    HRESULT(STDMETHODCALLTYPE *GetColumnCount)(ICertView*,LONG,LONG*);
    HRESULT(STDMETHODCALLTYPE *GetColumnIndex)(ICertView*,LONG,const BSTR,LONG*);
    HRESULT(STDMETHODCALLTYPE *SetResultColumnCount)(ICertView*,LONG);
    HRESULT(STDMETHODCALLTYPE *SetResultColumn)(ICertView*,LONG);
    HRESULT(STDMETHODCALLTYPE *SetRestriction)(ICertView*,LONG,LONG,LONG,const VARIANT*);
    HRESULT(STDMETHODCALLTYPE *OpenView)(ICertView*,IEnumCERTVIEWROW**);
    END_INTERFACE
} ICertViewVtbl;

interface ICertView {
    CONST_VTBL struct ICertViewVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICertView_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ICertView_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ICertView_Release(This)  (This)->lpVtbl->Release(This)
#define ICertView_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define ICertView_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define ICertView_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define ICertView_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define ICertView_OpenConnection(This,strConfig)  (This)->lpVtbl->OpenConnection(This,strConfig)
#define ICertView_EnumCertViewColumn(This,fResultColumn,ppenum)  (This)->lpVtbl->EnumCertViewColumn(This,fResultColumn,ppenum)
#define ICertView_GetColumnCount(This,fResultColumn,pcColumn)  (This)->lpVtbl->GetColumnCount(This,fResultColumn,pcColumn)
#define ICertView_GetColumnIndex(This,fResultColumn,strColumnName,pColumnIndex)  (This)->lpVtbl->GetColumnIndex(This,fResultColumn,strColumnName,pColumnIndex)
#define ICertView_SetResultColumnCount(This,cResultColumn)  (This)->lpVtbl->SetResultColumnCount(This,cResultColumn)
#define ICertView_SetResultColumn(This,ColumnIndex)  (This)->lpVtbl->SetResultColumn(This,ColumnIndex)
#define ICertView_SetRestriction(This,ColumnIndex,SeekOperator,SortOrder,pvarValue)  (This)->lpVtbl->SetRestriction(This,ColumnIndex,SeekOperator,SortOrder,pvarValue)
#define ICertView_OpenView(This,ppenum)  (This)->lpVtbl->OpenView(This,ppenum)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE ICertView_OpenConnection_Proxy(ICertView*,const BSTR);
void __RPC_STUB ICertView_OpenConnection_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertView_EnumCertViewColumn_Proxy(ICertView*,LONG,IEnumCERTVIEWCOLUMN**);
void __RPC_STUB ICertView_EnumCertViewColumn_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertView_GetColumnCount_Proxy(ICertView*,LONG,LONG*);
void __RPC_STUB ICertView_GetColumnCount_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertView_GetColumnIndex_Proxy(ICertView*,LONG,const BSTR,LONG*);
void __RPC_STUB ICertView_GetColumnIndex_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertView_SetResultColumnCount_Proxy(ICertView*,LONG);
void __RPC_STUB ICertView_SetResultColumnCount_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertView_SetResultColumn_Proxy(ICertView*,LONG);
void __RPC_STUB ICertView_SetResultColumn_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertView_SetRestriction_Proxy(ICertView*,LONG,LONG,LONG,const VARIANT*);
void __RPC_STUB ICertView_SetRestriction_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertView_OpenView_Proxy(ICertView*,IEnumCERTVIEWROW**);
void __RPC_STUB ICertView_OpenView_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __ICertView_INTERFACE_DEFINED__ */

#ifdef __cplusplus
}
#endif

#endif
