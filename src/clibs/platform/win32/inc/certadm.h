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

#ifndef _CERTADM_H
#define _CERTADM_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ??? */

#ifndef __ICertAdmin_FWD_DEFINED__
#define __ICertAdmin_FWD_DEFINED__
typedef interface ICertAdmin ICertAdmin;
#endif

#ifndef __CCertAdmin_FWD_DEFINED__
#define __CCertAdmin_FWD_DEFINED__
#ifdef __cplusplus
typedef class CCertAdmin CCertAdmin;
#else
typedef struct CCertAdmin CCertAdmin;
#endif /* __cplusplus */
#endif

#ifndef __CCertView_FWD_DEFINED__
#define __CCertView_FWD_DEFINED__
#ifdef __cplusplus
typedef class CCertView CCertView;
#else
typedef struct CCertView CCertView;
#endif /* __cplusplus */
#endif

#include "wtypes.h"
#include "certview.h"

#ifdef __cplusplus
extern "C" {
#endif

void *__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void *);

#define CA_DISP_INCOMPLETE  (0)
#define CA_DISP_ERROR  (0x1)
#define CA_DISP_REVOKED (0x2)
#define CA_DISP_VALID  (0x3)
#define CA_DISP_INVALID (0x4)
#define CA_DISP_UNDER_SUBMISSION  (0x5)

extern RPC_IF_HANDLE __MIDL_itf_certadm_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_certadm_0000_v0_0_s_ifspec;

#ifndef __ICertAdmin_INTERFACE_DEFINED__
#define __ICertAdmin_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICertAdmin;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("34df6950-7fb6-11d0-8817-00a0c903b83c") ICertAdmin:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE IsValidCertificate(const BSTR,const BSTR,LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRevocationReason(LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE RevokeCertificate(const BSTR,const BSTR,LONG,DATE) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetRequestAttributes(const BSTR,LONG,const BSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCertificateExtension(const BSTR,LONG,const BSTR,LONG,LONG,const VARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE DenyRequest(const BSTR,LONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResubmitRequest(const BSTR,LONG,LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE PublishCRL(const BSTR,DATE) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCRL(const BSTR,LONG,BSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE ImportCertificate(const BSTR,const BSTR,LONG,LONG*) = 0;
};
#else /* C style interface */
typedef struct ICertAdminVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICertAdmin*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICertAdmin*);
    ULONG(STDMETHODCALLTYPE *Release)(ICertAdmin*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ICertAdmin*,UINT*);
    HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ICertAdmin*,UINT,LCID,ITypeInfo**);
    HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ICertAdmin*,REFIID,LPOLESTR*,UINT,LCID,DISPID*);
    HRESULT(STDMETHODCALLTYPE *Invoke)(ICertAdmin*,DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
    HRESULT(STDMETHODCALLTYPE * IsValidCertificate)(ICertAdmin*,const BSTR,const BSTR,LONG*);
    HRESULT(STDMETHODCALLTYPE * GetRevocationReason)(ICertAdmin*,LONG*);
    HRESULT(STDMETHODCALLTYPE * RevokeCertificate)(ICertAdmin*,const BSTR,const BSTR,LONG,DATE);
    HRESULT(STDMETHODCALLTYPE * SetRequestAttributes)(ICertAdmin*,const BSTR,LONG,const BSTR);
    HRESULT(STDMETHODCALLTYPE * SetCertificateExtension)(ICertAdmin*,const BSTR,LONG,const BSTR,LONG,LONG,const VARIANT*);
    HRESULT(STDMETHODCALLTYPE * DenyRequest)(ICertAdmin*,const BSTR,LONG);
    HRESULT(STDMETHODCALLTYPE * ResubmitRequest)(ICertAdmin*,const BSTR,LONG,LONG*);
    HRESULT(STDMETHODCALLTYPE * PublishCRL)(ICertAdmin*,const BSTR,DATE);
    HRESULT(STDMETHODCALLTYPE * GetCRL)(ICertAdmin*,const BSTR,LONG,BSTR*);
    HRESULT(STDMETHODCALLTYPE * ImportCertificate)(ICertAdmin*,const BSTR,const BSTR,LONG,LONG*);
    END_INTERFACE
} ICertAdminVtbl;

interface ICertAdmin {
    CONST_VTBL struct ICertAdminVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICertAdmin_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ICertAdmin_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ICertAdmin_Release(This)  (This)->lpVtbl->Release(This)
#define ICertAdmin_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define ICertAdmin_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define ICertAdmin_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define ICertAdmin_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define ICertAdmin_IsValidCertificate(This,strConfig,strSerialNumber,pDisposition)  (This)->lpVtbl->IsValidCertificate(This,strConfig,strSerialNumber,pDisposition)
#define ICertAdmin_GetRevocationReason(This,pReason)  (This)->lpVtbl->GetRevocationReason(This,pReason)
#define ICertAdmin_RevokeCertificate(This,strConfig,strSerialNumber,Reason,Date)  (This)->lpVtbl->RevokeCertificate(This,strConfig,strSerialNumber,Reason,Date)
#define ICertAdmin_SetRequestAttributes(This,strConfig,RequestId,strAttributes)  (This)->lpVtbl->SetRequestAttributes(This,strConfig,RequestId,strAttributes)
#define ICertAdmin_SetCertificateExtension(This,strConfig,RequestId,strExtensionName,Type,Flags,pvarValue)  (This)->lpVtbl->SetCertificateExtension(This,strConfig,RequestId,strExtensionName,Type,Flags,pvarValue)
#define ICertAdmin_DenyRequest(This,strConfig,RequestId)  (This)->lpVtbl->DenyRequest(This,strConfig,RequestId)
#define ICertAdmin_ResubmitRequest(This,strConfig,RequestId,pDisposition)  (This)->lpVtbl->ResubmitRequest(This,strConfig,RequestId,pDisposition)
#define ICertAdmin_PublishCRL(This,strConfig,Date)  (This)->lpVtbl->PublishCRL(This,strConfig,Date)
#define ICertAdmin_GetCRL(This,strConfig,Flags,pstrCRL)  (This)->lpVtbl->GetCRL(This,strConfig,Flags,pstrCRL)
#define ICertAdmin_ImportCertificate(This,strConfig,strCertificate,Flags,pRequestID)  (This)->lpVtbl->ImportCertificate(This,strConfig,strCertificate,Flags,pRequestID)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE ICertAdmin_IsValidCertificate_Proxy(ICertAdmin*,const BSTR,const BSTR,LONG*);
void __RPC_STUB ICertAdmin_IsValidCertificate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertAdmin_GetRevocationReason_Proxy(ICertAdmin*,LONG*);
void __RPC_STUB ICertAdmin_GetRevocationReason_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertAdmin_RevokeCertificate_Proxy(ICertAdmin*,const BSTR,const BSTR,LONG,DATE);
void __RPC_STUB ICertAdmin_RevokeCertificate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertAdmin_SetRequestAttributes_Proxy(ICertAdmin*,const BSTR,LONG,const BSTR);
void __RPC_STUB ICertAdmin_SetRequestAttributes_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertAdmin_SetCertificateExtension_Proxy(ICertAdmin*,const BSTR,LONG,const BSTR,LONG,LONG,const VARIANT*);
void __RPC_STUB ICertAdmin_SetCertificateExtension_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertAdmin_DenyRequest_Proxy(ICertAdmin*,const BSTR,LONG);
void __RPC_STUB ICertAdmin_DenyRequest_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertAdmin_ResubmitRequest_Proxy(ICertAdmin*,const BSTR,LONG,LONG*);
void __RPC_STUB ICertAdmin_ResubmitRequest_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertAdmin_PublishCRL_Proxy(ICertAdmin*,const BSTR,DATE);
void __RPC_STUB ICertAdmin_PublishCRL_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertAdmin_GetCRL_Proxy(ICertAdmin*,const BSTR,LONG,BSTR*);
void __RPC_STUB ICertAdmin_GetCRL_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICertAdmin_ImportCertificate_Proxy(ICertAdmin*,const BSTR,const BSTR,LONG,LONG*);
void __RPC_STUB ICertAdmin_ImportCertificate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __ICertAdmin_INTERFACE_DEFINED__ */

#ifndef __CERTADMINLib_LIBRARY_DEFINED__
#define __CERTADMINLib_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_CERTADMINLib;
EXTERN_C const CLSID CLSID_CCertAdmin;

#ifdef __cplusplus
class DECLSPEC_UUID("37eabaf0-7fb6-11d0-8817-00a0c903b83c") CCertAdmin;
#endif

EXTERN_C const CLSID CLSID_CCertView;

#ifdef __cplusplus
class DECLSPEC_UUID("a12d0f7a-1e84-11d1-9bd6-00c04fb683fa") CCertView;
#endif
#endif /* __CERTADMINLib_LIBRARY_DEFINED__ */

unsigned long __RPC_USER BSTR_UserSize(unsigned long*, unsigned long, BSTR*);
unsigned char *__RPC_USER BSTR_UserMarshal(unsigned long*, unsigned char*, BSTR*);
unsigned char *__RPC_USER BSTR_UserUnmarshal(unsigned long*, unsigned char*, BSTR*);
void __RPC_USER BSTR_UserFree(unsigned long*, BSTR*);

unsigned long __RPC_USER VARIANT_UserSize(unsigned long*, unsigned long, VARIANT*);
unsigned char *__RPC_USER VARIANT_UserMarshal(unsigned long*, unsigned char*, VARIANT*);
unsigned char *__RPC_USER VARIANT_UserUnmarshal(unsigned long*, unsigned char*, VARIANT*);
void __RPC_USER VARIANT_UserFree(unsigned long*, VARIANT*);

#ifdef __cplusplus
}
#endif

#endif /* _CERTADM_H */
