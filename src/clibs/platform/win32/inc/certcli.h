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

#ifndef _CERTCLI_H
#define _CERTCLI_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* forward declarations */ 

#ifndef __ICertGetConfig_FWD_DEFINED__
#define __ICertGetConfig_FWD_DEFINED__
typedef interface ICertGetConfig ICertGetConfig;
#endif  /* __ICertGetConfig_FWD_DEFINED__ */


#ifndef __ICertConfig_FWD_DEFINED__
#define __ICertConfig_FWD_DEFINED__
typedef interface ICertConfig ICertConfig;
#endif  /* __ICertConfig_FWD_DEFINED__ */


#ifndef __ICertConfig2_FWD_DEFINED__
#define __ICertConfig2_FWD_DEFINED__
typedef interface ICertConfig2 ICertConfig2;
#endif  /* __ICertConfig2_FWD_DEFINED__ */


#ifndef __ICertRequest_FWD_DEFINED__
#define __ICertRequest_FWD_DEFINED__
typedef interface ICertRequest ICertRequest;
#endif  /* __ICertRequest_FWD_DEFINED__ */


#ifndef __ICertRequest2_FWD_DEFINED__
#define __ICertRequest2_FWD_DEFINED__
typedef interface ICertRequest2 ICertRequest2;
#endif  /* __ICertRequest2_FWD_DEFINED__ */


#ifndef __ICertRequest3_FWD_DEFINED__
#define __ICertRequest3_FWD_DEFINED__
typedef interface ICertRequest3 ICertRequest3;
#endif  /* __ICertRequest3_FWD_DEFINED__ */


#ifndef __CCertGetConfig_FWD_DEFINED__
#define __CCertGetConfig_FWD_DEFINED__

#ifdef __cplusplus
typedef class CCertGetConfig CCertGetConfig;
#else
typedef struct CCertGetConfig CCertGetConfig;
#endif /* __cplusplus */

#endif  /* __CCertGetConfig_FWD_DEFINED__ */


#ifndef __CCertConfig_FWD_DEFINED__
#define __CCertConfig_FWD_DEFINED__

#ifdef __cplusplus
typedef class CCertConfig CCertConfig;
#else
typedef struct CCertConfig CCertConfig;
#endif /* __cplusplus */

#endif  /* __CCertConfig_FWD_DEFINED__ */


#ifndef __CCertRequest_FWD_DEFINED__
#define __CCertRequest_FWD_DEFINED__

#ifdef __cplusplus
typedef class CCertRequest CCertRequest;
#else
typedef struct CCertRequest CCertRequest;
#endif /* __cplusplus */

#endif  /* __CCertRequest_FWD_DEFINED__ */


#ifndef __CCertServerPolicy_FWD_DEFINED__
#define __CCertServerPolicy_FWD_DEFINED__

#ifdef __cplusplus
typedef class CCertServerPolicy CCertServerPolicy;
#else
typedef struct CCertServerPolicy CCertServerPolicy;
#endif /* __cplusplus */

#endif  /* __CCertServerPolicy_FWD_DEFINED__ */


#ifndef __CCertServerExit_FWD_DEFINED__
#define __CCertServerExit_FWD_DEFINED__

#ifdef __cplusplus
typedef class CCertServerExit CCertServerExit;
#else
typedef struct CCertServerExit CCertServerExit;
#endif /* __cplusplus */

#endif  /* __CCertServerExit_FWD_DEFINED__ */


/* header files for imported files */
#include "wtypes.h"
#include "certif.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ICertGetConfig_INTERFACE_DEFINED__
#define __ICertGetConfig_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICertGetConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("c7ea09c0-ce17-11d0-8833-00a0c903b83c")ICertGetConfig:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetConfig(LONG Flags, BSTR * pstrOut) = 0;
};
#else /* C style interface */
typedef struct ICertGetConfigVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (ICertGetConfig * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICertGetConfig * This);
    ULONG(STDMETHODCALLTYPE *Release)(ICertGetConfig * This);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ICertGetConfig * This, UINT * pctinfo);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ICertGetConfig * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ICertGetConfig * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT(STDMETHODCALLTYPE *Invoke)(ICertGetConfig * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT(STDMETHODCALLTYPE *GetConfig)(ICertGetConfig * This, LONG Flags, BSTR * pstrOut);
    END_INTERFACE
} ICertGetConfigVtbl;

interface ICertGetConfig {
    CONST_VTBL struct ICertGetConfigVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICertGetConfig_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ICertGetConfig_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ICertGetConfig_Release(This)  ((This)->lpVtbl->Release(This))
#define ICertGetConfig_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ICertGetConfig_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ICertGetConfig_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ICertGetConfig_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ICertGetConfig_GetConfig(This,Flags,pstrOut)  ((This)->lpVtbl->GetConfig(This,Flags,pstrOut))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __ICertGetConfig_INTERFACE_DEFINED__ */

#ifndef __ICertConfig_INTERFACE_DEFINED__
#define __ICertConfig_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICertConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("372fce34-4324-11d0-8810-00a0c903b83c")ICertConfig:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Reset(LONG Index, LONG * pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE Next(LONG *pIndex) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetField(const BSTR strFieldName, BSTR *pstrOut) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetConfig(LONG Flags, BSTR *pstrOut) = 0;
};
#else /* C style interface */
typedef struct ICertConfigVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (ICertConfig * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICertConfig * This);
    ULONG(STDMETHODCALLTYPE *Release)(ICertConfig * This);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ICertConfig * This, UINT * pctinfo);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ICertConfig * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ICertConfig * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT(STDMETHODCALLTYPE *Invoke)(ICertConfig * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT(STDMETHODCALLTYPE *Reset)(ICertConfig * This, LONG Index, LONG * pCount);
    HRESULT(STDMETHODCALLTYPE *Next)(ICertConfig * This, LONG * pIndex);
    HRESULT(STDMETHODCALLTYPE *GetField)(ICertConfig * This, const BSTR strFieldName, BSTR * pstrOut);
    HRESULT(STDMETHODCALLTYPE *GetConfig)(ICertConfig * This, LONG Flags, BSTR * pstrOut);
    END_INTERFACE
} ICertConfigVtbl;

interface ICertConfig {
    CONST_VTBL struct ICertConfigVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICertConfig_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ICertConfig_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ICertConfig_Release(This)  ((This)->lpVtbl->Release(This))
#define ICertConfig_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ICertConfig_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ICertConfig_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ICertConfig_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ICertConfig_Reset(This,Index,pCount)  ((This)->lpVtbl->Reset(This,Index,pCount))
#define ICertConfig_Next(This,pIndex)  ((This)->lpVtbl->Next(This,pIndex))
#define ICertConfig_GetField(This,strFieldName,pstrOut)  ((This)->lpVtbl->GetField(This,strFieldName,pstrOut))
#define ICertConfig_GetConfig(This,Flags,pstrOut)  ((This)->lpVtbl->GetConfig(This,Flags,pstrOut))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __ICertConfig_INTERFACE_DEFINED__ */

#ifndef __ICertConfig2_INTERFACE_DEFINED__
#define __ICertConfig2_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICertConfig2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7a18edde-7e78-4163-8ded-78e2c9cee924")ICertConfig2:public ICertConfig
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetSharedFolder(const BSTR strSharedFolder) = 0;
};
#else /* C style interface */
typedef struct ICertConfig2Vtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (ICertConfig2 * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICertConfig2 * This);
    ULONG(STDMETHODCALLTYPE *Release)(ICertConfig2 * This);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ICertConfig2 * This, UINT * pctinfo);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ICertConfig2 * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ICertConfig2 * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT(STDMETHODCALLTYPE *Invoke)(ICertConfig2 * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT(STDMETHODCALLTYPE *Reset)(ICertConfig2 * This, LONG Index, LONG * pCount);
    HRESULT(STDMETHODCALLTYPE *Next)(ICertConfig2 * This, LONG * pIndex);
    HRESULT(STDMETHODCALLTYPE *GetField)(ICertConfig2 * This, const BSTR strFieldName, BSTR * pstrOut);
    HRESULT(STDMETHODCALLTYPE *GetConfig)(ICertConfig2 * This, LONG Flags, BSTR * pstrOut);
    HRESULT(STDMETHODCALLTYPE *SetSharedFolder)(ICertConfig2 * This, const BSTR strSharedFolder);
    END_INTERFACE
} ICertConfig2Vtbl;

interface ICertConfig2 {
    CONST_VTBL struct ICertConfig2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICertConfig2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ICertConfig2_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ICertConfig2_Release(This)  ((This)->lpVtbl->Release(This))
#define ICertConfig2_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ICertConfig2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ICertConfig2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ICertConfig2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ICertConfig2_Reset(This,Index,pCount)  ((This)->lpVtbl->Reset(This,Index,pCount))
#define ICertConfig2_Next(This,pIndex)  ((This)->lpVtbl->Next(This,pIndex))
#define ICertConfig2_GetField(This,strFieldName,pstrOut)  ((This)->lpVtbl->GetField(This,strFieldName,pstrOut))
#define ICertConfig2_GetConfig(This,Flags,pstrOut)  ((This)->lpVtbl->GetConfig(This,Flags,pstrOut))
#define ICertConfig2_SetSharedFolder(This,strSharedFolder)  ((This)->lpVtbl->SetSharedFolder(This,strSharedFolder))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __ICertConfig2_INTERFACE_DEFINED__ */

#ifndef __ICertRequest_INTERFACE_DEFINED__
#define __ICertRequest_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICertRequest;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("014e4840-5523-11d0-8812-00a0c903b83c")ICertRequest:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Submit(LONG Flags, const BSTR strRequest, const BSTR strAttributes, const BSTR strConfig, LONG *pDisposition) = 0;
    virtual HRESULT STDMETHODCALLTYPE RetrievePending(LONG RequestId, const BSTR strConfig, LONG *pDisposition) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLastStatus(LONG *pStatus) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRequestId(LONG *pRequestId) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDispositionMessage(BSTR *pstrDispositionMessage) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCACertificate(LONG fExchangeCertificate, const BSTR strConfig, LONG Flags, BSTR *pstrCertificate) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCertificate(LONG Flags, BSTR *pstrCertificate) = 0;
};
#else /* C style interface */
typedef struct ICertRequestVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (ICertRequest * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICertRequest * This);
    ULONG(STDMETHODCALLTYPE *Release)(ICertRequest * This);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ICertRequest * This, UINT * pctinfo);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ICertRequest * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ICertRequest * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT(STDMETHODCALLTYPE *Invoke)(ICertRequest * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT(STDMETHODCALLTYPE *Submit)(ICertRequest * This, LONG Flags, const BSTR strRequest, const BSTR strAttributes, const BSTR strConfig, LONG * pDisposition);
    HRESULT(STDMETHODCALLTYPE *RetrievePending)(ICertRequest * This, LONG RequestId, const BSTR strConfig, LONG * pDisposition);
    HRESULT(STDMETHODCALLTYPE *GetLastStatus)(ICertRequest * This, LONG * pStatus);
    HRESULT(STDMETHODCALLTYPE *GetRequestId)(ICertRequest * This, LONG * pRequestId);
    HRESULT(STDMETHODCALLTYPE *GetDispositionMessage)(ICertRequest * This, BSTR * pstrDispositionMessage);
    HRESULT(STDMETHODCALLTYPE *GetCACertificate)(ICertRequest * This, LONG fExchangeCertificate, const BSTR strConfig, LONG Flags, BSTR * pstrCertificate);
    HRESULT(STDMETHODCALLTYPE *GetCertificate)(ICertRequest * This, LONG Flags, BSTR * pstrCertificate);
    END_INTERFACE
} ICertRequestVtbl;

interface ICertRequest {
    CONST_VTBL struct ICertRequestVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICertRequest_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ICertRequest_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ICertRequest_Release(This)  ((This)->lpVtbl->Release(This))
#define ICertRequest_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ICertRequest_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ICertRequest_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ICertRequest_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ICertRequest_Submit(This,Flags,strRequest,strAttributes,strConfig,pDisposition)  ((This)->lpVtbl->Submit(This,Flags,strRequest,strAttributes,strConfig,pDisposition))
#define ICertRequest_RetrievePending(This,RequestId,strConfig,pDisposition)  ((This)->lpVtbl->RetrievePending(This,RequestId,strConfig,pDisposition))
#define ICertRequest_GetLastStatus(This,pStatus)  ((This)->lpVtbl->GetLastStatus(This,pStatus))
#define ICertRequest_GetRequestId(This,pRequestId)  ((This)->lpVtbl->GetRequestId(This,pRequestId))
#define ICertRequest_GetDispositionMessage(This,pstrDispositionMessage)  ((This)->lpVtbl->GetDispositionMessage(This,pstrDispositionMessage))
#define ICertRequest_GetCACertificate(This,fExchangeCertificate,strConfig,Flags,pstrCertificate)  ((This)->lpVtbl->GetCACertificate(This,fExchangeCertificate,strConfig,Flags,pstrCertificate))
#define ICertRequest_GetCertificate(This,Flags,pstrCertificate)  ((This)->lpVtbl->GetCertificate(This,Flags,pstrCertificate))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __ICertRequest_INTERFACE_DEFINED__ */

#ifndef __ICertRequest2_INTERFACE_DEFINED__
#define __ICertRequest2_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICertRequest2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a4772988-4a85-4fa9-824e-b5cf5c16405a")ICertRequest2:public ICertRequest
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetIssuedCertificate(const BSTR strConfig, LONG RequestId, const BSTR strSerialNumber, LONG *pDisposition) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetErrorMessageText(LONG hrMessage, LONG Flags, BSTR *pstrErrorMessageText) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCAProperty(const BSTR strConfig, LONG PropId, LONG PropIndex, LONG PropType, LONG Flags, VARIANT *pvarPropertyValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCAPropertyFlags(const BSTR strConfig, LONG PropId, LONG *pPropFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCAPropertyDisplayName(const BSTR strConfig, LONG PropId, BSTR *pstrDisplayName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFullResponseProperty(LONG PropId, LONG PropIndex, LONG PropType, LONG Flags, VARIANT *pvarPropertyValue) = 0;
};
#else /* C style interface */
typedef struct ICertRequest2Vtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (ICertRequest2 * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICertRequest2 * This);
    ULONG(STDMETHODCALLTYPE *Release)(ICertRequest2 * This);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ICertRequest2 * This, UINT * pctinfo);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ICertRequest2 * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ICertRequest2 * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT(STDMETHODCALLTYPE *Invoke)(ICertRequest2 * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT(STDMETHODCALLTYPE *Submit)(ICertRequest2 * This, LONG Flags, const BSTR strRequest, const BSTR strAttributes, const BSTR strConfig, LONG * pDisposition);
    HRESULT(STDMETHODCALLTYPE *RetrievePending)(ICertRequest2 * This, LONG RequestId, const BSTR strConfig, LONG * pDisposition);
    HRESULT(STDMETHODCALLTYPE *GetLastStatus)(ICertRequest2 * This, LONG * pStatus);
    HRESULT(STDMETHODCALLTYPE *GetRequestId)(ICertRequest2 * This, LONG * pRequestId);
    HRESULT(STDMETHODCALLTYPE *GetDispositionMessage)(ICertRequest2 * This, BSTR * pstrDispositionMessage);
    HRESULT(STDMETHODCALLTYPE *GetCACertificate)(ICertRequest2 * This, LONG fExchangeCertificate, const BSTR strConfig, LONG Flags, BSTR * pstrCertificate);
    HRESULT(STDMETHODCALLTYPE *GetCertificate)(ICertRequest2 * This, LONG Flags, BSTR * pstrCertificate);
    HRESULT(STDMETHODCALLTYPE *GetIssuedCertificate)(ICertRequest2 * This, const BSTR strConfig, LONG RequestId, const BSTR strSerialNumber, LONG * pDisposition);
    HRESULT(STDMETHODCALLTYPE *GetErrorMessageText)(ICertRequest2 * This, LONG hrMessage, LONG Flags, BSTR * pstrErrorMessageText);
    HRESULT(STDMETHODCALLTYPE *GetCAProperty)(ICertRequest2 * This, const BSTR strConfig, LONG PropId, LONG PropIndex, LONG PropType, LONG Flags, VARIANT * pvarPropertyValue);
    HRESULT(STDMETHODCALLTYPE *GetCAPropertyFlags)(ICertRequest2 * This, const BSTR strConfig, LONG PropId, LONG * pPropFlags);
    HRESULT(STDMETHODCALLTYPE *GetCAPropertyDisplayName)(ICertRequest2 * This, const BSTR strConfig, LONG PropId, BSTR * pstrDisplayName);
    HRESULT(STDMETHODCALLTYPE *GetFullResponseProperty)(ICertRequest2 * This, LONG PropId, LONG PropIndex, LONG PropType, LONG Flags, VARIANT * pvarPropertyValue);
    END_INTERFACE
} ICertRequest2Vtbl;

interface ICertRequest2 {
    CONST_VTBL struct ICertRequest2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICertRequest2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ICertRequest2_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ICertRequest2_Release(This)  ((This)->lpVtbl->Release(This))
#define ICertRequest2_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ICertRequest2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ICertRequest2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ICertRequest2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ICertRequest2_Submit(This,Flags,strRequest,strAttributes,strConfig,pDisposition)  ((This)->lpVtbl->Submit(This,Flags,strRequest,strAttributes,strConfig,pDisposition))
#define ICertRequest2_RetrievePending(This,RequestId,strConfig,pDisposition)  ((This)->lpVtbl->RetrievePending(This,RequestId,strConfig,pDisposition))
#define ICertRequest2_GetLastStatus(This,pStatus)  ((This)->lpVtbl->GetLastStatus(This,pStatus))
#define ICertRequest2_GetRequestId(This,pRequestId)  ((This)->lpVtbl->GetRequestId(This,pRequestId))
#define ICertRequest2_GetDispositionMessage(This,pstrDispositionMessage)  ((This)->lpVtbl->GetDispositionMessage(This,pstrDispositionMessage))
#define ICertRequest2_GetCACertificate(This,fExchangeCertificate,strConfig,Flags,pstrCertificate)  ((This)->lpVtbl->GetCACertificate(This,fExchangeCertificate,strConfig,Flags,pstrCertificate))
#define ICertRequest2_GetCertificate(This,Flags,pstrCertificate)  ((This)->lpVtbl->GetCertificate(This,Flags,pstrCertificate))
#define ICertRequest2_GetIssuedCertificate(This,strConfig,RequestId,strSerialNumber,pDisposition)  ((This)->lpVtbl->GetIssuedCertificate(This,strConfig,RequestId,strSerialNumber,pDisposition))
#define ICertRequest2_GetErrorMessageText(This,hrMessage,Flags,pstrErrorMessageText)  ((This)->lpVtbl->GetErrorMessageText(This,hrMessage,Flags,pstrErrorMessageText))
#define ICertRequest2_GetCAProperty(This,strConfig,PropId,PropIndex,PropType,Flags,pvarPropertyValue)  ((This)->lpVtbl->GetCAProperty(This,strConfig,PropId,PropIndex,PropType,Flags,pvarPropertyValue))
#define ICertRequest2_GetCAPropertyFlags(This,strConfig,PropId,pPropFlags)  ((This)->lpVtbl->GetCAPropertyFlags(This,strConfig,PropId,pPropFlags))
#define ICertRequest2_GetCAPropertyDisplayName(This,strConfig,PropId,pstrDisplayName)  ((This)->lpVtbl->GetCAPropertyDisplayName(This,strConfig,PropId,pstrDisplayName))
#define ICertRequest2_GetFullResponseProperty(This,PropId,PropIndex,PropType,Flags,pvarPropertyValue)  ((This)->lpVtbl->GetFullResponseProperty(This,PropId,PropIndex,PropType,Flags,pvarPropertyValue))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __ICertRequest2_INTERFACE_DEFINED__ */

typedef enum X509EnrollmentAuthFlags {
    X509AuthNone = 0,
    X509AuthAnonymous = 1,
    X509AuthKerberos = 2,
    X509AuthUsername = 4,
    X509AuthCertificate = 8
} X509EnrollmentAuthFlags;

extern RPC_IF_HANDLE __MIDL_itf_certcli_0000_0005_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_certcli_0000_0005_v0_0_s_ifspec;

#ifndef __ICertRequest3_INTERFACE_DEFINED__
#define __ICertRequest3_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICertRequest3;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("AFC8F92B-33A2-4861-BF36-2933B7CD67B3")ICertRequest3:public ICertRequest2
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetCredential(LONG hWnd, X509EnrollmentAuthFlags AuthType, BSTR strCredential, BSTR strPassword) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRequestIdString(BSTR *pstrRequestId) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetIssuedCertificate2(BSTR strConfig, BSTR strRequestId, BSTR strSerialNumber, LONG *pDisposition) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRefreshPolicy(VARIANT_BOOL *pValue) = 0;
};
#else /* C style interface */
typedef struct ICertRequest3Vtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (ICertRequest3 * This, REFIID riid,   void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(ICertRequest3 * This);
    ULONG(STDMETHODCALLTYPE *Release)(ICertRequest3 * This);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ICertRequest3 * This, UINT * pctinfo);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ICertRequest3 * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ICertRequest3 * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT(STDMETHODCALLTYPE *Invoke)(ICertRequest3 * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT(STDMETHODCALLTYPE *Submit)(ICertRequest3 * This, LONG Flags, const BSTR strRequest, const BSTR strAttributes, const BSTR strConfig, LONG * pDisposition);
    HRESULT(STDMETHODCALLTYPE *RetrievePending)(ICertRequest3 * This, LONG RequestId, const BSTR strConfig, LONG * pDisposition);
    HRESULT(STDMETHODCALLTYPE *GetLastStatus)(ICertRequest3 * This, LONG * pStatus);
    HRESULT(STDMETHODCALLTYPE *GetRequestId)(ICertRequest3 * This, LONG * pRequestId);
    HRESULT(STDMETHODCALLTYPE *GetDispositionMessage)(ICertRequest3 * This, BSTR * pstrDispositionMessage);
    HRESULT(STDMETHODCALLTYPE *GetCACertificate)(ICertRequest3 * This, LONG fExchangeCertificate, const BSTR strConfig, LONG Flags, BSTR * pstrCertificate);
    HRESULT(STDMETHODCALLTYPE *GetCertificate)(ICertRequest3 * This, LONG Flags, BSTR * pstrCertificate);
    HRESULT(STDMETHODCALLTYPE *GetIssuedCertificate)(ICertRequest3 * This, const BSTR strConfig, LONG RequestId, const BSTR strSerialNumber, LONG * pDisposition);
    HRESULT(STDMETHODCALLTYPE *GetErrorMessageText)(ICertRequest3 * This, LONG hrMessage, LONG Flags, BSTR * pstrErrorMessageText);
    HRESULT(STDMETHODCALLTYPE *GetCAProperty)(ICertRequest3 * This, const BSTR strConfig, LONG PropId, LONG PropIndex, LONG PropType, LONG Flags, VARIANT * pvarPropertyValue);
    HRESULT(STDMETHODCALLTYPE *GetCAPropertyFlags)(ICertRequest3 * This, const BSTR strConfig, LONG PropId, LONG * pPropFlags);
    HRESULT(STDMETHODCALLTYPE *GetCAPropertyDisplayName)(ICertRequest3 * This, const BSTR strConfig, LONG PropId, BSTR * pstrDisplayName);
    HRESULT(STDMETHODCALLTYPE *GetFullResponseProperty)(ICertRequest3 * This, LONG PropId, LONG PropIndex, LONG PropType, LONG Flags, VARIANT * pvarPropertyValue);
    HRESULT(STDMETHODCALLTYPE *SetCredential)(ICertRequest3 * This, LONG hWnd, X509EnrollmentAuthFlags AuthType, BSTR strCredential, BSTR strPassword);
    HRESULT(STDMETHODCALLTYPE *GetRequestIdString)(ICertRequest3 * This, BSTR * pstrRequestId);
    HRESULT(STDMETHODCALLTYPE *GetIssuedCertificate2)(ICertRequest3 * This, BSTR strConfig, BSTR strRequestId, BSTR strSerialNumber, LONG * pDisposition);
    HRESULT(STDMETHODCALLTYPE *GetRefreshPolicy)(ICertRequest3 * This, VARIANT_BOOL * pValue);
    END_INTERFACE
} ICertRequest3Vtbl;

interface ICertRequest3 {
    CONST_VTBL struct ICertRequest3Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICertRequest3_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ICertRequest3_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ICertRequest3_Release(This)  ((This)->lpVtbl->Release(This))
#define ICertRequest3_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ICertRequest3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ICertRequest3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ICertRequest3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ICertRequest3_Submit(This,Flags,strRequest,strAttributes,strConfig,pDisposition)  ((This)->lpVtbl->Submit(This,Flags,strRequest,strAttributes,strConfig,pDisposition))
#define ICertRequest3_RetrievePending(This,RequestId,strConfig,pDisposition)  ((This)->lpVtbl->RetrievePending(This,RequestId,strConfig,pDisposition))
#define ICertRequest3_GetLastStatus(This,pStatus)  ((This)->lpVtbl->GetLastStatus(This,pStatus))
#define ICertRequest3_GetRequestId(This,pRequestId)  ((This)->lpVtbl->GetRequestId(This,pRequestId))
#define ICertRequest3_GetDispositionMessage(This,pstrDispositionMessage)  ((This)->lpVtbl->GetDispositionMessage(This,pstrDispositionMessage))
#define ICertRequest3_GetCACertificate(This,fExchangeCertificate,strConfig,Flags,pstrCertificate)  ((This)->lpVtbl->GetCACertificate(This,fExchangeCertificate,strConfig,Flags,pstrCertificate))
#define ICertRequest3_GetCertificate(This,Flags,pstrCertificate)  ((This)->lpVtbl->GetCertificate(This,Flags,pstrCertificate))
#define ICertRequest3_GetIssuedCertificate(This,strConfig,RequestId,strSerialNumber,pDisposition)  ((This)->lpVtbl->GetIssuedCertificate(This,strConfig,RequestId,strSerialNumber,pDisposition))
#define ICertRequest3_GetErrorMessageText(This,hrMessage,Flags,pstrErrorMessageText)  ((This)->lpVtbl->GetErrorMessageText(This,hrMessage,Flags,pstrErrorMessageText))
#define ICertRequest3_GetCAProperty(This,strConfig,PropId,PropIndex,PropType,Flags,pvarPropertyValue)  ((This)->lpVtbl->GetCAProperty(This,strConfig,PropId,PropIndex,PropType,Flags,pvarPropertyValue))
#define ICertRequest3_GetCAPropertyFlags(This,strConfig,PropId,pPropFlags)  ((This)->lpVtbl->GetCAPropertyFlags(This,strConfig,PropId,pPropFlags))
#define ICertRequest3_GetCAPropertyDisplayName(This,strConfig,PropId,pstrDisplayName)  ((This)->lpVtbl->GetCAPropertyDisplayName(This,strConfig,PropId,pstrDisplayName))
#define ICertRequest3_GetFullResponseProperty(This,PropId,PropIndex,PropType,Flags,pvarPropertyValue)  ((This)->lpVtbl->GetFullResponseProperty(This,PropId,PropIndex,PropType,Flags,pvarPropertyValue))
#define ICertRequest3_SetCredential(This,hWnd,AuthType,strCredential,strPassword)  ((This)->lpVtbl->SetCredential(This,hWnd,AuthType,strCredential,strPassword))
#define ICertRequest3_GetRequestIdString(This,pstrRequestId)  ((This)->lpVtbl->GetRequestIdString(This,pstrRequestId))
#define ICertRequest3_GetIssuedCertificate2(This,strConfig,strRequestId,strSerialNumber,pDisposition)  ((This)->lpVtbl->GetIssuedCertificate2(This,strConfig,strRequestId,strSerialNumber,pDisposition))
#define ICertRequest3_GetRefreshPolicy(This,pValue)  ((This)->lpVtbl->GetRefreshPolicy(This,pValue))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __ICertRequest3_INTERFACE_DEFINED__ */

#ifndef __CERTCLIENTLib_LIBRARY_DEFINED__
#define __CERTCLIENTLib_LIBRARY_DEFINED__

#define wszCONFIG_COMMONNAME  L"CommonName"
#define wszCONFIG_ORGUNIT  L"OrgUnit"
#define wszCONFIG_ORGANIZATION  L"Organization"
#define wszCONFIG_LOCALITY  L"Locality"
#define wszCONFIG_STATE  L"State"
#define wszCONFIG_COUNTRY  L"Country"
#define wszCONFIG_CONFIG  L"Config"
#define wszCONFIG_EXCHANGECERTIFICATE   L"ExchangeCertificate"
#define wszCONFIG_SIGNATURECERTIFICATE  L"SignatureCertificate"
#define wszCONFIG_DESCRIPTION  L"Description"
#define wszCONFIG_COMMENT  L"Comment"
#define wszCONFIG_SERVER  L"Server"
#define wszCONFIG_AUTHORITY  L"Authority"
#define wszCONFIG_SANITIZEDNAME  L"SanitizedName"
#define wszCONFIG_SHORTNAME  L"ShortName"
#define wszCONFIG_SANITIZEDSHORTNAME    L"SanitizedShortName"
#define wszCONFIG_FLAGS  L"Flags"
#define wszCONFIG_WEBENROLLMENTSERVERS  L"WebEnrollmentServers"
#define CAIF_DSENTRY    (0x1 )

#define CAIF_SHAREDFOLDERENTRY  (0x2)
#define CAIF_REGISTRY  (0x4)
#define CAIF_LOCAL  (0x8)
#define CAIF_REGISTRYPARENT (0x10)

#define CR_IN_BASE64HEADER  (0)
#define CR_IN_BASE64  (0x1)
#define CR_IN_BINARY  (0x2)
#define CR_IN_ENCODEANY (0xff)
#define CR_IN_ENCODEMASK  (0xff)
#define CR_IN_FORMATANY (0)
#define CR_IN_PKCS10  (0x100)
#define CR_IN_KEYGEN  (0x200)
#define CR_IN_PKCS7 (0x300)
#define CR_IN_CMC  (0x400)
#define CR_IN_FORMATMASK  (0xff00)
#define CR_IN_RPC  (0x20000)
#define CR_IN_FULLRESPONSE  (0x40000)
#define CR_IN_CRLS  (0x80000)
#define CR_IN_MACHINE   (0x100000)
#define CR_IN_ROBO  (0x200000)
#define CR_IN_CLIENTIDNONE  (0x400000)
#define CR_IN_CONNECTONLY  (0x800000)
#define CR_IN_CLIENTFLAGSMASK  (((((CR_IN_ENCODEMASK|CR_IN_RPC)|CR_IN_MACHINE)|CR_IN_CLIENTIDNONE)|CR_IN_CONNECTONLY))

#define CC_DEFAULTCONFIG    (0)
#define CC_UIPICKCONFIG (0x1)
#define CC_FIRSTCONFIG  (0x2)
#define CC_LOCALCONFIG  (0x3)
#define CC_LOCALACTIVECONFIG    (0x4)
#define CC_UIPICKCONFIGSKIPLOCALCA  (0x5)

#define CR_DISP_INCOMPLETE  (0)
#define CR_DISP_ERROR   (0x1)
#define CR_DISP_DENIED  (0x2)
#define CR_DISP_ISSUED  (0x3)
#define CR_DISP_ISSUED_OUT_OF_BAND  (0x4)
#define CR_DISP_UNDER_SUBMISSION    (0x5)
#define CR_DISP_REVOKED (0x6)

#define CR_OUT_BASE64HEADER (0)
#define CR_OUT_BASE64   (0x1)
#define CR_OUT_BINARY   (0x2)
#define CR_OUT_BASE64REQUESTHEADER  (0x3)
#define CR_OUT_HEX  (0x4)
#define CR_OUT_HEXASCII (0x5)
#define CR_OUT_BASE64X509CRLHEADER  (0x9)
#define CR_OUT_HEXADDR  (0xa)
#define CR_OUT_HEXASCIIADDR (0xb)
#define CR_OUT_HEXRAW   (0xc)
#define CR_OUT_ENCODEMASK   (0xff)
#define CR_OUT_CHAIN    (0x100)
#define CR_OUT_CRLS (0x200)
#define CR_OUT_NOCRLF   (0x40000000)
#define CR_OUT_NOCR (0x80000000)

#define CR_GEMT_HRESULT_STRING  (0x1)

#define CR_PROP_NONE  0
#define CR_PROP_FILEVERSION  1
#define CR_PROP_PRODUCTVERSION  2
#define CR_PROP_EXITCOUNT  3
#define CR_PROP_EXITDESCRIPTION  4
#define CR_PROP_POLICYDESCRIPTION  5
#define CR_PROP_CANAME  6
#define CR_PROP_SANITIZEDCANAME  7
#define CR_PROP_SHAREDFOLDER  8
#define CR_PROP_PARENTCA  9
#define CR_PROP_CATYPE  10
#define CR_PROP_CASIGCERTCOUNT  11
#define CR_PROP_CASIGCERT  12
#define CR_PROP_CASIGCERTCHAIN  13
#define CR_PROP_CAXCHGCERTCOUNT   14
#define CR_PROP_CAXCHGCERT  15
#define CR_PROP_CAXCHGCERTCHAIN   16
#define CR_PROP_BASECRL  17
#define CR_PROP_DELTACRL  18
#define CR_PROP_CACERTSTATE  19
#define CR_PROP_CRLSTATE  20
#define CR_PROP_CAPROPIDMAX  21
#define CR_PROP_DNSNAME  22
#define CR_PROP_ROLESEPARATIONENABLED 23
#define CR_PROP_KRACERTUSEDCOUNT  24
#define CR_PROP_KRACERTCOUNT  25
#define CR_PROP_KRACERT  26
#define CR_PROP_KRACERTSTATE  27
#define CR_PROP_ADVANCEDSERVER  28
#define CR_PROP_TEMPLATES  29
#define CR_PROP_BASECRLPUBLISHSTATUS 30
#define CR_PROP_DELTACRLPUBLISHSTATUS 31
#define CR_PROP_CASIGCERTCRLCHAIN 32
#define CR_PROP_CAXCHGCERTCRLCHAIN 33
#define CR_PROP_CACERTSTATUSCODE  34
#define CR_PROP_CAFORWARDCROSSCERT 35
#define CR_PROP_CABACKWARDCROSSCERT 36
#define CR_PROP_CAFORWARDCROSSCERTSTATE 37
#define CR_PROP_CABACKWARDCROSSCERTSTATE 38
#define CR_PROP_CACERTVERSION  39
#define CR_PROP_SANITIZEDCASHORTNAME 40
#define CR_PROP_CERTCDPURLS 41
#define CR_PROP_CERTAIAURLS 42
#define CR_PROP_CERTAIAOCSPURLS 43
#define CR_PROP_LOCALENAME 44

#define FR_PROP_NONE  0
#define FR_PROP_FULLRESPONSE  1
#define FR_PROP_STATUSINFOCOUNT  2
#define FR_PROP_BODYPARTSTRING  3
#define FR_PROP_STATUS  4
#define FR_PROP_STATUSSTRING  5
#define FR_PROP_OTHERINFOCHOICE  6
#define FR_PROP_FAILINFO  7
#define FR_PROP_PENDINFOTOKEN  8
#define FR_PROP_PENDINFOTIME  9
#define FR_PROP_ISSUEDCERTIFICATEHASH  10
#define FR_PROP_ISSUEDCERTIFICATE  11
#define FR_PROP_ISSUEDCERTIFICATECHAIN 12
#define FR_PROP_ISSUEDCERTIFICATECRLCHAIN 13
#define FR_PROP_ENCRYPTEDKEYHASH  14
#define FR_PROP_FULLRESPONSENOPKCS7   15

EXTERN_C const IID LIBID_CERTCLIENTLib;

EXTERN_C const CLSID CLSID_CCertGetConfig;

#ifdef __cplusplus
class DECLSPEC_UUID("c6cc49b0-ce17-11d0-8833-00a0c903b83c") CCertGetConfig;
#endif

EXTERN_C const CLSID CLSID_CCertConfig;

#ifdef __cplusplus
class DECLSPEC_UUID("372fce38-4324-11d0-8810-00a0c903b83c") CCertConfig;
#endif

EXTERN_C const CLSID CLSID_CCertRequest;

#ifdef __cplusplus
class DECLSPEC_UUID("98aff3f0-5524-11d0-8812-00a0c903b83c") CCertRequest;
#endif

EXTERN_C const CLSID CLSID_CCertServerPolicy;

#ifdef __cplusplus
class DECLSPEC_UUID("aa000926-ffbe-11cf-8800-00a0c903b83c") CCertServerPolicy;
#endif

EXTERN_C const CLSID CLSID_CCertServerExit;

#ifdef __cplusplus
class DECLSPEC_UUID("4c4a5e40-732c-11d0-8816-00a0c903b83c") CCertServerExit;
#endif
#endif /* __CERTCLIENTLib_LIBRARY_DEFINED__ */

unsigned long __RPC_USER BSTR_UserSize(unsigned long *, unsigned long, BSTR *);
unsigned char *__RPC_USER BSTR_UserMarshal(unsigned long *, unsigned char *, BSTR *);
unsigned char *__RPC_USER BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR *);
void __RPC_USER BSTR_UserFree(unsigned long *, BSTR *);

unsigned long __RPC_USER VARIANT_UserSize(unsigned long *, unsigned long, VARIANT *);
unsigned char *__RPC_USER VARIANT_UserMarshal(unsigned long *, unsigned char *, VARIANT *);
unsigned char *__RPC_USER VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT *);
void __RPC_USER VARIANT_UserFree(unsigned long *, VARIANT *);

unsigned long __RPC_USER BSTR_UserSize64(unsigned long *, unsigned long, BSTR *);
unsigned char *__RPC_USER BSTR_UserMarshal64(unsigned long *, unsigned char *, BSTR *);
unsigned char *__RPC_USER BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR *);
void __RPC_USER BSTR_UserFree64(unsigned long *, BSTR *);

unsigned long __RPC_USER VARIANT_UserSize64(unsigned long *, unsigned long, VARIANT *);
unsigned char *__RPC_USER VARIANT_UserMarshal64(unsigned long *, unsigned char *, VARIANT *);
unsigned char *__RPC_USER VARIANT_UserUnmarshal64(unsigned long *, unsigned char *, VARIANT *);
void __RPC_USER VARIANT_UserFree64(unsigned long *, VARIANT *);

#ifdef __cplusplus
}
#endif

#endif /* _CERTCLI_H */
