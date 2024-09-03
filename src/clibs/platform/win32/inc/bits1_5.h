#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef _BITS1_5_H
#define _BITS1_5_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Forward Declarations */ 

#ifndef __IBackgroundCopyJob2_FWD_DEFINED__
#define __IBackgroundCopyJob2_FWD_DEFINED__
typedef interface IBackgroundCopyJob2 IBackgroundCopyJob2;
#endif /* __IBackgroundCopyJob2_FWD_DEFINED__ */

#ifndef __BackgroundCopyManager1_5_FWD_DEFINED__
#define __BackgroundCopyManager1_5_FWD_DEFINED__

#ifdef __cplusplus
typedef class BackgroundCopyManager1_5 BackgroundCopyManager1_5;
#else
typedef struct BackgroundCopyManager1_5 BackgroundCopyManager1_5;
#endif /* __cplusplus */

#endif /* __BackgroundCopyManager1_5_FWD_DEFINED__ */

#ifndef __IBackgroundCopyJob2_FWD_DEFINED__
#define __IBackgroundCopyJob2_FWD_DEFINED__
typedef interface IBackgroundCopyJob2 IBackgroundCopyJob2;
#endif /* __IBackgroundCopyJob2_FWD_DEFINED__ */

#include "bits.h"

#ifdef __cplusplus
extern "C"{
#endif 

extern RPC_IF_HANDLE __MIDL_itf_bits1_5_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_bits1_5_0000_0000_v0_0_s_ifspec;

#ifndef __IBackgroundCopyJob2_INTERFACE_DEFINED__
#define __IBackgroundCopyJob2_INTERFACE_DEFINED__

typedef struct _BG_JOB_REPLY_PROGRESS {
    UINT64 BytesTotal;
    UINT64 BytesTransferred;
} BG_JOB_REPLY_PROGRESS;

typedef enum __MIDL_IBackgroundCopyJob2_0001 {
    BG_AUTH_TARGET_SERVER = 1,
    BG_AUTH_TARGET_PROXY = (BG_AUTH_TARGET_SERVER + 1)
} BG_AUTH_TARGET;

typedef enum __MIDL_IBackgroundCopyJob2_0002 {
    BG_AUTH_SCHEME_BASIC = 1,
    BG_AUTH_SCHEME_DIGEST = (BG_AUTH_SCHEME_BASIC + 1),
    BG_AUTH_SCHEME_NTLM = (BG_AUTH_SCHEME_DIGEST + 1),
    BG_AUTH_SCHEME_NEGOTIATE = (BG_AUTH_SCHEME_NTLM + 1),
    BG_AUTH_SCHEME_PASSPORT = (BG_AUTH_SCHEME_NEGOTIATE + 1)
} BG_AUTH_SCHEME;

typedef struct __MIDL_IBackgroundCopyJob2_0003 {
    LPWSTR UserName;
    LPWSTR Password;
} BG_BASIC_CREDENTIALS;

typedef BG_BASIC_CREDENTIALS *PBG_BASIC_CREDENTIALS;

typedef union __MIDL_IBackgroundCopyJob2_0004 {
    BG_BASIC_CREDENTIALS Basic;
} BG_AUTH_CREDENTIALS_UNION;

typedef struct __MIDL_IBackgroundCopyJob2_0005 {
    BG_AUTH_TARGET Target;
    BG_AUTH_SCHEME Scheme;
    BG_AUTH_CREDENTIALS_UNION Credentials;
} BG_AUTH_CREDENTIALS;

typedef BG_AUTH_CREDENTIALS *PBG_AUTH_CREDENTIALS;

EXTERN_C const IID IID_IBackgroundCopyJob2;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("54b50739-686f-45eb-9dff-d6a9a0faa9af")
IBackgroundCopyJob2:public IBackgroundCopyJob
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetNotifyCmdLine(LPCWSTR Program, LPCWSTR Parameters) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNotifyCmdLine(LPWSTR *pProgram, LPWSTR *pParameters) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetReplyProgress(BG_JOB_REPLY_PROGRESS *pProgress) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetReplyData(byte ** ppBuffer, UINT64 *pLength) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetReplyFileName(LPCWSTR ReplyFileName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetReplyFileName(LPWSTR *pReplyFileName) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCredentials(BG_AUTH_CREDENTIALS *credentials) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveCredentials(BG_AUTH_TARGET Target, BG_AUTH_SCHEME Scheme) = 0;
};
#else 	/* C style interface */

typedef struct IBackgroundCopyJob2Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IBackgroundCopyJob2 *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IBackgroundCopyJob2 *This);
    ULONG (STDMETHODCALLTYPE *Release)(IBackgroundCopyJob2 *This);
    HRESULT (STDMETHODCALLTYPE *AddFileSet)(IBackgroundCopyJob2 *This, ULONG cFileCount, BG_FILE_INFO *pFileSet);
    HRESULT (STDMETHODCALLTYPE *AddFile)(IBackgroundCopyJob2 *This, LPCWSTR RemoteUrl, LPCWSTR LocalName);
    HRESULT (STDMETHODCALLTYPE *EnumFiles)(IBackgroundCopyJob2 *This, IEnumBackgroundCopyFiles ** pEnum);
    HRESULT (STDMETHODCALLTYPE *Suspend)(IBackgroundCopyJob2 *This);
    HRESULT (STDMETHODCALLTYPE *Resume)(IBackgroundCopyJob2 *This);
    HRESULT (STDMETHODCALLTYPE *Cancel)(IBackgroundCopyJob2 *This);
    HRESULT (STDMETHODCALLTYPE *Complete)(IBackgroundCopyJob2 *This);
    HRESULT (STDMETHODCALLTYPE *GetId)(IBackgroundCopyJob2 *This, GUID *pVal);
    HRESULT (STDMETHODCALLTYPE *GetType)(IBackgroundCopyJob2 *This, BG_JOB_TYPE *pVal);
    HRESULT (STDMETHODCALLTYPE *GetProgress)(IBackgroundCopyJob2 *This, BG_JOB_PROGRESS *pVal);
    HRESULT (STDMETHODCALLTYPE *GetTimes)(IBackgroundCopyJob2 *This, BG_JOB_TIMES *pVal);
    HRESULT (STDMETHODCALLTYPE *GetState)(IBackgroundCopyJob2 *This, BG_JOB_STATE *pVal);
    HRESULT (STDMETHODCALLTYPE *GetError)(IBackgroundCopyJob2 *This, IBackgroundCopyError ** ppError);
    HRESULT (STDMETHODCALLTYPE *GetOwner)(IBackgroundCopyJob2 *This, LPWSTR *pVal);
    HRESULT (STDMETHODCALLTYPE *SetDisplayName)(IBackgroundCopyJob2 *This, LPCWSTR Val);
    HRESULT (STDMETHODCALLTYPE *GetDisplayName)(IBackgroundCopyJob2 *This, LPWSTR *pVal);
    HRESULT (STDMETHODCALLTYPE *SetDescription)(IBackgroundCopyJob2 *This, LPCWSTR Val);
    HRESULT (STDMETHODCALLTYPE *GetDescription)(IBackgroundCopyJob2 *This, LPWSTR *pVal);
    HRESULT (STDMETHODCALLTYPE *SetPriority)(IBackgroundCopyJob2 *This, BG_JOB_PRIORITY Val);
    HRESULT (STDMETHODCALLTYPE *GetPriority)(IBackgroundCopyJob2 *This, BG_JOB_PRIORITY *pVal);
    HRESULT (STDMETHODCALLTYPE *SetNotifyFlags)(IBackgroundCopyJob2 *This, ULONG Val);
    HRESULT (STDMETHODCALLTYPE *GetNotifyFlags)(IBackgroundCopyJob2 *This, ULONG *pVal);
    HRESULT (STDMETHODCALLTYPE *SetNotifyInterface)(IBackgroundCopyJob2 *This, IUnknown *Val);
    HRESULT (STDMETHODCALLTYPE *GetNotifyInterface)(IBackgroundCopyJob2 *This, IUnknown ** pVal);
    HRESULT (STDMETHODCALLTYPE *SetMinimumRetryDelay)(IBackgroundCopyJob2 *This, ULONG Seconds);
    HRESULT (STDMETHODCALLTYPE *GetMinimumRetryDelay)(IBackgroundCopyJob2 *This, ULONG *Seconds);
    HRESULT (STDMETHODCALLTYPE *SetNoProgressTimeout)(IBackgroundCopyJob2 *This, ULONG Seconds);
    HRESULT (STDMETHODCALLTYPE *GetNoProgressTimeout)(IBackgroundCopyJob2 *This, ULONG *Seconds);
    HRESULT (STDMETHODCALLTYPE *GetErrorCount)(IBackgroundCopyJob2 *This, ULONG *Errors);
    HRESULT (STDMETHODCALLTYPE *SetProxySettings)(IBackgroundCopyJob2 *This, BG_JOB_PROXY_USAGE ProxyUsage, const WCHAR *ProxyList, const WCHAR *ProxyBypassList);
    HRESULT (STDMETHODCALLTYPE *GetProxySettings)(IBackgroundCopyJob2 *This, BG_JOB_PROXY_USAGE *pProxyUsage, LPWSTR *pProxyList, LPWSTR *pProxyBypassList);
    HRESULT (STDMETHODCALLTYPE *TakeOwnership)(IBackgroundCopyJob2 *This);
    HRESULT (STDMETHODCALLTYPE *SetNotifyCmdLine)(IBackgroundCopyJob2 *This, LPCWSTR Program, LPCWSTR Parameters);
    HRESULT (STDMETHODCALLTYPE *GetNotifyCmdLine)(IBackgroundCopyJob2 *This, LPWSTR *pProgram, LPWSTR *pParameters);
    HRESULT (STDMETHODCALLTYPE *GetReplyProgress)(IBackgroundCopyJob2 *This, BG_JOB_REPLY_PROGRESS *pProgress);
    HRESULT (STDMETHODCALLTYPE *GetReplyData)(IBackgroundCopyJob2 *This, byte ** ppBuffer, UINT64 *pLength);
    HRESULT (STDMETHODCALLTYPE *SetReplyFileName)(IBackgroundCopyJob2 *This, LPCWSTR ReplyFileName);
    HRESULT (STDMETHODCALLTYPE *GetReplyFileName)(IBackgroundCopyJob2 *This, LPWSTR *pReplyFileName);
    HRESULT (STDMETHODCALLTYPE *SetCredentials)(IBackgroundCopyJob2 *This, BG_AUTH_CREDENTIALS *credentials);
    HRESULT (STDMETHODCALLTYPE *RemoveCredentials)(IBackgroundCopyJob2 *This, BG_AUTH_TARGET Target, BG_AUTH_SCHEME Scheme);
    END_INTERFACE
} IBackgroundCopyJob2Vtbl;

interface IBackgroundCopyJob2 {
    CONST_VTBL struct IBackgroundCopyJob2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IBackgroundCopyJob2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IBackgroundCopyJob2_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IBackgroundCopyJob2_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IBackgroundCopyJob2_AddFileSet(This,cFileCount,pFileSet)  ((This)->lpVtbl->AddFileSet(This,cFileCount,pFileSet)) 
#define IBackgroundCopyJob2_AddFile(This,RemoteUrl,LocalName)  ((This)->lpVtbl->AddFile(This,RemoteUrl,LocalName)) 
#define IBackgroundCopyJob2_EnumFiles(This,pEnum)  ((This)->lpVtbl->EnumFiles(This,pEnum)) 
#define IBackgroundCopyJob2_Suspend(This)  ((This)->lpVtbl->Suspend(This)) 
#define IBackgroundCopyJob2_Resume(This)  ((This)->lpVtbl->Resume(This)) 
#define IBackgroundCopyJob2_Cancel(This)  ((This)->lpVtbl->Cancel(This)) 
#define IBackgroundCopyJob2_Complete(This)  ((This)->lpVtbl->Complete(This)) 
#define IBackgroundCopyJob2_GetId(This,pVal)  ((This)->lpVtbl->GetId(This,pVal)) 
#define IBackgroundCopyJob2_GetType(This,pVal)  ((This)->lpVtbl->GetType(This,pVal)) 
#define IBackgroundCopyJob2_GetProgress(This,pVal)  ((This)->lpVtbl->GetProgress(This,pVal)) 
#define IBackgroundCopyJob2_GetTimes(This,pVal)  ((This)->lpVtbl->GetTimes(This,pVal)) 
#define IBackgroundCopyJob2_GetState(This,pVal)  ((This)->lpVtbl->GetState(This,pVal)) 
#define IBackgroundCopyJob2_GetError(This,ppError)  ((This)->lpVtbl->GetError(This,ppError)) 
#define IBackgroundCopyJob2_GetOwner(This,pVal)  ((This)->lpVtbl->GetOwner(This,pVal)) 
#define IBackgroundCopyJob2_SetDisplayName(This,Val)  ((This)->lpVtbl->SetDisplayName(This,Val)) 
#define IBackgroundCopyJob2_GetDisplayName(This,pVal)  ((This)->lpVtbl->GetDisplayName(This,pVal)) 
#define IBackgroundCopyJob2_SetDescription(This,Val)  ((This)->lpVtbl->SetDescription(This,Val)) 
#define IBackgroundCopyJob2_GetDescription(This,pVal)  ((This)->lpVtbl->GetDescription(This,pVal)) 
#define IBackgroundCopyJob2_SetPriority(This,Val)  ((This)->lpVtbl->SetPriority(This,Val)) 
#define IBackgroundCopyJob2_GetPriority(This,pVal)  ((This)->lpVtbl->GetPriority(This,pVal)) 
#define IBackgroundCopyJob2_SetNotifyFlags(This,Val)  ((This)->lpVtbl->SetNotifyFlags(This,Val)) 
#define IBackgroundCopyJob2_GetNotifyFlags(This,pVal)  ((This)->lpVtbl->GetNotifyFlags(This,pVal)) 
#define IBackgroundCopyJob2_SetNotifyInterface(This,Val)  ((This)->lpVtbl->SetNotifyInterface(This,Val)) 
#define IBackgroundCopyJob2_GetNotifyInterface(This,pVal)  ((This)->lpVtbl->GetNotifyInterface(This,pVal)) 
#define IBackgroundCopyJob2_SetMinimumRetryDelay(This,Seconds)  ((This)->lpVtbl->SetMinimumRetryDelay(This,Seconds)) 
#define IBackgroundCopyJob2_GetMinimumRetryDelay(This,Seconds)  ((This)->lpVtbl->GetMinimumRetryDelay(This,Seconds)) 
#define IBackgroundCopyJob2_SetNoProgressTimeout(This,Seconds)  ((This)->lpVtbl->SetNoProgressTimeout(This,Seconds)) 
#define IBackgroundCopyJob2_GetNoProgressTimeout(This,Seconds)  ((This)->lpVtbl->GetNoProgressTimeout(This,Seconds)) 
#define IBackgroundCopyJob2_GetErrorCount(This,Errors)  ((This)->lpVtbl->GetErrorCount(This,Errors)) 
#define IBackgroundCopyJob2_SetProxySettings(This,ProxyUsage,ProxyList,ProxyBypassList)  ((This)->lpVtbl->SetProxySettings(This,ProxyUsage,ProxyList,ProxyBypassList)) 
#define IBackgroundCopyJob2_GetProxySettings(This,pProxyUsage,pProxyList,pProxyBypassList)  ((This)->lpVtbl->GetProxySettings(This,pProxyUsage,pProxyList,pProxyBypassList)) 
#define IBackgroundCopyJob2_TakeOwnership(This)  ((This)->lpVtbl->TakeOwnership(This)) 
#define IBackgroundCopyJob2_SetNotifyCmdLine(This,Program,Parameters)  ((This)->lpVtbl->SetNotifyCmdLine(This,Program,Parameters)) 
#define IBackgroundCopyJob2_GetNotifyCmdLine(This,pProgram,pParameters)  ((This)->lpVtbl->GetNotifyCmdLine(This,pProgram,pParameters)) 
#define IBackgroundCopyJob2_GetReplyProgress(This,pProgress)  ((This)->lpVtbl->GetReplyProgress(This,pProgress)) 
#define IBackgroundCopyJob2_GetReplyData(This,ppBuffer,pLength)  ((This)->lpVtbl->GetReplyData(This,ppBuffer,pLength)) 
#define IBackgroundCopyJob2_SetReplyFileName(This,ReplyFileName)  ((This)->lpVtbl->SetReplyFileName(This,ReplyFileName)) 
#define IBackgroundCopyJob2_GetReplyFileName(This,pReplyFileName)  ((This)->lpVtbl->GetReplyFileName(This,pReplyFileName)) 
#define IBackgroundCopyJob2_SetCredentials(This,credentials)  ((This)->lpVtbl->SetCredentials(This,credentials)) 
#define IBackgroundCopyJob2_RemoveCredentials(This,Target,Scheme)  ((This)->lpVtbl->RemoveCredentials(This,Target,Scheme)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IBackgroundCopyJob2_INTERFACE_DEFINED__ */


#ifndef __BackgroundCopyManager1_5_LIBRARY_DEFINED__
#define __BackgroundCopyManager1_5_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_BackgroundCopyManager1_5;
EXTERN_C const CLSID CLSID_BackgroundCopyManager1_5;

#ifdef __cplusplus
class DECLSPEC_UUID("f087771f-d74f-4c1a-bb8a-e16aca9124ea")
BackgroundCopyManager1_5;
#endif
#endif /* __BackgroundCopyManager1_5_LIBRARY_DEFINED__ */

#include "bits2_0.h"

extern RPC_IF_HANDLE __MIDL_itf_bits1_5_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_bits1_5_0000_0002_v0_0_s_ifspec;

#ifdef __cplusplus
}
#endif

#endif /* _BITS1_5_H */
