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

#ifndef _DOCOBJ_H
#define _DOCOBJ_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Forward Declarations */
#ifndef __IOleDocument_FWD_DEFINED__
#define __IOleDocument_FWD_DEFINED__
typedef interface IOleDocument IOleDocument;
#endif /* __IOleDocument_FWD_DEFINED__ */

#ifndef __IOleDocumentSite_FWD_DEFINED__
#define __IOleDocumentSite_FWD_DEFINED__
typedef interface IOleDocumentSite IOleDocumentSite;
#endif /* __IOleDocumentSite_FWD_DEFINED__ */

#ifndef __IOleDocumentView_FWD_DEFINED__
#define __IOleDocumentView_FWD_DEFINED__
typedef interface IOleDocumentView IOleDocumentView;
#endif /* __IOleDocumentView_FWD_DEFINED__ */

#ifndef __IEnumOleDocumentViews_FWD_DEFINED__
#define __IEnumOleDocumentViews_FWD_DEFINED__
typedef interface IEnumOleDocumentViews IEnumOleDocumentViews;
#endif /* __IEnumOleDocumentViews_FWD_DEFINED__ */

#ifndef __IContinueCallback_FWD_DEFINED__
#define __IContinueCallback_FWD_DEFINED__
typedef interface IContinueCallback IContinueCallback;
#endif /* __IContinueCallback_FWD_DEFINED__ */

#ifndef __IPrint_FWD_DEFINED__
#define __IPrint_FWD_DEFINED__
typedef interface IPrint IPrint;
#endif /* __IPrint_FWD_DEFINED__ */

#ifndef __IOleCommandTarget_FWD_DEFINED__
#define __IOleCommandTarget_FWD_DEFINED__
typedef interface IOleCommandTarget IOleCommandTarget;
#endif /* __IOleCommandTarget_FWD_DEFINED__ */

#include "ocidl.h"

#ifdef __cplusplus
extern "C" {
#endif

void *__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void *);

#ifndef _LPOLEDOCUMENT_DEFINED
#define _LPOLEDOCUMENT_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_docobj_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_docobj_0000_v0_0_s_ifspec;

#ifndef __IOleDocument_INTERFACE_DEFINED__
#define __IOleDocument_INTERFACE_DEFINED__

/* interface IOleDocument */
typedef IOleDocument *LPOLEDOCUMENT;

typedef enum __MIDL_IOleDocument_0001 {
    DOCMISC_CANCREATEMULTIPLEVIEWS = 1,
    DOCMISC_SUPPORTCOMPLEXRECTANGLES = 2,
    DOCMISC_CANTOPENEDIT = 4,
    DOCMISC_NOFILESUPPORT = 8
} DOCMISC;

EXTERN_C const IID IID_IOleDocument;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("b722bcc5-4e68-101b-a2bc-00aa00404770") IOleDocument:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateView(IOleInPlaceSite*,IStream*,DWORD,IOleDocumentView**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDocMiscStatus(DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumViews(IEnumOleDocumentViews**,IOleDocumentView**) = 0;
};
#else /* C style interface */
typedef struct IOleDocumentVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IOleDocument*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IOleDocument*);
    ULONG(STDMETHODCALLTYPE *Release)(IOleDocument*);
    HRESULT(STDMETHODCALLTYPE *CreateView)(IOleDocument*,IOleInPlaceSite*,IStream*,DWORD,IOleDocumentView**);
    HRESULT(STDMETHODCALLTYPE *GetDocMiscStatus)(IOleDocument*,DWORD*);
    HRESULT(STDMETHODCALLTYPE *EnumViews)(IOleDocument*,IEnumOleDocumentViews**,IOleDocumentView**);
    END_INTERFACE
} IOleDocumentVtbl;

interface IOleDocument {
    CONST_VTBL struct IOleDocumentVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleDocument_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IOleDocument_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleDocument_Release(This)  (This)->lpVtbl->Release(This)
#define IOleDocument_CreateView(This,pIPSite,pstm,dwReserved,ppView)  (This)->lpVtbl->CreateView(This,pIPSite,pstm,dwReserved,ppView)
#define IOleDocument_GetDocMiscStatus(This,pdwStatus)  (This)->lpVtbl->GetDocMiscStatus(This,pdwStatus)
#define IOleDocument_EnumViews(This,ppEnum,ppView)  (This)->lpVtbl->EnumViews(This,ppEnum,ppView)
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IOleDocument_CreateView_Proxy(IOleDocument*,IOleInPlaceSite*,IStream*,DWORD,IOleDocumentView**);
void __RPC_STUB IOleDocument_CreateView_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleDocument_GetDocMiscStatus_Proxy(IOleDocument*,DWORD*);
void __RPC_STUB IOleDocument_GetDocMiscStatus_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleDocument_EnumViews_Proxy(IOleDocument*,IEnumOleDocumentViews**,IOleDocumentView**);
void __RPC_STUB IOleDocument_EnumViews_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IOleDocument_INTERFACE_DEFINED__ */

#endif

#ifndef _LPOLEDOCUMENTSITE_DEFINED
#define _LPOLEDOCUMENTSITE_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_docobj_0255_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_docobj_0255_v0_0_s_ifspec;

#ifndef __IOleDocumentSite_INTERFACE_DEFINED__
#define __IOleDocumentSite_INTERFACE_DEFINED__

/* interface IOleDocumentSite */
typedef IOleDocumentSite *LPOLEDOCUMENTSITE;

EXTERN_C const IID IID_IOleDocumentSite;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("b722bcc7-4e68-101b-a2bc-00aa00404770") IOleDocumentSite:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE ActivateMe(IOleDocumentView*) = 0;
};
#else /* C style interface */
typedef struct IOleDocumentSiteVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface)(IOleDocumentSite*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE * AddRef) (IOleDocumentSite*);
    ULONG(STDMETHODCALLTYPE * Release) (IOleDocumentSite*);
    HRESULT(STDMETHODCALLTYPE * ActivateMe) (IOleDocumentSite*,IOleDocumentView*);
    END_INTERFACE
} IOleDocumentSiteVtbl;

interface IOleDocumentSite {
    CONST_VTBL struct IOleDocumentSiteVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleDocumentSite_QueryInterface(This, riid, ppvObject)  (This)->lpVtbl->QueryInterface(This, riid, ppvObject)
#define IOleDocumentSite_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleDocumentSite_Release(This)  (This)->lpVtbl->Release(This)
#define IOleDocumentSite_ActivateMe(This, pViewToActivate)  (This)->lpVtbl->ActivateMe(This, pViewToActivate)
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IOleDocumentSite_ActivateMe_Proxy(IOleDocumentSite *, IOleDocumentView *);
void __RPC_STUB IOleDocumentSite_ActivateMe_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);

#endif /* __IOleDocumentSite_INTERFACE_DEFINED__ */

#endif

#ifndef _LPOLEDOCUMENTVIEW_DEFINED
#define _LPOLEDOCUMENTVIEW_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_docobj_0256_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_docobj_0256_v0_0_s_ifspec;

#ifndef __IOleDocumentView_INTERFACE_DEFINED__
#define __IOleDocumentView_INTERFACE_DEFINED__

/* interface IOleDocumentView */
typedef IOleDocumentView *LPOLEDOCUMENTVIEW;

EXTERN_C const IID IID_IOleDocumentView;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("b722bcc6-4e68-101b-a2bc-00aa00404770") IOleDocumentView:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE SetInPlaceSite(IOleInPlaceSite * pIPSite) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetInPlaceSite(IOleInPlaceSite ** ppIPSite) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDocument(IUnknown ** ppunk) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetRect(LPRECT prcView) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRect(LPRECT prcView) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetRectComplex(LPRECT prcView, LPRECT prcHScroll, LPRECT prcVScroll, LPRECT prcSizeBox) = 0;
    virtual HRESULT STDMETHODCALLTYPE Show(BOOL fShow) = 0;
    virtual HRESULT STDMETHODCALLTYPE UIActivate(BOOL fUIActivate) = 0;
    virtual HRESULT STDMETHODCALLTYPE Open(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE CloseView(DWORD dwReserved) = 0;
    virtual HRESULT STDMETHODCALLTYPE SaveViewState(LPSTREAM pstm) = 0;
    virtual HRESULT STDMETHODCALLTYPE ApplyViewState(LPSTREAM pstm) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IOleInPlaceSite * pIPSiteNew, IOleDocumentView ** ppViewNew) = 0;
};
#else /* C style interface */
typedef struct IOleDocumentViewVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IOleDocumentView * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IOleDocumentView * This);
    ULONG(STDMETHODCALLTYPE * Release) (IOleDocumentView * This);
    HRESULT(STDMETHODCALLTYPE * SetInPlaceSite) (IOleDocumentView * This, IOleInPlaceSite * pIPSite);
    HRESULT(STDMETHODCALLTYPE * GetInPlaceSite) (IOleDocumentView * This, IOleInPlaceSite ** ppIPSite);
    HRESULT(STDMETHODCALLTYPE * GetDocument) (IOleDocumentView * This, IUnknown ** ppunk);
    HRESULT(STDMETHODCALLTYPE * SetRect) (IOleDocumentView * This, LPRECT prcView);
    HRESULT(STDMETHODCALLTYPE * GetRect) (IOleDocumentView * This, LPRECT prcView);
    HRESULT(STDMETHODCALLTYPE * SetRectComplex) (IOleDocumentView * This, LPRECT prcView, LPRECT prcHScroll, LPRECT prcVScroll, LPRECT prcSizeBox);
    HRESULT(STDMETHODCALLTYPE * Show) (IOleDocumentView * This, BOOL fShow);
    HRESULT(STDMETHODCALLTYPE * UIActivate) (IOleDocumentView * This, BOOL fUIActivate);
    HRESULT(STDMETHODCALLTYPE * Open) (IOleDocumentView * This);
    HRESULT(STDMETHODCALLTYPE * CloseView) (IOleDocumentView * This, DWORD dwReserved);
    HRESULT(STDMETHODCALLTYPE * SaveViewState) (IOleDocumentView * This, LPSTREAM pstm);
    HRESULT(STDMETHODCALLTYPE * ApplyViewState) (IOleDocumentView * This, LPSTREAM pstm);
    HRESULT(STDMETHODCALLTYPE * Clone) (IOleDocumentView * This, IOleInPlaceSite * pIPSiteNew, IOleDocumentView ** ppViewNew);
    END_INTERFACE
} IOleDocumentViewVtbl;

interface IOleDocumentView {
    CONST_VTBL struct IOleDocumentViewVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleDocumentView_QueryInterface(This, riid, ppvObject)  (This)->lpVtbl->QueryInterface(This, riid, ppvObject)
#define IOleDocumentView_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleDocumentView_Release(This)  (This)->lpVtbl->Release(This)
#define IOleDocumentView_SetInPlaceSite(This, pIPSite)  (This)->lpVtbl->SetInPlaceSite(This, pIPSite)
#define IOleDocumentView_GetInPlaceSite(This, ppIPSite)  (This)->lpVtbl->GetInPlaceSite(This, ppIPSite)
#define IOleDocumentView_GetDocument(This, ppunk)  (This)->lpVtbl->GetDocument(This, ppunk)
#define IOleDocumentView_SetRect(This, prcView)  (This)->lpVtbl->SetRect(This, prcView)
#define IOleDocumentView_GetRect(This, prcView)  (This)->lpVtbl->GetRect(This, prcView)
#define IOleDocumentView_SetRectComplex(This, prcView, prcHScroll, prcVScroll, prcSizeBox)  (This)->lpVtbl->SetRectComplex(This, prcView, prcHScroll, prcVScroll, prcSizeBox)
#define IOleDocumentView_Show(This, fShow)  (This)->lpVtbl->Show(This, fShow)
#define IOleDocumentView_UIActivate(This, fUIActivate)  (This)->lpVtbl->UIActivate(This, fUIActivate)
#define IOleDocumentView_Open(This)  (This)->lpVtbl->Open(This)
#define IOleDocumentView_CloseView(This, dwReserved)  (This)->lpVtbl->CloseView(This, dwReserved)
#define IOleDocumentView_SaveViewState(This, pstm)  (This)->lpVtbl->SaveViewState(This, pstm)
#define IOleDocumentView_ApplyViewState(This, pstm)  (This)->lpVtbl->ApplyViewState(This, pstm)
#define IOleDocumentView_Clone(This, pIPSiteNew, ppViewNew)  (This)->lpVtbl->Clone(This, pIPSiteNew, ppViewNew)
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IOleDocumentView_SetInPlaceSite_Proxy(IOleDocumentView *, /* [unique][in] */ IOleInPlaceSite *);
void __RPC_STUB IOleDocumentView_SetInPlaceSite_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IOleDocumentView_GetInPlaceSite_Proxy(IOleDocumentView *, IOleInPlaceSite **);
void __RPC_STUB IOleDocumentView_GetInPlaceSite_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IOleDocumentView_GetDocument_Proxy(IOleDocumentView *, IUnknown **);
void __RPC_STUB IOleDocumentView_GetDocument_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IOleDocumentView_SetRect_Proxy(IOleDocumentView *, LPRECT prcView);
void __RPC_STUB IOleDocumentView_SetRect_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IOleDocumentView_GetRect_Proxy(IOleDocumentView *, LPRECT prcView);
void __RPC_STUB IOleDocumentView_GetRect_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IOleDocumentView_SetRectComplex_Proxy(IOleDocumentView *, LPRECT prcView, LPRECT prcHScroll, LPRECT prcVScroll, LPRECT prcSizeBox);
void __RPC_STUB IOleDocumentView_SetRectComplex_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IOleDocumentView_Show_Proxy(IOleDocumentView *, BOOL fShow);
void __RPC_STUB IOleDocumentView_Show_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IOleDocumentView_UIActivate_Proxy(IOleDocumentView *, BOOL fUIActivate);
void __RPC_STUB IOleDocumentView_UIActivate_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IOleDocumentView_Open_Proxy(IOleDocumentView *);
void __RPC_STUB IOleDocumentView_Open_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IOleDocumentView_CloseView_Proxy(IOleDocumentView *, DWORD dwReserved);
void __RPC_STUB IOleDocumentView_CloseView_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IOleDocumentView_SaveViewState_Proxy(IOleDocumentView *, LPSTREAM pstm);
void __RPC_STUB IOleDocumentView_SaveViewState_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IOleDocumentView_ApplyViewState_Proxy(IOleDocumentView *, LPSTREAM pstm);
void __RPC_STUB IOleDocumentView_ApplyViewState_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IOleDocumentView_Clone_Proxy(IOleDocumentView *, IOleInPlaceSite *, IOleDocumentView **);
void __RPC_STUB IOleDocumentView_Clone_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);

#endif /* __IOleDocumentView_INTERFACE_DEFINED__ */

/* interface __MIDL_itf_docobj_0257 */
#endif

#ifndef _LPENUMOLEDOCUMENTVIEWS_DEFINED
#define _LPENUMOLEDOCUMENTVIEWS_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_docobj_0257_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_docobj_0257_v0_0_s_ifspec;

#ifndef __IEnumOleDocumentViews_INTERFACE_DEFINED__
#define __IEnumOleDocumentViews_INTERFACE_DEFINED__

/* interface IEnumOleDocumentViews */

typedef IEnumOleDocumentViews *LPENUMOLEDOCUMENTVIEWS;

EXTERN_C const IID IID_IEnumOleDocumentViews;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("b722bcc8-4e68-101b-a2bc-00aa00404770") IEnumOleDocumentViews:public IUnknown {
    public:
    virtual HRESULT __stdcall Next(ULONG cViews, IOleDocumentView ** rgpView, ULONG * pcFetched) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(ULONG cViews) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumOleDocumentViews ** ppEnum) = 0;
};
#else /* C style interface */
typedef struct IEnumOleDocumentViewsVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IEnumOleDocumentViews * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IEnumOleDocumentViews * This);
    ULONG(STDMETHODCALLTYPE * Release) (IEnumOleDocumentViews * This);
    HRESULT(__stdcall * Next) (IEnumOleDocumentViews * This, ULONG cViews, IOleDocumentView ** rgpView, ULONG * pcFetched);
    HRESULT(STDMETHODCALLTYPE * Skip) (IEnumOleDocumentViews * This, ULONG cViews);
    HRESULT(STDMETHODCALLTYPE * Reset) (IEnumOleDocumentViews * This);
    HRESULT(STDMETHODCALLTYPE * Clone) (IEnumOleDocumentViews * This, IEnumOleDocumentViews ** ppEnum);
    END_INTERFACE
} IEnumOleDocumentViewsVtbl;

interface IEnumOleDocumentViews {
    CONST_VTBL struct IEnumOleDocumentViewsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEnumOleDocumentViews_QueryInterface(This, riid, ppvObject)  (This)->lpVtbl->QueryInterface(This, riid, ppvObject)
#define IEnumOleDocumentViews_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IEnumOleDocumentViews_Release(This)  (This)->lpVtbl->Release(This)
#define IEnumOleDocumentViews_Next(This, cViews, rgpView, pcFetched)  (This)->lpVtbl->Next(This, cViews, rgpView, pcFetched)
#define IEnumOleDocumentViews_Skip(This, cViews)  (This)->lpVtbl->Skip(This, cViews)
#define IEnumOleDocumentViews_Reset(This)  (This)->lpVtbl->Reset(This)
#define IEnumOleDocumentViews_Clone(This, ppEnum)  (This)->lpVtbl->Clone(This, ppEnum)
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT __stdcall IEnumOleDocumentViews_RemoteNext_Proxy(IEnumOleDocumentViews *, ULONG cViews, IOleDocumentView **, ULONG *);
void __RPC_STUB IEnumOleDocumentViews_RemoteNext_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IEnumOleDocumentViews_Skip_Proxy(IEnumOleDocumentViews *, ULONG cViews);
void __RPC_STUB IEnumOleDocumentViews_Skip_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IEnumOleDocumentViews_Reset_Proxy(IEnumOleDocumentViews *);
void __RPC_STUB IEnumOleDocumentViews_Reset_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IEnumOleDocumentViews_Clone_Proxy(IEnumOleDocumentViews *, IEnumOleDocumentViews **);
void __RPC_STUB IEnumOleDocumentViews_Clone_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);

#endif /* __IEnumOleDocumentViews_INTERFACE_DEFINED__ */

/* interface __MIDL_itf_docobj_0258 */
#endif

#ifndef _LPCONTINUECALLBACK_DEFINED
#define _LPCONTINUECALLBACK_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_docobj_0258_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_docobj_0258_v0_0_s_ifspec;

#ifndef __IContinueCallback_INTERFACE_DEFINED__
#define __IContinueCallback_INTERFACE_DEFINED__

/* interface IContinueCallback */

typedef IContinueCallback *LPCONTINUECALLBACK;

EXTERN_C const IID IID_IContinueCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("b722bcca-4e68-101b-a2bc-00aa00404770") IContinueCallback:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE FContinue(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE FContinuePrinting(LONG nCntPrinted, LONG nCurPage, wchar_t * pwszPrintStatus) = 0;
};
#else /* C style interface */
typedef struct IContinueCallbackVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IContinueCallback * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IContinueCallback * This);
    ULONG(STDMETHODCALLTYPE * Release) (IContinueCallback * This);
    HRESULT(STDMETHODCALLTYPE * FContinue) (IContinueCallback * This);
    HRESULT(STDMETHODCALLTYPE * FContinuePrinting) (IContinueCallback * This, LONG nCntPrinted, LONG nCurPage, wchar_t * pwszPrintStatus);
    END_INTERFACE
} IContinueCallbackVtbl;

interface IContinueCallback {
    CONST_VTBL struct IContinueCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IContinueCallback_QueryInterface(This, riid, ppvObject)  (This)->lpVtbl->QueryInterface(This, riid, ppvObject)
#define IContinueCallback_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IContinueCallback_Release(This)  (This)->lpVtbl->Release(This)
#define IContinueCallback_FContinue(This)  (This)->lpVtbl->FContinue(This)
#define IContinueCallback_FContinuePrinting(This, nCntPrinted, nCurPage, pwszPrintStatus)  (This)->lpVtbl->FContinuePrinting(This, nCntPrinted, nCurPage, pwszPrintStatus)
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IContinueCallback_FContinue_Proxy(IContinueCallback *);
void __RPC_STUB IContinueCallback_FContinue_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IContinueCallback_FContinuePrinting_Proxy(IContinueCallback *, LONG nCntPrinted, LONG nCurPage, wchar_t *);
void __RPC_STUB IContinueCallback_FContinuePrinting_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);

#endif /* __IContinueCallback_INTERFACE_DEFINED__ */

#endif

#ifndef _LPPRINT_DEFINED
#define _LPPRINT_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_docobj_0259_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_docobj_0259_v0_0_s_ifspec;

#ifndef __IPrint_INTERFACE_DEFINED__
#define __IPrint_INTERFACE_DEFINED__

typedef IPrint *LPPRINT;

typedef enum __MIDL_IPrint_0001 {
    PRINTFLAG_MAYBOTHERUSER = 1,
    PRINTFLAG_PROMPTUSER = 2,
    PRINTFLAG_USERMAYCHANGEPRINTER = 4,
    PRINTFLAG_RECOMPOSETODEVICE = 8,
    PRINTFLAG_DONTACTUALLYPRINT = 16,
    PRINTFLAG_FORCEPROPERTIES = 32,
    PRINTFLAG_PRINTTOFILE = 64
} PRINTFLAG;

typedef struct tagPAGERANGE {
    LONG nFromPage;
    LONG nToPage;
} PAGERANGE;

typedef struct tagPAGESET {
    ULONG cbStruct;
    BOOL fOddPages;
    BOOL fEvenPages;
    ULONG cPageRange;
    PAGERANGE rgPages[1];
} PAGESET;

#define PAGESET_TOLASTPAGE  ((WORD)(-1L))

EXTERN_C const IID IID_IPrint;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("b722bcc9-4e68-101b-a2bc-00aa00404770") IPrint:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE SetInitialPageNum(LONG nFirstPage) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPageInfo(LONG * pnFirstPage, LONG * pcPages) = 0;
    virtual HRESULT __stdcall Print(DWORD grfFlags, DVTARGETDEVICE ** pptd, PAGESET ** ppPageSet, STGMEDIUM * pstgmOptions, IContinueCallback * pcallback, LONG nFirstPage, LONG * pcPagesPrinted, LONG * pnLastPage) = 0;
};
#else /* C style interface */
typedef struct IPrintVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IPrint * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IPrint * This);
    ULONG(STDMETHODCALLTYPE * Release) (IPrint * This);
    HRESULT(STDMETHODCALLTYPE * SetInitialPageNum) (IPrint * This, LONG nFirstPage);
    HRESULT(STDMETHODCALLTYPE * GetPageInfo) (IPrint * This, LONG * pnFirstPage, LONG * pcPages);
    HRESULT(__stdcall * Print) (IPrint * This, DWORD grfFlags, DVTARGETDEVICE ** pptd, PAGESET ** ppPageSet, STGMEDIUM * pstgmOptions, IContinueCallback * pcallback, LONG nFirstPage, LONG * pcPagesPrinted, LONG * pnLastPage);
    END_INTERFACE
} IPrintVtbl;

interface IPrint {
    CONST_VTBL struct IPrintVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPrint_QueryInterface(This, riid, ppvObject)  (This)->lpVtbl->QueryInterface(This, riid, ppvObject)
#define IPrint_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IPrint_Release(This)  (This)->lpVtbl->Release(This)
#define IPrint_SetInitialPageNum(This, nFirstPage)  (This)->lpVtbl->SetInitialPageNum(This, nFirstPage)
#define IPrint_GetPageInfo(This, pnFirstPage, pcPages)  (This)->lpVtbl->GetPageInfo(This, pnFirstPage, pcPages)
#define IPrint_Print(This, grfFlags, pptd, ppPageSet, pstgmOptions, pcallback, nFirstPage, pcPagesPrinted, pnLastPage)  (This)->lpVtbl->Print(This, grfFlags, pptd, ppPageSet, pstgmOptions, pcallback, nFirstPage, pcPagesPrinted, pnLastPage)
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IPrint_SetInitialPageNum_Proxy(IPrint *, LONG nFirstPage);
void __RPC_STUB IPrint_SetInitialPageNum_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IPrint_GetPageInfo_Proxy(IPrint *, LONG *, LONG *);
void __RPC_STUB IPrint_GetPageInfo_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT __stdcall IPrint_RemotePrint_Proxy(IPrint *, DWORD grfFlags, DVTARGETDEVICE **, PAGESET **, RemSTGMEDIUM *, IContinueCallback *, LONG nFirstPage, LONG *, LONG *);
void __RPC_STUB IPrint_RemotePrint_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);

#endif /* __IPrint_INTERFACE_DEFINED__ */

/* interface __MIDL_itf_docobj_0260 */
#endif

#ifndef _LPOLECOMMANDTARGET_DEFINED
#define _LPOLECOMMANDTARGET_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_docobj_0260_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_docobj_0260_v0_0_s_ifspec;

#ifndef __IOleCommandTarget_INTERFACE_DEFINED__
#define __IOleCommandTarget_INTERFACE_DEFINED__

/* interface IOleCommandTarget */
typedef IOleCommandTarget *LPOLECOMMANDTARGET;

typedef enum OLECMDF {
    OLECMDF_SUPPORTED = 0x1,
    OLECMDF_ENABLED = 0x2,
    OLECMDF_LATCHED = 0x4,
    OLECMDF_NINCHED = 0x8,
    OLECMDF_INVISIBLE = 0x10,
    OLECMDF_DEFHIDEONCTXTMENU = 0x20
} OLECMDF;

typedef struct _tagOLECMD {
    ULONG cmdID;
    DWORD cmdf;
} OLECMD;

typedef struct _tagOLECMDTEXT {
    DWORD cmdtextf;
    ULONG cwActual;
    ULONG cwBuf;
    wchar_t rgwz[1];
} OLECMDTEXT;

typedef enum OLECMDTEXTF {
    OLECMDTEXTF_NONE = 0,
    OLECMDTEXTF_NAME = 1,
    OLECMDTEXTF_STATUS = 2
} OLECMDTEXTF;

typedef enum OLECMDEXECOPT {
    OLECMDEXECOPT_DODEFAULT = 0,
    OLECMDEXECOPT_PROMPTUSER = 1,
    OLECMDEXECOPT_DONTPROMPTUSER = 2,
    OLECMDEXECOPT_SHOWHELP = 3
} OLECMDEXECOPT;

typedef enum OLECMDID {
    OLECMDID_OPEN = 1,
    OLECMDID_NEW = 2,
    OLECMDID_SAVE = 3,
    OLECMDID_SAVEAS = 4,
    OLECMDID_SAVECOPYAS = 5,
    OLECMDID_PRINT = 6,
    OLECMDID_PRINTPREVIEW = 7,
    OLECMDID_PAGESETUP = 8,
    OLECMDID_SPELL = 9,
    OLECMDID_PROPERTIES = 10,
    OLECMDID_CUT = 11,
    OLECMDID_COPY = 12,
    OLECMDID_PASTE = 13,
    OLECMDID_PASTESPECIAL = 14,
    OLECMDID_UNDO = 15,
    OLECMDID_REDO = 16,
    OLECMDID_SELECTALL = 17,
    OLECMDID_CLEARSELECTION = 18,
    OLECMDID_ZOOM = 19,
    OLECMDID_GETZOOMRANGE = 20,
    OLECMDID_UPDATECOMMANDS = 21,
    OLECMDID_REFRESH = 22,
    OLECMDID_STOP = 23,
    OLECMDID_HIDETOOLBARS = 24,
    OLECMDID_SETPROGRESSMAX = 25,
    OLECMDID_SETPROGRESSPOS = 26,
    OLECMDID_SETPROGRESSTEXT = 27,
    OLECMDID_SETTITLE = 28,
    OLECMDID_SETDOWNLOADSTATE = 29,
    OLECMDID_STOPDOWNLOAD = 30,
    OLECMDID_ONTOOLBARACTIVATED = 31,
    OLECMDID_FIND = 32,
    OLECMDID_DELETE = 33,
    OLECMDID_HTTPEQUIV = 34,
    OLECMDID_HTTPEQUIV_DONE = 35,
    OLECMDID_ENABLE_INTERACTION = 36,
    OLECMDID_ONUNLOAD = 37,
    OLECMDID_PROPERTYBAG2 = 38,
    OLECMDID_PREREFRESH = 39,
    OLECMDID_SHOWSCRIPTERROR = 40,
    OLECMDID_SHOWMESSAGE = 41,
    OLECMDID_SHOWFIND = 42,
    OLECMDID_SHOWPAGESETUP = 43,
    OLECMDID_SHOWPRINT = 44,
    OLECMDID_CLOSE = 45,
    OLECMDID_ALLOWUILESSSAVEAS = 46,
    OLECMDID_DONTDOWNLOADCSS = 47,
    OLECMDID_UPDATEPAGESTATUS = 48,
    OLECMDID_PRINT2 = 49,
    OLECMDID_PRINTPREVIEW2 = 50,
    OLECMDID_SETPRINTTEMPLATE = 51,
    OLECMDID_GETPRINTTEMPLATE = 52
} OLECMDID;

#define OLECMDERR_E_FIRST  (OLE_E_LAST + 1)
#define OLECMDERR_E_NOTSUPPORTED  (OLECMDERR_E_FIRST)
#define OLECMDERR_E_DISABLED  (OLECMDERR_E_FIRST + 1)
#define OLECMDERR_E_NOHELP  (OLECMDERR_E_FIRST + 2)
#define OLECMDERR_E_CANCELED  (OLECMDERR_E_FIRST + 3)
#define OLECMDERR_E_UNKNOWNGROUP  (OLECMDERR_E_FIRST + 4)
#define MSOCMDERR_E_FIRST  OLECMDERR_E_FIRST
#define MSOCMDERR_E_NOTSUPPORTED  OLECMDERR_E_NOTSUPPORTED
#define MSOCMDERR_E_DISABLED  OLECMDERR_E_DISABLED
#define MSOCMDERR_E_NOHELP  OLECMDERR_E_NOHELP
#define MSOCMDERR_E_CANCELED  OLECMDERR_E_CANCELED
#define MSOCMDERR_E_UNKNOWNGROUP  OLECMDERR_E_UNKNOWNGROUP

EXTERN_C const IID IID_IOleCommandTarget;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("b722bccb-4e68-101b-a2bc-00aa00404770") IOleCommandTarget:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE QueryStatus(const GUID * pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT * pCmdText) = 0;
    virtual HRESULT STDMETHODCALLTYPE Exec(const GUID * pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT * pvaIn, VARIANT * pvaOut) = 0;
};
#else /* C style interface */
typedef struct IOleCommandTargetVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IOleCommandTarget * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IOleCommandTarget * This);
    ULONG(STDMETHODCALLTYPE * Release) (IOleCommandTarget * This);
    HRESULT(STDMETHODCALLTYPE * QueryStatus) (IOleCommandTarget * This, const GUID * pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT * pCmdText);
    HRESULT(STDMETHODCALLTYPE * Exec) (IOleCommandTarget * This, const GUID * pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT * pvaIn, VARIANT * pvaOut);
    END_INTERFACE
} IOleCommandTargetVtbl;

interface IOleCommandTarget {
    CONST_VTBL struct IOleCommandTargetVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleCommandTarget_QueryInterface(This, riid, ppvObject)  (This)->lpVtbl->QueryInterface(This, riid, ppvObject)
#define IOleCommandTarget_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleCommandTarget_Release(This)  (This)->lpVtbl->Release(This)
#define IOleCommandTarget_QueryStatus(This, pguidCmdGroup, cCmds, prgCmds, pCmdText)  (This)->lpVtbl->QueryStatus(This, pguidCmdGroup, cCmds, prgCmds, pCmdText)
#define IOleCommandTarget_Exec(This, pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut)  (This)->lpVtbl->Exec(This, pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut)
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IOleCommandTarget_QueryStatus_Proxy(IOleCommandTarget *, const GUID *, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *);
void __RPC_STUB IOleCommandTarget_QueryStatus_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);
HRESULT STDMETHODCALLTYPE IOleCommandTarget_Exec_Proxy(IOleCommandTarget *, const GUID *, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *, VARIANT *);
void __RPC_STUB IOleCommandTarget_Exec_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE _pRpcMessage, DWORD *);

#endif /* __IOleCommandTarget_INTERFACE_DEFINED__ */

#endif

typedef enum {
    OLECMDIDF_REFRESH_NORMAL = 0,
    OLECMDIDF_REFRESH_IFEXPIRED = 1,
    OLECMDIDF_REFRESH_CONTINUE = 2,
    OLECMDIDF_REFRESH_COMPLETELY = 3,
    OLECMDIDF_REFRESH_NO_CACHE = 4,
    OLECMDIDF_REFRESH_RELOAD = 5,
    OLECMDIDF_REFRESH_LEVELMASK = 0x00FF,
    OLECMDIDF_REFRESH_CLEARUSERINPUT = 0x1000,
    OLECMDIDF_REFRESH_PROMPTIFOFFLINE = 0x2000,
    OLECMDIDF_REFRESH_THROUGHSCRIPT = 0x4000
} OLECMDID_REFRESHFLAG;

#define IMsoDocument  IOleDocument
#define IMsoDocumentSite  IOleDocumentSite
#define IMsoView  IOleDocumentView
#define IEnumMsoView  IEnumOleDocumentViews
#define IMsoCommandTarget  IOleCommandTarget
#define LPMSODOCUMENT  LPOLEDOCUMENT
#define LPMSODOCUMENTSITE  LPOLEDOCUMENTSITE
#define LPMSOVIEW  LPOLEDOCUMENTVIEW
#define LPENUMMSOVIEW  LPENUMOLEDOCUMENTVIEWS
#define LPMSOCOMMANDTARGET  LPOLECOMMANDTARGET
#define MSOCMD  OLECMD
#define MSOCMDTEXT  OLECMDTEXT
#define IID_IMsoDocument  IID_IOleDocument
#define IID_IMsoDocumentSite  IID_IOleDocumentSite
#define IID_IMsoView  IID_IOleDocumentView
#define IID_IEnumMsoView  IID_IEnumOleDocumentViews
#define IID_IMsoCommandTarget  IID_IOleCommandTarget
#define MSOCMDF_SUPPORTED  OLECMDF_SUPPORTED
#define MSOCMDF_ENABLED  OLECMDF_ENABLED
#define MSOCMDF_LATCHED  OLECMDF_LATCHED
#define MSOCMDF_NINCHED  OLECMDF_NINCHED
#define MSOCMDTEXTF_NONE  OLECMDTEXTF_NONE
#define MSOCMDTEXTF_NAME  OLECMDTEXTF_NAME
#define MSOCMDTEXTF_STATUS  OLECMDTEXTF_STATUS
#define MSOCMDEXECOPT_DODEFAULT  OLECMDEXECOPT_DODEFAULT
#define MSOCMDEXECOPT_PROMPTUSER  OLECMDEXECOPT_PROMPTUSER
#define MSOCMDEXECOPT_DONTPROMPTUSER  OLECMDEXECOPT_DONTPROMPTUSER
#define MSOCMDEXECOPT_SHOWHELP  OLECMDEXECOPT_SHOWHELP
#define MSOCMDID_OPEN  OLECMDID_OPEN
#define MSOCMDID_NEW  OLECMDID_NEW
#define MSOCMDID_SAVE  OLECMDID_SAVE
#define MSOCMDID_SAVEAS  OLECMDID_SAVEAS
#define MSOCMDID_SAVECOPYAS  OLECMDID_SAVECOPYAS
#define MSOCMDID_PRINT  OLECMDID_PRINT
#define MSOCMDID_PRINTPREVIEW  OLECMDID_PRINTPREVIEW
#define MSOCMDID_PAGESETUP  OLECMDID_PAGESETUP
#define MSOCMDID_SPELL  OLECMDID_SPELL
#define MSOCMDID_PROPERTIES  OLECMDID_PROPERTIES
#define MSOCMDID_CUT  OLECMDID_CUT
#define MSOCMDID_COPY  OLECMDID_COPY
#define MSOCMDID_PASTE  OLECMDID_PASTE
#define MSOCMDID_PASTESPECIAL  OLECMDID_PASTESPECIAL
#define MSOCMDID_UNDO  OLECMDID_UNDO
#define MSOCMDID_REDO  OLECMDID_REDO
#define MSOCMDID_SELECTALL  OLECMDID_SELECTALL
#define MSOCMDID_CLEARSELECTION  OLECMDID_CLEARSELECTION
#define MSOCMDID_ZOOM  OLECMDID_ZOOM
#define MSOCMDID_GETZOOMRANGE  OLECMDID_GETZOOMRANGE

EXTERN_C const GUID SID_SContainerDispatch;

extern RPC_IF_HANDLE __MIDL_itf_docobj_0261_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_docobj_0261_v0_0_s_ifspec;

unsigned long __RPC_USER VARIANT_UserSize(unsigned long *, unsigned long, VARIANT *);
unsigned char *__RPC_USER VARIANT_UserMarshal(unsigned long *, unsigned char *, VARIANT *);
unsigned char *__RPC_USER VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT *);
void __RPC_USER VARIANT_UserFree(unsigned long *, VARIANT *);

HRESULT __stdcall IEnumOleDocumentViews_Next_Proxy(IEnumOleDocumentViews *, ULONG cViews, IOleDocumentView **, ULONG *);
HRESULT __stdcall IEnumOleDocumentViews_Next_Stub(IEnumOleDocumentViews *, ULONG cViews, IOleDocumentView **, ULONG *);
HRESULT __stdcall IPrint_Print_Proxy(IPrint *, DWORD grfFlags, DVTARGETDEVICE **, PAGESET **, STGMEDIUM *, IContinueCallback *, LONG nFirstPage, LONG *, LONG *);
HRESULT __stdcall IPrint_Print_Stub(IPrint *, DWORD grfFlags, DVTARGETDEVICE **, PAGESET **, RemSTGMEDIUM *, IContinueCallback *, LONG nFirstPage, LONG *, LONG *);

#ifdef __cplusplus
}
#endif
#endif
