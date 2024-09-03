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

#ifndef _BITS4_0_H
#define _BITS4_0_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Forward Declarations */ 

#ifndef __IBitsTokenOptions_FWD_DEFINED__
#define __IBitsTokenOptions_FWD_DEFINED__
typedef interface IBitsTokenOptions IBitsTokenOptions;
#endif /* __IBitsTokenOptions_FWD_DEFINED__ */

#ifndef __IBackgroundCopyFile4_FWD_DEFINED__
#define __IBackgroundCopyFile4_FWD_DEFINED__
typedef interface IBackgroundCopyFile4 IBackgroundCopyFile4;
#endif /* __IBackgroundCopyFile4_FWD_DEFINED__ */

#ifndef __BackgroundCopyManager4_0_FWD_DEFINED__
#define __BackgroundCopyManager4_0_FWD_DEFINED__

#ifdef __cplusplus
typedef class BackgroundCopyManager4_0 BackgroundCopyManager4_0;
#else
typedef struct BackgroundCopyManager4_0 BackgroundCopyManager4_0;
#endif /* __cplusplus */

#endif /* __BackgroundCopyManager4_0_FWD_DEFINED__ */

#ifndef __IBackgroundCopyFile4_FWD_DEFINED__
#define __IBackgroundCopyFile4_FWD_DEFINED__
typedef interface IBackgroundCopyFile4 IBackgroundCopyFile4;
#endif /* __IBackgroundCopyFile4_FWD_DEFINED__ */

#ifndef __IBitsTokenOptions_FWD_DEFINED__
#define __IBitsTokenOptions_FWD_DEFINED__
typedef interface IBitsTokenOptions IBitsTokenOptions;
#endif /* __IBitsTokenOptions_FWD_DEFINED__ */

/* header files for imported files */
#include "bits.h"
#include "bits1_5.h"
#include "bits2_0.h"
#include "bits2_5.h"
#include "bits3_0.h"

#ifdef __cplusplus
extern "C"{
#endif 

extern RPC_IF_HANDLE __MIDL_itf_bits4_0_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_bits4_0_0000_0000_v0_0_s_ifspec;

#ifndef __IBitsTokenOptions_INTERFACE_DEFINED__
#define __IBitsTokenOptions_INTERFACE_DEFINED__

#define BG_TOKEN_LOCAL_FILE  0x0001
#define BG_TOKEN_NETWORK     0x0002

EXTERN_C const IID IID_IBitsTokenOptions;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("9a2584c3-f7d2-457a-9a5e-22b67bffc7d2")
IBitsTokenOptions:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetHelperTokenFlags(DWORD UsageFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHelperTokenFlags(DWORD *pFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetHelperToken(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE ClearHelperToken(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHelperTokenSid(LPWSTR *pSid) = 0;
};
#else /* C style interface */

typedef struct IBitsTokenOptionsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IBitsTokenOptions *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IBitsTokenOptions *This);
    ULONG (STDMETHODCALLTYPE *Release)(IBitsTokenOptions *This);
    HRESULT (STDMETHODCALLTYPE *SetHelperTokenFlags)(IBitsTokenOptions *This, DWORD UsageFlags);
    HRESULT (STDMETHODCALLTYPE *GetHelperTokenFlags)(IBitsTokenOptions *This, DWORD *pFlags);
    HRESULT (STDMETHODCALLTYPE *SetHelperToken)(IBitsTokenOptions *This);
    HRESULT (STDMETHODCALLTYPE *ClearHelperToken)(IBitsTokenOptions *This);
    HRESULT (STDMETHODCALLTYPE *GetHelperTokenSid)(IBitsTokenOptions *This, LPWSTR *pSid);
    END_INTERFACE
} IBitsTokenOptionsVtbl;

interface IBitsTokenOptions {
    CONST_VTBL struct IBitsTokenOptionsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IBitsTokenOptions_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IBitsTokenOptions_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IBitsTokenOptions_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IBitsTokenOptions_SetHelperTokenFlags(This,UsageFlags)  ((This)->lpVtbl->SetHelperTokenFlags(This,UsageFlags)) 
#define IBitsTokenOptions_GetHelperTokenFlags(This,pFlags)  ((This)->lpVtbl->GetHelperTokenFlags(This,pFlags)) 
#define IBitsTokenOptions_SetHelperToken(This)  ((This)->lpVtbl->SetHelperToken(This)) 
#define IBitsTokenOptions_ClearHelperToken(This)  ((This)->lpVtbl->ClearHelperToken(This)) 
#define IBitsTokenOptions_GetHelperTokenSid(This,pSid)  ((This)->lpVtbl->GetHelperTokenSid(This,pSid)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IBitsTokenOptions_INTERFACE_DEFINED__ */


#ifndef __IBackgroundCopyFile4_INTERFACE_DEFINED__
#define __IBackgroundCopyFile4_INTERFACE_DEFINED__

EXTERN_C const IID IID_IBackgroundCopyFile4;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("ef7e0655-7888-4960-b0e5-730846e03492")
IBackgroundCopyFile4:public IBackgroundCopyFile3
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetPeerDownloadStats(PUINT64 pFromOrigin, PUINT64 pFromPeers) = 0;
};
#else /* C style interface */

typedef struct IBackgroundCopyFile4Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IBackgroundCopyFile4 *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IBackgroundCopyFile4 *This);
    ULONG (STDMETHODCALLTYPE *Release)(IBackgroundCopyFile4 *This);
    HRESULT (STDMETHODCALLTYPE *GetRemoteName)(IBackgroundCopyFile4 *This, LPWSTR *pVal);
    HRESULT (STDMETHODCALLTYPE *GetLocalName)(IBackgroundCopyFile4 *This, LPWSTR *pVal);
    HRESULT (STDMETHODCALLTYPE *GetProgress)(IBackgroundCopyFile4 *This, BG_FILE_PROGRESS *pVal);
    HRESULT (STDMETHODCALLTYPE *GetFileRanges)(IBackgroundCopyFile4 *This, DWORD *RangeCount, BG_FILE_RANGE ** Ranges);
    HRESULT (STDMETHODCALLTYPE *SetRemoteName)(IBackgroundCopyFile4 *This, LPCWSTR Val);
    HRESULT (STDMETHODCALLTYPE *GetTemporaryName)(IBackgroundCopyFile4 *This, LPWSTR *pFilename);
    HRESULT (STDMETHODCALLTYPE *SetValidationState)(IBackgroundCopyFile4 *This, BOOL state);
    HRESULT (STDMETHODCALLTYPE *GetValidationState)(IBackgroundCopyFile4 *This, BOOL *pState);
    HRESULT (STDMETHODCALLTYPE *IsDownloadedFromPeer)(IBackgroundCopyFile4 *This, BOOL *pVal);
    HRESULT (STDMETHODCALLTYPE *GetPeerDownloadStats)(IBackgroundCopyFile4 *This, PUINT64 pFromOrigin, PUINT64 pFromPeers);
    END_INTERFACE
} IBackgroundCopyFile4Vtbl;

interface IBackgroundCopyFile4 {
    CONST_VTBL struct IBackgroundCopyFile4Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IBackgroundCopyFile4_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IBackgroundCopyFile4_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IBackgroundCopyFile4_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IBackgroundCopyFile4_GetRemoteName(This,pVal)  ((This)->lpVtbl->GetRemoteName(This,pVal)) 
#define IBackgroundCopyFile4_GetLocalName(This,pVal)  ((This)->lpVtbl->GetLocalName(This,pVal)) 
#define IBackgroundCopyFile4_GetProgress(This,pVal)  ((This)->lpVtbl->GetProgress(This,pVal)) 
#define IBackgroundCopyFile4_GetFileRanges(This,RangeCount,Ranges)  ((This)->lpVtbl->GetFileRanges(This,RangeCount,Ranges)) 
#define IBackgroundCopyFile4_SetRemoteName(This,Val)  ((This)->lpVtbl->SetRemoteName(This,Val)) 
#define IBackgroundCopyFile4_GetTemporaryName(This,pFilename)  ((This)->lpVtbl->GetTemporaryName(This,pFilename)) 
#define IBackgroundCopyFile4_SetValidationState(This,state)  ((This)->lpVtbl->SetValidationState(This,state)) 
#define IBackgroundCopyFile4_GetValidationState(This,pState)  ((This)->lpVtbl->GetValidationState(This,pState)) 
#define IBackgroundCopyFile4_IsDownloadedFromPeer(This,pVal)  ((This)->lpVtbl->IsDownloadedFromPeer(This,pVal)) 
#define IBackgroundCopyFile4_GetPeerDownloadStats(This,pFromOrigin,pFromPeers)  ((This)->lpVtbl->GetPeerDownloadStats(This,pFromOrigin,pFromPeers)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IBackgroundCopyFile4_INTERFACE_DEFINED__ */

#ifndef __BackgroundCopyManager4_0_LIBRARY_DEFINED__
#define __BackgroundCopyManager4_0_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_BackgroundCopyManager4_0;
EXTERN_C const CLSID CLSID_BackgroundCopyManager4_0;

#ifdef __cplusplus
class DECLSPEC_UUID("bb6df56b-cace-11dc-9992-0019b93a3a84")
BackgroundCopyManager4_0;
#endif
#endif /* __BackgroundCopyManager4_0_LIBRARY_DEFINED__ */

#include "bits5_0.h"

extern RPC_IF_HANDLE __MIDL_itf_bits4_0_0000_0003_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_bits4_0_0000_0003_v0_0_s_ifspec;

#ifdef __cplusplus
}
#endif

#endif /* _BITS4_0_H */
