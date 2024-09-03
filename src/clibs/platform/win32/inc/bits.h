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

#ifndef _BITS_H
#define _BITS_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Forward Declarations */ 

#ifndef __IBackgroundCopyFile_FWD_DEFINED__
#define __IBackgroundCopyFile_FWD_DEFINED__
typedef interface IBackgroundCopyFile IBackgroundCopyFile;
#endif /* __IBackgroundCopyFile_FWD_DEFINED__ */

#ifndef __IEnumBackgroundCopyFiles_FWD_DEFINED__
#define __IEnumBackgroundCopyFiles_FWD_DEFINED__
typedef interface IEnumBackgroundCopyFiles IEnumBackgroundCopyFiles;
#endif /* __IEnumBackgroundCopyFiles_FWD_DEFINED__ */

#ifndef __IBackgroundCopyError_FWD_DEFINED__
#define __IBackgroundCopyError_FWD_DEFINED__
typedef interface IBackgroundCopyError IBackgroundCopyError;
#endif /* __IBackgroundCopyError_FWD_DEFINED__ */

#ifndef __IBackgroundCopyJob_FWD_DEFINED__
#define __IBackgroundCopyJob_FWD_DEFINED__
typedef interface IBackgroundCopyJob IBackgroundCopyJob;
#endif /* __IBackgroundCopyJob_FWD_DEFINED__ */

#ifndef __IEnumBackgroundCopyJobs_FWD_DEFINED__
#define __IEnumBackgroundCopyJobs_FWD_DEFINED__
typedef interface IEnumBackgroundCopyJobs IEnumBackgroundCopyJobs;
#endif /* __IEnumBackgroundCopyJobs_FWD_DEFINED__ */

#ifndef __IBackgroundCopyCallback_FWD_DEFINED__
#define __IBackgroundCopyCallback_FWD_DEFINED__
typedef interface IBackgroundCopyCallback IBackgroundCopyCallback;
#endif /* __IBackgroundCopyCallback_FWD_DEFINED__ */

#ifndef __AsyncIBackgroundCopyCallback_FWD_DEFINED__
#define __AsyncIBackgroundCopyCallback_FWD_DEFINED__
typedef interface AsyncIBackgroundCopyCallback AsyncIBackgroundCopyCallback;
#endif /* __AsyncIBackgroundCopyCallback_FWD_DEFINED__ */

#ifndef __IBackgroundCopyManager_FWD_DEFINED__
#define __IBackgroundCopyManager_FWD_DEFINED__
typedef interface IBackgroundCopyManager IBackgroundCopyManager;
#endif /* __IBackgroundCopyManager_FWD_DEFINED__ */

#ifndef __BackgroundCopyManager_FWD_DEFINED__
#define __BackgroundCopyManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class BackgroundCopyManager BackgroundCopyManager;
#else
typedef struct BackgroundCopyManager BackgroundCopyManager;
#endif /* __cplusplus */

#endif /* __BackgroundCopyManager_FWD_DEFINED__ */

#ifndef __IBackgroundCopyCallback_FWD_DEFINED__
#define __IBackgroundCopyCallback_FWD_DEFINED__
typedef interface IBackgroundCopyCallback IBackgroundCopyCallback;

#endif /* __IBackgroundCopyCallback_FWD_DEFINED__ */

#include "unknwn.h"

#ifdef __cplusplus
extern "C"{
#endif 

#include "bitsmsg.h"

#define BG_SIZE_UNKNOWN  (UINT64)(-1)

extern RPC_IF_HANDLE __MIDL_itf_bits_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_bits_0000_0000_v0_0_s_ifspec;

#ifndef __IBackgroundCopyFile_INTERFACE_DEFINED__
#define __IBackgroundCopyFile_INTERFACE_DEFINED__

typedef struct _BG_FILE_PROGRESS {
    UINT64 BytesTotal;
    UINT64 BytesTransferred;
    BOOL Completed;
} BG_FILE_PROGRESS;

EXTERN_C const IID IID_IBackgroundCopyFile;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("01b7bd23-fb88-4a77-8490-5891d3e4653a")
IBackgroundCopyFile:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetRemoteName(LPWSTR *pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLocalName(LPWSTR *pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProgress(BG_FILE_PROGRESS *pVal) = 0;
};

#else /* C style interface */

typedef struct IBackgroundCopyFileVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IBackgroundCopyFile *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IBackgroundCopyFile *This);
    ULONG (STDMETHODCALLTYPE *Release)(IBackgroundCopyFile *This);
    HRESULT (STDMETHODCALLTYPE *GetRemoteName)(IBackgroundCopyFile *This, LPWSTR *pVal);
    HRESULT (STDMETHODCALLTYPE *GetLocalName)(IBackgroundCopyFile *This, LPWSTR *pVal);
    HRESULT (STDMETHODCALLTYPE *GetProgress)(IBackgroundCopyFile *This, BG_FILE_PROGRESS *pVal);
    END_INTERFACE
} IBackgroundCopyFileVtbl;

interface IBackgroundCopyFile {
    CONST_VTBL struct IBackgroundCopyFileVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IBackgroundCopyFile_QueryInterface(This,riid,ppvObject)   ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IBackgroundCopyFile_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IBackgroundCopyFile_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IBackgroundCopyFile_GetRemoteName(This,pVal)  ((This)->lpVtbl->GetRemoteName(This,pVal)) 
#define IBackgroundCopyFile_GetLocalName(This,pVal)  ((This)->lpVtbl->GetLocalName(This,pVal)) 
#define IBackgroundCopyFile_GetProgress(This,pVal)  ((This)->lpVtbl->GetProgress(This,pVal)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IBackgroundCopyFile_INTERFACE_DEFINED__ */


#ifndef __IEnumBackgroundCopyFiles_INTERFACE_DEFINED__
#define __IEnumBackgroundCopyFiles_INTERFACE_DEFINED__

EXTERN_C const IID IID_IEnumBackgroundCopyFiles;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("ca51e165-c365-424c-8d41-24aaa4ff3c40")
IEnumBackgroundCopyFiles:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Next(ULONG celt, IBackgroundCopyFile ** rgelt, ULONG *pceltFetched) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(ULONG celt) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumBackgroundCopyFiles ** ppenum) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCount(ULONG *puCount) = 0;
};

#else /* C style interface */

typedef struct IEnumBackgroundCopyFilesVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IEnumBackgroundCopyFiles *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IEnumBackgroundCopyFiles *This);
    ULONG (STDMETHODCALLTYPE *Release)(IEnumBackgroundCopyFiles *This);
    HRESULT (STDMETHODCALLTYPE *Next)(IEnumBackgroundCopyFiles *This, ULONG celt, IBackgroundCopyFile ** rgelt, ULONG *pceltFetched);
    HRESULT (STDMETHODCALLTYPE *Skip)(IEnumBackgroundCopyFiles *This, ULONG celt);
    HRESULT (STDMETHODCALLTYPE *Reset)(IEnumBackgroundCopyFiles *This);
    HRESULT (STDMETHODCALLTYPE *Clone)(IEnumBackgroundCopyFiles *This, IEnumBackgroundCopyFiles ** ppenum);
    HRESULT (STDMETHODCALLTYPE *GetCount)(IEnumBackgroundCopyFiles *This, ULONG *puCount);
    END_INTERFACE
} IEnumBackgroundCopyFilesVtbl;

interface IEnumBackgroundCopyFiles {
    CONST_VTBL struct IEnumBackgroundCopyFilesVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEnumBackgroundCopyFiles_QueryInterface(This,riid,ppvObject)   ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IEnumBackgroundCopyFiles_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IEnumBackgroundCopyFiles_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IEnumBackgroundCopyFiles_Next(This,celt,rgelt,pceltFetched)  ((This)->lpVtbl->Next(This,celt,rgelt,pceltFetched)) 
#define IEnumBackgroundCopyFiles_Skip(This,celt)  ((This)->lpVtbl->Skip(This,celt)) 
#define IEnumBackgroundCopyFiles_Reset(This)  ((This)->lpVtbl->Reset(This)) 
#define IEnumBackgroundCopyFiles_Clone(This,ppenum)  ((This)->lpVtbl->Clone(This,ppenum)) 
#define IEnumBackgroundCopyFiles_GetCount(This,puCount)  ((This)->lpVtbl->GetCount(This,puCount)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IEnumBackgroundCopyFiles_INTERFACE_DEFINED__ */


#ifndef __IBackgroundCopyError_INTERFACE_DEFINED__
#define __IBackgroundCopyError_INTERFACE_DEFINED__

typedef enum __MIDL_IBackgroundCopyError_0001 {
    BG_ERROR_CONTEXT_NONE = 0,
    BG_ERROR_CONTEXT_UNKNOWN = 1,
    BG_ERROR_CONTEXT_GENERAL_QUEUE_MANAGER = 2,
    BG_ERROR_CONTEXT_QUEUE_MANAGER_NOTIFICATION = 3,
    BG_ERROR_CONTEXT_LOCAL_FILE = 4,
    BG_ERROR_CONTEXT_REMOTE_FILE = 5,
    BG_ERROR_CONTEXT_GENERAL_TRANSPORT = 6,
    BG_ERROR_CONTEXT_REMOTE_APPLICATION = 7
} BG_ERROR_CONTEXT;

EXTERN_C const IID IID_IBackgroundCopyError;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("19c613a0-fcb8-4f28-81ae-897c3d078f81")
IBackgroundCopyError:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetError(BG_ERROR_CONTEXT *pContext, HRESULT *pCode) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFile(IBackgroundCopyFile ** pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetErrorDescription(DWORD LanguageId, LPWSTR *pErrorDescription) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetErrorContextDescription(DWORD LanguageId, LPWSTR *pContextDescription) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProtocol(LPWSTR *pProtocol) = 0;
};

#else /* C style interface */

typedef struct IBackgroundCopyErrorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IBackgroundCopyError *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IBackgroundCopyError *This);
    ULONG (STDMETHODCALLTYPE *Release)(IBackgroundCopyError *This);
    HRESULT (STDMETHODCALLTYPE *GetError)(IBackgroundCopyError *This, BG_ERROR_CONTEXT *pContext, HRESULT *pCode);
    HRESULT (STDMETHODCALLTYPE *GetFile)(IBackgroundCopyError *This, IBackgroundCopyFile ** pVal);
    HRESULT (STDMETHODCALLTYPE *GetErrorDescription)(IBackgroundCopyError *This, DWORD LanguageId, LPWSTR *pErrorDescription);
    HRESULT (STDMETHODCALLTYPE *GetErrorContextDescription)(IBackgroundCopyError *This, DWORD LanguageId, LPWSTR *pContextDescription);
    HRESULT (STDMETHODCALLTYPE *GetProtocol)(IBackgroundCopyError *This, LPWSTR *pProtocol);
    END_INTERFACE
} IBackgroundCopyErrorVtbl;

interface IBackgroundCopyError {
    CONST_VTBL struct IBackgroundCopyErrorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IBackgroundCopyError_QueryInterface(This,riid,ppvObject)   ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IBackgroundCopyError_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IBackgroundCopyError_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IBackgroundCopyError_GetError(This,pContext,pCode)  ((This)->lpVtbl->GetError(This,pContext,pCode)) 
#define IBackgroundCopyError_GetFile(This,pVal)  ((This)->lpVtbl->GetFile(This,pVal)) 
#define IBackgroundCopyError_GetErrorDescription(This,LanguageId,pErrorDescription)  ((This)->lpVtbl->GetErrorDescription(This,LanguageId,pErrorDescription)) 
#define IBackgroundCopyError_GetErrorContextDescription(This,LanguageId,pContextDescription)  ((This)->lpVtbl->GetErrorContextDescription(This,LanguageId,pContextDescription)) 
#define IBackgroundCopyError_GetProtocol(This,pProtocol)  ((This)->lpVtbl->GetProtocol(This,pProtocol)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IBackgroundCopyError_INTERFACE_DEFINED__ */


#ifndef __IBackgroundCopyJob_INTERFACE_DEFINED__
#define __IBackgroundCopyJob_INTERFACE_DEFINED__

typedef struct _BG_FILE_INFO {
    LPWSTR RemoteName;
    LPWSTR LocalName;
} BG_FILE_INFO;

typedef struct _BG_JOB_PROGRESS {
    UINT64 BytesTotal;
    UINT64 BytesTransferred;
    ULONG FilesTotal;
    ULONG FilesTransferred;
} BG_JOB_PROGRESS;

typedef struct _BG_JOB_TIMES {
    FILETIME CreationTime;
    FILETIME ModificationTime;
    FILETIME TransferCompletionTime;
} BG_JOB_TIMES;

typedef enum __MIDL_IBackgroundCopyJob_0001 {
    BG_JOB_PRIORITY_FOREGROUND = 0,
    BG_JOB_PRIORITY_HIGH = (BG_JOB_PRIORITY_FOREGROUND + 1),
    BG_JOB_PRIORITY_NORMAL = (BG_JOB_PRIORITY_HIGH + 1),
    BG_JOB_PRIORITY_LOW = (BG_JOB_PRIORITY_NORMAL + 1)
} BG_JOB_PRIORITY;

typedef enum __MIDL_IBackgroundCopyJob_0002 {
    BG_JOB_STATE_QUEUED = 0,
    BG_JOB_STATE_CONNECTING = (BG_JOB_STATE_QUEUED + 1),
    BG_JOB_STATE_TRANSFERRING = (BG_JOB_STATE_CONNECTING + 1),
    BG_JOB_STATE_SUSPENDED = (BG_JOB_STATE_TRANSFERRING + 1),
    BG_JOB_STATE_ERROR = (BG_JOB_STATE_SUSPENDED + 1),
    BG_JOB_STATE_TRANSIENT_ERROR = (BG_JOB_STATE_ERROR + 1),
    BG_JOB_STATE_TRANSFERRED = (BG_JOB_STATE_TRANSIENT_ERROR + 1),
    BG_JOB_STATE_ACKNOWLEDGED = (BG_JOB_STATE_TRANSFERRED + 1),
    BG_JOB_STATE_CANCELLED = (BG_JOB_STATE_ACKNOWLEDGED + 1)
} BG_JOB_STATE;

typedef enum __MIDL_IBackgroundCopyJob_0003 {
    BG_JOB_TYPE_DOWNLOAD = 0,
    BG_JOB_TYPE_UPLOAD = (BG_JOB_TYPE_DOWNLOAD + 1),
    BG_JOB_TYPE_UPLOAD_REPLY = (BG_JOB_TYPE_UPLOAD + 1)
} BG_JOB_TYPE;

typedef enum __MIDL_IBackgroundCopyJob_0004 {
    BG_JOB_PROXY_USAGE_PRECONFIG = 0,
    BG_JOB_PROXY_USAGE_NO_PROXY = (BG_JOB_PROXY_USAGE_PRECONFIG + 1),
    BG_JOB_PROXY_USAGE_OVERRIDE = (BG_JOB_PROXY_USAGE_NO_PROXY + 1),
    BG_JOB_PROXY_USAGE_AUTODETECT = (BG_JOB_PROXY_USAGE_OVERRIDE + 1)
} BG_JOB_PROXY_USAGE;

EXTERN_C const IID IID_IBackgroundCopyJob;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("37668d37-507e-4160-9316-26306d150b12")
IBackgroundCopyJob:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE AddFileSet(ULONG cFileCount, BG_FILE_INFO *pFileSet) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddFile(LPCWSTR RemoteUrl, LPCWSTR LocalName) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumFiles(IEnumBackgroundCopyFiles ** pEnum) = 0;
    virtual HRESULT STDMETHODCALLTYPE Suspend(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Resume(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Complete(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetId(GUID *pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetType(BG_JOB_TYPE *pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProgress(BG_JOB_PROGRESS *pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTimes(BG_JOB_TIMES *pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetState(BG_JOB_STATE *pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetError(IBackgroundCopyError ** ppError) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOwner(LPWSTR *pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDisplayName(LPCWSTR Val) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDisplayName(LPWSTR *pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDescription(LPCWSTR Val) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDescription(LPWSTR *pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPriority(BG_JOB_PRIORITY Val) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPriority(BG_JOB_PRIORITY *pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetNotifyFlags(ULONG Val) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNotifyFlags(ULONG *pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetNotifyInterface(IUnknown *Val) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNotifyInterface(IUnknown ** pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMinimumRetryDelay(ULONG Seconds) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMinimumRetryDelay(ULONG *Seconds) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetNoProgressTimeout(ULONG Seconds) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNoProgressTimeout(ULONG *Seconds) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetErrorCount(ULONG *Errors) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProxySettings(BG_JOB_PROXY_USAGE ProxyUsage, const WCHAR *ProxyList, const WCHAR *ProxyBypassList) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProxySettings(BG_JOB_PROXY_USAGE *pProxyUsage, LPWSTR *pProxyList, LPWSTR *pProxyBypassList) = 0;
    virtual HRESULT STDMETHODCALLTYPE TakeOwnership(void) = 0;
};
#else /* C style interface */

typedef struct IBackgroundCopyJobVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IBackgroundCopyJob *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IBackgroundCopyJob *This);
    ULONG (STDMETHODCALLTYPE *Release)(IBackgroundCopyJob *This);
    HRESULT (STDMETHODCALLTYPE *AddFileSet)(IBackgroundCopyJob *This, ULONG cFileCount, BG_FILE_INFO *pFileSet);
    HRESULT (STDMETHODCALLTYPE *AddFile)(IBackgroundCopyJob *This, LPCWSTR RemoteUrl, LPCWSTR LocalName);
    HRESULT (STDMETHODCALLTYPE *EnumFiles)(IBackgroundCopyJob *This, IEnumBackgroundCopyFiles ** pEnum);
    HRESULT (STDMETHODCALLTYPE *Suspend)(IBackgroundCopyJob *This);
    HRESULT (STDMETHODCALLTYPE *Resume)(IBackgroundCopyJob *This);
    HRESULT (STDMETHODCALLTYPE *Cancel)(IBackgroundCopyJob *This);
    HRESULT (STDMETHODCALLTYPE *Complete)(IBackgroundCopyJob *This);
    HRESULT (STDMETHODCALLTYPE *GetId)(IBackgroundCopyJob *This, GUID *pVal);
    HRESULT (STDMETHODCALLTYPE *GetType)(IBackgroundCopyJob *This, BG_JOB_TYPE *pVal);
    HRESULT (STDMETHODCALLTYPE *GetProgress)(IBackgroundCopyJob *This, BG_JOB_PROGRESS *pVal);
    HRESULT (STDMETHODCALLTYPE *GetTimes)(IBackgroundCopyJob *This, BG_JOB_TIMES *pVal);
    HRESULT (STDMETHODCALLTYPE *GetState)(IBackgroundCopyJob *This, BG_JOB_STATE *pVal);
    HRESULT (STDMETHODCALLTYPE *GetError)(IBackgroundCopyJob *This, IBackgroundCopyError ** ppError);
    HRESULT (STDMETHODCALLTYPE *GetOwner)(IBackgroundCopyJob *This, LPWSTR *pVal);
    HRESULT (STDMETHODCALLTYPE *SetDisplayName)(IBackgroundCopyJob *This, LPCWSTR Val);
    HRESULT (STDMETHODCALLTYPE *GetDisplayName)(IBackgroundCopyJob *This, LPWSTR *pVal);
    HRESULT (STDMETHODCALLTYPE *SetDescription)(IBackgroundCopyJob *This, LPCWSTR Val);
    HRESULT (STDMETHODCALLTYPE *GetDescription)(IBackgroundCopyJob *This, LPWSTR *pVal);
    HRESULT (STDMETHODCALLTYPE *SetPriority)(IBackgroundCopyJob *This, BG_JOB_PRIORITY Val);
    HRESULT (STDMETHODCALLTYPE *GetPriority)(IBackgroundCopyJob *This, BG_JOB_PRIORITY *pVal);
    HRESULT (STDMETHODCALLTYPE *SetNotifyFlags)(IBackgroundCopyJob *This, ULONG Val);
    HRESULT (STDMETHODCALLTYPE *GetNotifyFlags)(IBackgroundCopyJob *This, ULONG *pVal);
    HRESULT (STDMETHODCALLTYPE *SetNotifyInterface)(IBackgroundCopyJob *This, IUnknown *Val);
    HRESULT (STDMETHODCALLTYPE *GetNotifyInterface)(IBackgroundCopyJob *This, IUnknown ** pVal);
    HRESULT (STDMETHODCALLTYPE *SetMinimumRetryDelay)(IBackgroundCopyJob *This, ULONG Seconds);
    HRESULT (STDMETHODCALLTYPE *GetMinimumRetryDelay)(IBackgroundCopyJob *This, ULONG *Seconds);
    HRESULT (STDMETHODCALLTYPE *SetNoProgressTimeout)(IBackgroundCopyJob *This, ULONG Seconds);
    HRESULT (STDMETHODCALLTYPE *GetNoProgressTimeout)(IBackgroundCopyJob *This, ULONG *Seconds);
    HRESULT (STDMETHODCALLTYPE *GetErrorCount)(IBackgroundCopyJob *This, ULONG *Errors);
    HRESULT (STDMETHODCALLTYPE *SetProxySettings)(IBackgroundCopyJob *This, BG_JOB_PROXY_USAGE ProxyUsage, const WCHAR *ProxyList, const WCHAR *ProxyBypassList);
    HRESULT (STDMETHODCALLTYPE *GetProxySettings)(IBackgroundCopyJob *This, BG_JOB_PROXY_USAGE *pProxyUsage, LPWSTR *pProxyList, LPWSTR *pProxyBypassList);
    HRESULT (STDMETHODCALLTYPE *TakeOwnership)(IBackgroundCopyJob *This);
    END_INTERFACE
} IBackgroundCopyJobVtbl;

interface IBackgroundCopyJob {
    CONST_VTBL struct IBackgroundCopyJobVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IBackgroundCopyJob_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IBackgroundCopyJob_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IBackgroundCopyJob_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IBackgroundCopyJob_AddFileSet(This,cFileCount,pFileSet)  ((This)->lpVtbl->AddFileSet(This,cFileCount,pFileSet)) 
#define IBackgroundCopyJob_AddFile(This,RemoteUrl,LocalName)  ((This)->lpVtbl->AddFile(This,RemoteUrl,LocalName)) 
#define IBackgroundCopyJob_EnumFiles(This,pEnum)  ((This)->lpVtbl->EnumFiles(This,pEnum)) 
#define IBackgroundCopyJob_Suspend(This)  ((This)->lpVtbl->Suspend(This)) 
#define IBackgroundCopyJob_Resume(This)  ((This)->lpVtbl->Resume(This)) 
#define IBackgroundCopyJob_Cancel(This)  ((This)->lpVtbl->Cancel(This)) 
#define IBackgroundCopyJob_Complete(This)  ((This)->lpVtbl->Complete(This)) 
#define IBackgroundCopyJob_GetId(This,pVal)  ((This)->lpVtbl->GetId(This,pVal)) 
#define IBackgroundCopyJob_GetType(This,pVal)  ((This)->lpVtbl->GetType(This,pVal)) 
#define IBackgroundCopyJob_GetProgress(This,pVal)  ((This)->lpVtbl->GetProgress(This,pVal)) 
#define IBackgroundCopyJob_GetTimes(This,pVal)  ((This)->lpVtbl->GetTimes(This,pVal)) 
#define IBackgroundCopyJob_GetState(This,pVal)  ((This)->lpVtbl->GetState(This,pVal)) 
#define IBackgroundCopyJob_GetError(This,ppError)  ((This)->lpVtbl->GetError(This,ppError)) 
#define IBackgroundCopyJob_GetOwner(This,pVal)  ((This)->lpVtbl->GetOwner(This,pVal)) 
#define IBackgroundCopyJob_SetDisplayName(This,Val)  ((This)->lpVtbl->SetDisplayName(This,Val)) 
#define IBackgroundCopyJob_GetDisplayName(This,pVal)  ((This)->lpVtbl->GetDisplayName(This,pVal)) 
#define IBackgroundCopyJob_SetDescription(This,Val)  ((This)->lpVtbl->SetDescription(This,Val)) 
#define IBackgroundCopyJob_GetDescription(This,pVal)  ((This)->lpVtbl->GetDescription(This,pVal)) 
#define IBackgroundCopyJob_SetPriority(This,Val)  ((This)->lpVtbl->SetPriority(This,Val)) 
#define IBackgroundCopyJob_GetPriority(This,pVal)  ((This)->lpVtbl->GetPriority(This,pVal)) 
#define IBackgroundCopyJob_SetNotifyFlags(This,Val)  ((This)->lpVtbl->SetNotifyFlags(This,Val)) 
#define IBackgroundCopyJob_GetNotifyFlags(This,pVal)  ((This)->lpVtbl->GetNotifyFlags(This,pVal)) 
#define IBackgroundCopyJob_SetNotifyInterface(This,Val)  ((This)->lpVtbl->SetNotifyInterface(This,Val)) 
#define IBackgroundCopyJob_GetNotifyInterface(This,pVal)  ((This)->lpVtbl->GetNotifyInterface(This,pVal)) 
#define IBackgroundCopyJob_SetMinimumRetryDelay(This,Seconds)  ((This)->lpVtbl->SetMinimumRetryDelay(This,Seconds)) 
#define IBackgroundCopyJob_GetMinimumRetryDelay(This,Seconds)  ((This)->lpVtbl->GetMinimumRetryDelay(This,Seconds)) 
#define IBackgroundCopyJob_SetNoProgressTimeout(This,Seconds)  ((This)->lpVtbl->SetNoProgressTimeout(This,Seconds)) 
#define IBackgroundCopyJob_GetNoProgressTimeout(This,Seconds)  ((This)->lpVtbl->GetNoProgressTimeout(This,Seconds)) 
#define IBackgroundCopyJob_GetErrorCount(This,Errors)  ((This)->lpVtbl->GetErrorCount(This,Errors)) 
#define IBackgroundCopyJob_SetProxySettings(This,ProxyUsage,ProxyList,ProxyBypassList)  ((This)->lpVtbl->SetProxySettings(This,ProxyUsage,ProxyList,ProxyBypassList)) 
#define IBackgroundCopyJob_GetProxySettings(This,pProxyUsage,pProxyList,pProxyBypassList)  ((This)->lpVtbl->GetProxySettings(This,pProxyUsage,pProxyList,pProxyBypassList)) 
#define IBackgroundCopyJob_TakeOwnership(This)  ((This)->lpVtbl->TakeOwnership(This)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IBackgroundCopyJob_INTERFACE_DEFINED__ */


#ifndef __IEnumBackgroundCopyJobs_INTERFACE_DEFINED__
#define __IEnumBackgroundCopyJobs_INTERFACE_DEFINED__

EXTERN_C const IID IID_IEnumBackgroundCopyJobs;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("1af4f612-3b71-466f-8f58-7b6f73ac57ad")
IEnumBackgroundCopyJobs:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Next(ULONG celt, IBackgroundCopyJob ** rgelt, ULONG *pceltFetched) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(ULONG celt) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumBackgroundCopyJobs ** ppenum) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCount(ULONG *puCount) = 0;
};
#else /* C style interface */

typedef struct IEnumBackgroundCopyJobsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IEnumBackgroundCopyJobs *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IEnumBackgroundCopyJobs *This);
    ULONG (STDMETHODCALLTYPE *Release)(IEnumBackgroundCopyJobs *This);
    HRESULT (STDMETHODCALLTYPE *Next)(IEnumBackgroundCopyJobs *This, ULONG celt, IBackgroundCopyJob ** rgelt, ULONG *pceltFetched);
    HRESULT (STDMETHODCALLTYPE *Skip)(IEnumBackgroundCopyJobs *This, ULONG celt);
    HRESULT (STDMETHODCALLTYPE *Reset)(IEnumBackgroundCopyJobs *This);
    HRESULT (STDMETHODCALLTYPE *Clone)(IEnumBackgroundCopyJobs *This, IEnumBackgroundCopyJobs ** ppenum);
    HRESULT (STDMETHODCALLTYPE *GetCount)(IEnumBackgroundCopyJobs *This, ULONG *puCount);
    END_INTERFACE
} IEnumBackgroundCopyJobsVtbl;

interface IEnumBackgroundCopyJobs {
    CONST_VTBL struct IEnumBackgroundCopyJobsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEnumBackgroundCopyJobs_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IEnumBackgroundCopyJobs_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IEnumBackgroundCopyJobs_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IEnumBackgroundCopyJobs_Next(This,celt,rgelt,pceltFetched)  ((This)->lpVtbl->Next(This,celt,rgelt,pceltFetched)) 
#define IEnumBackgroundCopyJobs_Skip(This,celt)  ((This)->lpVtbl->Skip(This,celt)) 
#define IEnumBackgroundCopyJobs_Reset(This)  ((This)->lpVtbl->Reset(This)) 
#define IEnumBackgroundCopyJobs_Clone(This,ppenum)  ((This)->lpVtbl->Clone(This,ppenum)) 
#define IEnumBackgroundCopyJobs_GetCount(This,puCount)  ((This)->lpVtbl->GetCount(This,puCount)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IEnumBackgroundCopyJobs_INTERFACE_DEFINED__ */


#define BG_NOTIFY_JOB_TRANSFERRED    0x0001
#define BG_NOTIFY_JOB_ERROR          0x0002
#define BG_NOTIFY_DISABLE            0x0004
#define BG_NOTIFY_JOB_MODIFICATION   0x0008
#define BG_NOTIFY_FILE_TRANSFERRED   0x0010

extern RPC_IF_HANDLE __MIDL_itf_bits_0000_0005_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_bits_0000_0005_v0_0_s_ifspec;

#ifndef __IBackgroundCopyCallback_INTERFACE_DEFINED__
#define __IBackgroundCopyCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_IBackgroundCopyCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("97ea99c7-0186-4ad4-8df9-c5b4e0ed6b22")
IBackgroundCopyCallback:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE JobTransferred(IBackgroundCopyJob *pJob) = 0;
    virtual HRESULT STDMETHODCALLTYPE JobError(IBackgroundCopyJob *pJob, IBackgroundCopyError *pError) = 0;
    virtual HRESULT STDMETHODCALLTYPE JobModification(IBackgroundCopyJob *pJob, DWORD dwReserved) = 0;
};
#else /* C style interface */

typedef struct IBackgroundCopyCallbackVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IBackgroundCopyCallback *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IBackgroundCopyCallback *This);
    ULONG (STDMETHODCALLTYPE *Release)(IBackgroundCopyCallback *This);
    HRESULT (STDMETHODCALLTYPE *JobTransferred)(IBackgroundCopyCallback *This, IBackgroundCopyJob *pJob);
    HRESULT (STDMETHODCALLTYPE *JobError)(IBackgroundCopyCallback *This, IBackgroundCopyJob *pJob, IBackgroundCopyError *pError);
    HRESULT (STDMETHODCALLTYPE *JobModification)(IBackgroundCopyCallback *This, IBackgroundCopyJob *pJob, DWORD dwReserved);
    END_INTERFACE
} IBackgroundCopyCallbackVtbl;

interface IBackgroundCopyCallback {
    CONST_VTBL struct IBackgroundCopyCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IBackgroundCopyCallback_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IBackgroundCopyCallback_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IBackgroundCopyCallback_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IBackgroundCopyCallback_JobTransferred(This,pJob)  ((This)->lpVtbl->JobTransferred(This,pJob)) 
#define IBackgroundCopyCallback_JobError(This,pJob,pError)  ((This)->lpVtbl->JobError(This,pJob,pError)) 
#define IBackgroundCopyCallback_JobModification(This,pJob,dwReserved)  ((This)->lpVtbl->JobModification(This,pJob,dwReserved)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IBackgroundCopyCallback_INTERFACE_DEFINED__ */


#ifndef __AsyncIBackgroundCopyCallback_INTERFACE_DEFINED__
#define __AsyncIBackgroundCopyCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_AsyncIBackgroundCopyCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("ca29d251-b4bb-4679-a3d9-ae8006119d54")
AsyncIBackgroundCopyCallback:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Begin_JobTransferred(IBackgroundCopyJob *pJob) = 0;
    virtual HRESULT STDMETHODCALLTYPE Finish_JobTransferred(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Begin_JobError(IBackgroundCopyJob *pJob, IBackgroundCopyError *pError) = 0;
    virtual HRESULT STDMETHODCALLTYPE Finish_JobError(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Begin_JobModification(IBackgroundCopyJob *pJob, DWORD dwReserved) = 0;
    virtual HRESULT STDMETHODCALLTYPE Finish_JobModification(void) = 0;
};
#else /* C style interface */

typedef struct AsyncIBackgroundCopyCallbackVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(AsyncIBackgroundCopyCallback *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(AsyncIBackgroundCopyCallback *This);
    ULONG (STDMETHODCALLTYPE *Release)(AsyncIBackgroundCopyCallback *This);
    HRESULT (STDMETHODCALLTYPE *Begin_JobTransferred)(AsyncIBackgroundCopyCallback *This, IBackgroundCopyJob *pJob);
    HRESULT (STDMETHODCALLTYPE *Finish_JobTransferred)(AsyncIBackgroundCopyCallback *This);
    HRESULT (STDMETHODCALLTYPE *Begin_JobError)(AsyncIBackgroundCopyCallback *This, IBackgroundCopyJob *pJob, IBackgroundCopyError *pError);
    HRESULT (STDMETHODCALLTYPE *Finish_JobError)(AsyncIBackgroundCopyCallback *This);
    HRESULT (STDMETHODCALLTYPE *Begin_JobModification)(AsyncIBackgroundCopyCallback *This, IBackgroundCopyJob *pJob, DWORD dwReserved);
    HRESULT (STDMETHODCALLTYPE *Finish_JobModification)(AsyncIBackgroundCopyCallback *This);
    END_INTERFACE
} AsyncIBackgroundCopyCallbackVtbl;

interface AsyncIBackgroundCopyCallback {
    CONST_VTBL struct AsyncIBackgroundCopyCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define AsyncIBackgroundCopyCallback_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define AsyncIBackgroundCopyCallback_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define AsyncIBackgroundCopyCallback_Release(This)  ((This)->lpVtbl->Release(This)) 
#define AsyncIBackgroundCopyCallback_Begin_JobTransferred(This,pJob)  ((This)->lpVtbl->Begin_JobTransferred(This,pJob)) 
#define AsyncIBackgroundCopyCallback_Finish_JobTransferred(This)  ((This)->lpVtbl->Finish_JobTransferred(This)) 
#define AsyncIBackgroundCopyCallback_Begin_JobError(This,pJob,pError)  ((This)->lpVtbl->Begin_JobError(This,pJob,pError)) 
#define AsyncIBackgroundCopyCallback_Finish_JobError(This)  ((This)->lpVtbl->Finish_JobError(This)) 
#define AsyncIBackgroundCopyCallback_Begin_JobModification(This,pJob,dwReserved)  ((This)->lpVtbl->Begin_JobModification(This,pJob,dwReserved)) 
#define AsyncIBackgroundCopyCallback_Finish_JobModification(This)  ((This)->lpVtbl->Finish_JobModification(This)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __AsyncIBackgroundCopyCallback_INTERFACE_DEFINED__ */


#ifndef __IBackgroundCopyManager_INTERFACE_DEFINED__
#define __IBackgroundCopyManager_INTERFACE_DEFINED__

#define BG_JOB_ENUM_ALL_USERS  0x0001

EXTERN_C const IID IID_IBackgroundCopyManager;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("5ce34c0d-0dc9-4c1f-897c-daa1b78cee7c")
IBackgroundCopyManager:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateJob(LPCWSTR DisplayName, BG_JOB_TYPE Type, GUID *pJobId, IBackgroundCopyJob ** ppJob) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetJob(REFGUID jobID, IBackgroundCopyJob ** ppJob) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumJobs(DWORD dwFlags, IEnumBackgroundCopyJobs ** ppEnum) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetErrorDescription(HRESULT hResult, DWORD LanguageId, LPWSTR *pErrorDescription) = 0;
};

#else /* C style interface */

typedef struct IBackgroundCopyManagerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IBackgroundCopyManager *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IBackgroundCopyManager *This);
    ULONG (STDMETHODCALLTYPE *Release)(IBackgroundCopyManager *This);
    HRESULT (STDMETHODCALLTYPE *CreateJob)(IBackgroundCopyManager *This, LPCWSTR DisplayName, BG_JOB_TYPE Type, GUID *pJobId, IBackgroundCopyJob ** ppJob);
    HRESULT (STDMETHODCALLTYPE *GetJob)(IBackgroundCopyManager *This, REFGUID jobID, IBackgroundCopyJob ** ppJob);
    HRESULT (STDMETHODCALLTYPE *EnumJobs)(IBackgroundCopyManager *This, DWORD dwFlags, IEnumBackgroundCopyJobs ** ppEnum);
    HRESULT (STDMETHODCALLTYPE *GetErrorDescription)(IBackgroundCopyManager *This, HRESULT hResult, DWORD LanguageId, LPWSTR *pErrorDescription);
    END_INTERFACE
} IBackgroundCopyManagerVtbl;

interface IBackgroundCopyManager {
    CONST_VTBL struct IBackgroundCopyManagerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IBackgroundCopyManager_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IBackgroundCopyManager_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IBackgroundCopyManager_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IBackgroundCopyManager_CreateJob(This,DisplayName,Type,pJobId,ppJob)  ((This)->lpVtbl->CreateJob(This,DisplayName,Type,pJobId,ppJob)) 
#define IBackgroundCopyManager_GetJob(This,jobID,ppJob)  ((This)->lpVtbl->GetJob(This,jobID,ppJob)) 
#define IBackgroundCopyManager_EnumJobs(This,dwFlags,ppEnum)  ((This)->lpVtbl->EnumJobs(This,dwFlags,ppEnum)) 
#define IBackgroundCopyManager_GetErrorDescription(This,hResult,LanguageId,pErrorDescription)  ((This)->lpVtbl->GetErrorDescription(This,hResult,LanguageId,pErrorDescription)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IBackgroundCopyManager_INTERFACE_DEFINED__ */


#ifndef __BackgroundCopyManager_LIBRARY_DEFINED__
#define __BackgroundCopyManager_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_BackgroundCopyManager;
EXTERN_C const CLSID CLSID_BackgroundCopyManager;

#ifdef __cplusplus
class DECLSPEC_UUID("4991d34b-80a1-4291-83b6-3328366b9097")
BackgroundCopyManager;
#endif
#endif /* __BackgroundCopyManager_LIBRARY_DEFINED__ */

#include "bits1_5.h"

extern RPC_IF_HANDLE __MIDL_itf_bits_0000_0008_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_bits_0000_0008_v0_0_s_ifspec;

#ifdef __cplusplus
}
#endif

#endif /* _BITS_H */
