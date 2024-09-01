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

#ifndef _BITS2_0_H
#define _BITS2_0_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Forward Declarations */ 

#ifndef __IBackgroundCopyJob3_FWD_DEFINED__
#define __IBackgroundCopyJob3_FWD_DEFINED__
typedef interface IBackgroundCopyJob3 IBackgroundCopyJob3;
#endif /* __IBackgroundCopyJob3_FWD_DEFINED__ */

#ifndef __IBackgroundCopyFile2_FWD_DEFINED__
#define __IBackgroundCopyFile2_FWD_DEFINED__
typedef interface IBackgroundCopyFile2 IBackgroundCopyFile2;
#endif /* __IBackgroundCopyFile2_FWD_DEFINED__ */

#ifndef __BackgroundCopyManager2_0_FWD_DEFINED__
#define __BackgroundCopyManager2_0_FWD_DEFINED__

#ifdef __cplusplus
typedef class BackgroundCopyManager2_0 BackgroundCopyManager2_0;
#else
typedef struct BackgroundCopyManager2_0 BackgroundCopyManager2_0;
#endif /* __cplusplus */

#endif /* __BackgroundCopyManager2_0_FWD_DEFINED__ */

#ifndef __IBackgroundCopyJob3_FWD_DEFINED__
#define __IBackgroundCopyJob3_FWD_DEFINED__
typedef interface IBackgroundCopyJob3 IBackgroundCopyJob3;
#endif /* __IBackgroundCopyJob3_FWD_DEFINED__ */

#ifndef __IBackgroundCopyFile2_FWD_DEFINED__
#define __IBackgroundCopyFile2_FWD_DEFINED__
typedef interface IBackgroundCopyFile2 IBackgroundCopyFile2;
#endif /* __IBackgroundCopyFile2_FWD_DEFINED__ */

#include "bits.h"
#include "bits1_5.h"

#ifdef __cplusplus
extern "C"{
#endif 

#define BG_LENGTH_TO_EOF (UINT64)(-1)

typedef struct _BG_FILE_RANGE {
    UINT64 InitialOffset;
    UINT64 Length;
} BG_FILE_RANGE;

#define BG_COPY_FILE_OWNER   1
#define BG_COPY_FILE_GROUP   2
#define BG_COPY_FILE_DACL    4
#define BG_COPY_FILE_SACL    8
#define BG_COPY_FILE_ALL    15

extern RPC_IF_HANDLE __MIDL_itf_bits2_0_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_bits2_0_0000_0000_v0_0_s_ifspec;

#ifndef __IBackgroundCopyJob3_INTERFACE_DEFINED__
#define __IBackgroundCopyJob3_INTERFACE_DEFINED__

EXTERN_C const IID IID_IBackgroundCopyJob3;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("443c8934-90ff-48ed-bcde-26f5c7450042")
IBackgroundCopyJob3:public IBackgroundCopyJob2
{
    public:
    virtual HRESULT STDMETHODCALLTYPE ReplaceRemotePrefix(LPCWSTR OldPrefix, LPCWSTR NewPrefix) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddFileWithRanges(LPCWSTR RemoteUrl, LPCWSTR LocalName, DWORD RangeCount, BG_FILE_RANGE Ranges[]) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFileACLFlags(DWORD Flags) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFileACLFlags(DWORD *Flags) = 0;
};
#else /* C style interface */

typedef struct IBackgroundCopyJob3Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IBackgroundCopyJob3 *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IBackgroundCopyJob3 *This);
    ULONG (STDMETHODCALLTYPE *Release)(IBackgroundCopyJob3 *This);
    HRESULT (STDMETHODCALLTYPE *AddFileSet)(IBackgroundCopyJob3 *This, ULONG cFileCount, BG_FILE_INFO *pFileSet);
    HRESULT (STDMETHODCALLTYPE *AddFile)(IBackgroundCopyJob3 *This, LPCWSTR RemoteUrl, LPCWSTR LocalName);
    HRESULT (STDMETHODCALLTYPE *EnumFiles)(IBackgroundCopyJob3 *This, IEnumBackgroundCopyFiles ** pEnum);
    HRESULT (STDMETHODCALLTYPE *Suspend)(IBackgroundCopyJob3 *This);
    HRESULT (STDMETHODCALLTYPE *Resume)(IBackgroundCopyJob3 *This);
    HRESULT (STDMETHODCALLTYPE *Cancel)(IBackgroundCopyJob3 *This);
    HRESULT (STDMETHODCALLTYPE *Complete)(IBackgroundCopyJob3 *This);
    HRESULT (STDMETHODCALLTYPE *GetId)(IBackgroundCopyJob3 *This, GUID *pVal);
    HRESULT (STDMETHODCALLTYPE *GetType)(IBackgroundCopyJob3 *This, BG_JOB_TYPE *pVal);
    HRESULT (STDMETHODCALLTYPE *GetProgress)(IBackgroundCopyJob3 *This, BG_JOB_PROGRESS *pVal);
    HRESULT (STDMETHODCALLTYPE *GetTimes)(IBackgroundCopyJob3 *This, BG_JOB_TIMES *pVal);
    HRESULT (STDMETHODCALLTYPE *GetState)(IBackgroundCopyJob3 *This, BG_JOB_STATE *pVal);
    HRESULT (STDMETHODCALLTYPE *GetError)(IBackgroundCopyJob3 *This, IBackgroundCopyError ** ppError);
    HRESULT (STDMETHODCALLTYPE *GetOwner)(IBackgroundCopyJob3 *This, LPWSTR *pVal);
    HRESULT (STDMETHODCALLTYPE *SetDisplayName)(IBackgroundCopyJob3 *This, LPCWSTR Val);
    HRESULT (STDMETHODCALLTYPE *GetDisplayName)(IBackgroundCopyJob3 *This, LPWSTR *pVal);
    HRESULT (STDMETHODCALLTYPE *SetDescription)(IBackgroundCopyJob3 *This, LPCWSTR Val);
    HRESULT (STDMETHODCALLTYPE *GetDescription)(IBackgroundCopyJob3 *This, LPWSTR *pVal);
    HRESULT (STDMETHODCALLTYPE *SetPriority)(IBackgroundCopyJob3 *This, BG_JOB_PRIORITY Val);
    HRESULT (STDMETHODCALLTYPE *GetPriority)(IBackgroundCopyJob3 *This, BG_JOB_PRIORITY *pVal);
    HRESULT (STDMETHODCALLTYPE *SetNotifyFlags)(IBackgroundCopyJob3 *This, ULONG Val);
    HRESULT (STDMETHODCALLTYPE *GetNotifyFlags)(IBackgroundCopyJob3 *This, ULONG *pVal);
    HRESULT (STDMETHODCALLTYPE *SetNotifyInterface)(IBackgroundCopyJob3 *This, IUnknown *Val);
    HRESULT (STDMETHODCALLTYPE *GetNotifyInterface)(IBackgroundCopyJob3 *This, IUnknown ** pVal);
    HRESULT (STDMETHODCALLTYPE *SetMinimumRetryDelay)(IBackgroundCopyJob3 *This, ULONG Seconds);
    HRESULT (STDMETHODCALLTYPE *GetMinimumRetryDelay)(IBackgroundCopyJob3 *This, ULONG *Seconds);
    HRESULT (STDMETHODCALLTYPE *SetNoProgressTimeout)(IBackgroundCopyJob3 *This, ULONG Seconds);
    HRESULT (STDMETHODCALLTYPE *GetNoProgressTimeout)(IBackgroundCopyJob3 *This, ULONG *Seconds);
    HRESULT (STDMETHODCALLTYPE *GetErrorCount)(IBackgroundCopyJob3 *This, ULONG *Errors);
    HRESULT (STDMETHODCALLTYPE *SetProxySettings)(IBackgroundCopyJob3 *This, BG_JOB_PROXY_USAGE ProxyUsage, const WCHAR *ProxyList, const WCHAR *ProxyBypassList);
    HRESULT (STDMETHODCALLTYPE *GetProxySettings)(IBackgroundCopyJob3 *This, BG_JOB_PROXY_USAGE *pProxyUsage, LPWSTR *pProxyList, LPWSTR *pProxyBypassList);
    HRESULT (STDMETHODCALLTYPE *TakeOwnership)(IBackgroundCopyJob3 *This);
    HRESULT (STDMETHODCALLTYPE *SetNotifyCmdLine)(IBackgroundCopyJob3 *This, LPCWSTR Program, LPCWSTR Parameters);
    HRESULT (STDMETHODCALLTYPE *GetNotifyCmdLine)(IBackgroundCopyJob3 *This, LPWSTR *pProgram, LPWSTR *pParameters);
    HRESULT (STDMETHODCALLTYPE *GetReplyProgress)(IBackgroundCopyJob3 *This, BG_JOB_REPLY_PROGRESS *pProgress);
    HRESULT (STDMETHODCALLTYPE *GetReplyData)(IBackgroundCopyJob3 *This, byte ** ppBuffer, UINT64 *pLength);
    HRESULT (STDMETHODCALLTYPE *SetReplyFileName)(IBackgroundCopyJob3 *This, LPCWSTR ReplyFileName);
    HRESULT (STDMETHODCALLTYPE *GetReplyFileName)(IBackgroundCopyJob3 *This, LPWSTR *pReplyFileName);
    HRESULT (STDMETHODCALLTYPE *SetCredentials)(IBackgroundCopyJob3 *This, BG_AUTH_CREDENTIALS *credentials);
    HRESULT (STDMETHODCALLTYPE *RemoveCredentials)(IBackgroundCopyJob3 *This, BG_AUTH_TARGET Target, BG_AUTH_SCHEME Scheme);
    HRESULT (STDMETHODCALLTYPE *ReplaceRemotePrefix)(IBackgroundCopyJob3 *This, LPCWSTR OldPrefix, LPCWSTR NewPrefix);
    HRESULT (STDMETHODCALLTYPE *AddFileWithRanges)(IBackgroundCopyJob3 *This, LPCWSTR RemoteUrl, LPCWSTR LocalName, DWORD RangeCount, BG_FILE_RANGE Ranges[]);
    HRESULT (STDMETHODCALLTYPE *SetFileACLFlags)(IBackgroundCopyJob3 *This, DWORD Flags);
    HRESULT (STDMETHODCALLTYPE *GetFileACLFlags)(IBackgroundCopyJob3 *This, DWORD *Flags);
    END_INTERFACE
} IBackgroundCopyJob3Vtbl;

interface IBackgroundCopyJob3 {
    CONST_VTBL struct IBackgroundCopyJob3Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IBackgroundCopyJob3_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IBackgroundCopyJob3_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IBackgroundCopyJob3_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IBackgroundCopyJob3_AddFileSet(This,cFileCount,pFileSet)  ((This)->lpVtbl->AddFileSet(This,cFileCount,pFileSet)) 
#define IBackgroundCopyJob3_AddFile(This,RemoteUrl,LocalName)  ((This)->lpVtbl->AddFile(This,RemoteUrl,LocalName)) 
#define IBackgroundCopyJob3_EnumFiles(This,pEnum)  ((This)->lpVtbl->EnumFiles(This,pEnum)) 
#define IBackgroundCopyJob3_Suspend(This)  ((This)->lpVtbl->Suspend(This)) 
#define IBackgroundCopyJob3_Resume(This)  ((This)->lpVtbl->Resume(This)) 
#define IBackgroundCopyJob3_Cancel(This)  ((This)->lpVtbl->Cancel(This)) 
#define IBackgroundCopyJob3_Complete(This)  ((This)->lpVtbl->Complete(This)) 
#define IBackgroundCopyJob3_GetId(This,pVal)  ((This)->lpVtbl->GetId(This,pVal)) 
#define IBackgroundCopyJob3_GetType(This,pVal)  ((This)->lpVtbl->GetType(This,pVal)) 
#define IBackgroundCopyJob3_GetProgress(This,pVal)  ((This)->lpVtbl->GetProgress(This,pVal)) 
#define IBackgroundCopyJob3_GetTimes(This,pVal)  ((This)->lpVtbl->GetTimes(This,pVal)) 
#define IBackgroundCopyJob3_GetState(This,pVal)  ((This)->lpVtbl->GetState(This,pVal)) 
#define IBackgroundCopyJob3_GetError(This,ppError)  ((This)->lpVtbl->GetError(This,ppError)) 
#define IBackgroundCopyJob3_GetOwner(This,pVal)  ((This)->lpVtbl->GetOwner(This,pVal)) 
#define IBackgroundCopyJob3_SetDisplayName(This,Val)  ((This)->lpVtbl->SetDisplayName(This,Val)) 
#define IBackgroundCopyJob3_GetDisplayName(This,pVal)  ((This)->lpVtbl->GetDisplayName(This,pVal)) 
#define IBackgroundCopyJob3_SetDescription(This,Val)  ((This)->lpVtbl->SetDescription(This,Val)) 
#define IBackgroundCopyJob3_GetDescription(This,pVal)  ((This)->lpVtbl->GetDescription(This,pVal)) 
#define IBackgroundCopyJob3_SetPriority(This,Val)  ((This)->lpVtbl->SetPriority(This,Val)) 
#define IBackgroundCopyJob3_GetPriority(This,pVal)  ((This)->lpVtbl->GetPriority(This,pVal)) 
#define IBackgroundCopyJob3_SetNotifyFlags(This,Val)  ((This)->lpVtbl->SetNotifyFlags(This,Val)) 
#define IBackgroundCopyJob3_GetNotifyFlags(This,pVal)   ((This)->lpVtbl->GetNotifyFlags(This,pVal)) 
#define IBackgroundCopyJob3_SetNotifyInterface(This,Val)  ((This)->lpVtbl->SetNotifyInterface(This,Val)) 
#define IBackgroundCopyJob3_GetNotifyInterface(This,pVal)  ((This)->lpVtbl->GetNotifyInterface(This,pVal)) 
#define IBackgroundCopyJob3_SetMinimumRetryDelay(This,Seconds)  ((This)->lpVtbl->SetMinimumRetryDelay(This,Seconds)) 
#define IBackgroundCopyJob3_GetMinimumRetryDelay(This,Seconds)  ((This)->lpVtbl->GetMinimumRetryDelay(This,Seconds)) 
#define IBackgroundCopyJob3_SetNoProgressTimeout(This,Seconds)  ((This)->lpVtbl->SetNoProgressTimeout(This,Seconds)) 
#define IBackgroundCopyJob3_GetNoProgressTimeout(This,Seconds)  ((This)->lpVtbl->GetNoProgressTimeout(This,Seconds)) 
#define IBackgroundCopyJob3_GetErrorCount(This,Errors)  ((This)->lpVtbl->GetErrorCount(This,Errors)) 
#define IBackgroundCopyJob3_SetProxySettings(This,ProxyUsage,ProxyList,ProxyBypassList)  ((This)->lpVtbl->SetProxySettings(This,ProxyUsage,ProxyList,ProxyBypassList)) 
#define IBackgroundCopyJob3_GetProxySettings(This,pProxyUsage,pProxyList,pProxyBypassList)  ((This)->lpVtbl->GetProxySettings(This,pProxyUsage,pProxyList,pProxyBypassList)) 
#define IBackgroundCopyJob3_TakeOwnership(This)  ((This)->lpVtbl->TakeOwnership(This)) 
#define IBackgroundCopyJob3_SetNotifyCmdLine(This,Program,Parameters)  ((This)->lpVtbl->SetNotifyCmdLine(This,Program,Parameters)) 
#define IBackgroundCopyJob3_GetNotifyCmdLine(This,pProgram,pParameters)  ((This)->lpVtbl->GetNotifyCmdLine(This,pProgram,pParameters)) 
#define IBackgroundCopyJob3_GetReplyProgress(This,pProgress)  ((This)->lpVtbl->GetReplyProgress(This,pProgress)) 
#define IBackgroundCopyJob3_GetReplyData(This,ppBuffer,pLength)  ((This)->lpVtbl->GetReplyData(This,ppBuffer,pLength)) 
#define IBackgroundCopyJob3_SetReplyFileName(This,ReplyFileName)  ((This)->lpVtbl->SetReplyFileName(This,ReplyFileName)) 
#define IBackgroundCopyJob3_GetReplyFileName(This,pReplyFileName)  ((This)->lpVtbl->GetReplyFileName(This,pReplyFileName)) 
#define IBackgroundCopyJob3_SetCredentials(This,credentials)  ((This)->lpVtbl->SetCredentials(This,credentials)) 
#define IBackgroundCopyJob3_RemoveCredentials(This,Target,Scheme)  ((This)->lpVtbl->RemoveCredentials(This,Target,Scheme)) 
#define IBackgroundCopyJob3_ReplaceRemotePrefix(This,OldPrefix,NewPrefix)  ((This)->lpVtbl->ReplaceRemotePrefix(This,OldPrefix,NewPrefix)) 
#define IBackgroundCopyJob3_AddFileWithRanges(This,RemoteUrl,LocalName,RangeCount,Ranges)  ((This)->lpVtbl->AddFileWithRanges(This,RemoteUrl,LocalName,RangeCount,Ranges)) 
#define IBackgroundCopyJob3_SetFileACLFlags(This,Flags)  ((This)->lpVtbl->SetFileACLFlags(This,Flags)) 
#define IBackgroundCopyJob3_GetFileACLFlags(This,Flags)  ((This)->lpVtbl->GetFileACLFlags(This,Flags)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IBackgroundCopyJob3_INTERFACE_DEFINED__ */


#ifndef __IBackgroundCopyFile2_INTERFACE_DEFINED__
#define __IBackgroundCopyFile2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IBackgroundCopyFile2;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("83e81b93-0873-474d-8a8c-f2018b1a939c")
IBackgroundCopyFile2:public IBackgroundCopyFile
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetFileRanges(DWORD *RangeCount, BG_FILE_RANGE ** Ranges) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetRemoteName(LPCWSTR Val) = 0;
};
#else /* C style interface */

typedef struct IBackgroundCopyFile2Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IBackgroundCopyFile2 *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IBackgroundCopyFile2 *This);
    ULONG (STDMETHODCALLTYPE *Release)(IBackgroundCopyFile2 *This);
    HRESULT (STDMETHODCALLTYPE *GetRemoteName)(IBackgroundCopyFile2 *This, LPWSTR *pVal);
    HRESULT (STDMETHODCALLTYPE *GetLocalName)(IBackgroundCopyFile2 *This, LPWSTR *pVal);
    HRESULT (STDMETHODCALLTYPE *GetProgress)(IBackgroundCopyFile2 *This, BG_FILE_PROGRESS *pVal);
    HRESULT (STDMETHODCALLTYPE *GetFileRanges)(IBackgroundCopyFile2 *This, DWORD *RangeCount, BG_FILE_RANGE ** Ranges);
    HRESULT (STDMETHODCALLTYPE *SetRemoteName)(IBackgroundCopyFile2 *This, LPCWSTR Val);
    END_INTERFACE
} IBackgroundCopyFile2Vtbl;

interface IBackgroundCopyFile2 {
    CONST_VTBL struct IBackgroundCopyFile2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IBackgroundCopyFile2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IBackgroundCopyFile2_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IBackgroundCopyFile2_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IBackgroundCopyFile2_GetRemoteName(This,pVal)  ((This)->lpVtbl->GetRemoteName(This,pVal)) 
#define IBackgroundCopyFile2_GetLocalName(This,pVal)  ((This)->lpVtbl->GetLocalName(This,pVal)) 
#define IBackgroundCopyFile2_GetProgress(This,pVal)  ((This)->lpVtbl->GetProgress(This,pVal)) 
#define IBackgroundCopyFile2_GetFileRanges(This,RangeCount,Ranges)  ((This)->lpVtbl->GetFileRanges(This,RangeCount,Ranges)) 
#define IBackgroundCopyFile2_SetRemoteName(This,Val)  ((This)->lpVtbl->SetRemoteName(This,Val)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IBackgroundCopyFile2_INTERFACE_DEFINED__ */


#ifndef __BackgroundCopyManager2_0_LIBRARY_DEFINED__
#define __BackgroundCopyManager2_0_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_BackgroundCopyManager2_0;
EXTERN_C const CLSID CLSID_BackgroundCopyManager2_0;

#ifdef __cplusplus
class DECLSPEC_UUID("6d18ad12-bde3-4393-b311-099c346e6df9")
BackgroundCopyManager2_0;
#endif
#endif /* __BackgroundCopyManager2_0_LIBRARY_DEFINED__ */

#include "bits2_5.h"

extern RPC_IF_HANDLE __MIDL_itf_bits2_0_0000_0003_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_bits2_0_0000_0003_v0_0_s_ifspec;

#ifdef __cplusplus
}
#endif

#endif /* _BITS2_0_H */


