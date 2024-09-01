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

#ifndef _MSHTMHST_H
#define _MSHTMHST_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IHostDialogHelper_FWD_DEFINED__
#define __IHostDialogHelper_FWD_DEFINED__
typedef interface IHostDialogHelper IHostDialogHelper;
#endif /* __IHostDialogHelper_FWD_DEFINED__ */

#ifndef __HostDialogHelper_FWD_DEFINED__
#define __HostDialogHelper_FWD_DEFINED__

#ifdef __cplusplus
typedef class HostDialogHelper HostDialogHelper;
#else
typedef struct HostDialogHelper HostDialogHelper;
#endif /* __cplusplus */

#endif /* __HostDialogHelper_FWD_DEFINED__ */

#ifndef __IDocHostUIHandler_FWD_DEFINED__
#define __IDocHostUIHandler_FWD_DEFINED__
typedef interface IDocHostUIHandler IDocHostUIHandler;
#endif

#ifndef __IDocHostUIHandler2_FWD_DEFINED__
#define __IDocHostUIHandler2_FWD_DEFINED__
typedef interface IDocHostUIHandler2 IDocHostUIHandler2;
#endif

#ifndef __ICustomDoc_FWD_DEFINED__
#define __ICustomDoc_FWD_DEFINED__
typedef interface ICustomDoc ICustomDoc;
#endif

#ifndef __IDocHostShowUI_FWD_DEFINED__
#define __IDocHostShowUI_FWD_DEFINED__
typedef interface IDocHostShowUI IDocHostShowUI;
#endif

#ifndef __IClassFactoryEx_FWD_DEFINED__
#define __IClassFactoryEx_FWD_DEFINED__
typedef interface IClassFactoryEx IClassFactoryEx;
#endif

#include "ocidl.h"
#include "docobj.h"

#ifdef __cplusplus
extern "C" {
#endif

void *__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void *);

#pragma comment(lib,"uuid.lib")

#ifndef MSHTMHST_H
#define MSHTMHST_H
#define CONTEXT_MENU_DEFAULT  0
#define CONTEXT_MENU_IMAGE  1
#define CONTEXT_MENU_CONTROL  2
#define CONTEXT_MENU_TABLE  3
#define CONTEXT_MENU_TEXTSELECT  4
#define CONTEXT_MENU_ANCHOR  5
#define CONTEXT_MENU_UNKNOWN  6
#define CONTEXT_MENU_IMGDYNSRC  7
#define CONTEXT_MENU_IMGART  8
#define CONTEXT_MENU_DEBUG  9
#define CONTEXT_MENU_VSCROLL  10
#define CONTEXT_MENU_HSCROLL  11
#define MENUEXT_SHOWDIALOG  0x1
#define DOCHOSTUIFLAG_BROWSER  DOCHOSTUIFLAG_DISABLE_HELP_MENU|DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE
#define HTMLDLG_NOUI  0x10
#define HTMLDLG_MODAL  0x20
#define HTMLDLG_MODELESS  0x40
#define HTMLDLG_PRINT_TEMPLATE  0x80
#define HTMLDLG_VERIFY  0x100
#define PRINT_DONTBOTHERUSER  0x01
#define PRINT_WAITFORCOMPLETION  0x02
EXTERN_C const GUID CGID_MSHTML;
#define CMDSETID_Forms3 CGID_MSHTML
#define SZ_HTML_CLIENTSITE_OBJECTPARAM  L"{d4db6850-5385-11d0-89e9-00a0c90a90ac}"

#ifndef __IHTMLWindow2_FWD_DEFINED__
#define __IHTMLWindow2_FWD_DEFINED__
typedef interface IHTMLWindow2 IHTMLWindow2;
#endif
typedef HRESULT STDAPICALLTYPE SHOWHTMLDIALOGFN(HWND,IMoniker*,VARIANT*,WCHAR*,VARIANT*);
typedef HRESULT STDAPICALLTYPE SHOWHTMLDIALOGEXFN(HWND,IMoniker*,DWORD,VARIANT*,WCHAR*,VARIANT*);
typedef HRESULT STDAPICALLTYPE SHOWMODELESSHTMLDIALOGFN(HWND,IMoniker*,VARIANT*,VARIANT*,IHTMLWindow2**);

STDAPI ShowHTMLDialog(HWND,IMoniker*,VARIANT*,WCHAR*,VARIANT*);
STDAPI ShowHTMLDialogEx(HWND,IMoniker*,DWORD,VARIANT*,WCHAR*,VARIANT*);
STDAPI ShowModelessHTMLDialog(HWND,IMoniker*,VARIANT*,VARIANT*,IHTMLWindow2**);
STDAPI RunHTMLApplication(HINSTANCE,HINSTANCE,LPSTR,int);
STDAPI CreateHTMLPropertyPage(IMoniker*,IPropertyPage**);

extern RPC_IF_HANDLE __MIDL_itf_mshtmhst_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mshtmhst_0000_v0_0_s_ifspec;

#ifndef __IHostDialogHelper_INTERFACE_DEFINED__
#define __IHostDialogHelper_INTERFACE_DEFINED__

EXTERN_C const IID IID_IHostDialogHelper;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("53DEC138-A51E-11d2-861E-00C04FA35C89")IHostDialogHelper:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE ShowHTMLDialog(HWND,IMoniker*,VARIANT*,WCHAR*,VARIANT*,IUnknown*) = 0;
};
#else /* C style interface */
typedef struct IHostDialogHelperVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface) (IHostDialogHelper * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef) (IHostDialogHelper * This);
    ULONG(STDMETHODCALLTYPE *Release) (IHostDialogHelper * This);
    HRESULT(STDMETHODCALLTYPE *ShowHTMLDialog) (IHostDialogHelper * This, HWND hwndParent,IMoniker*, VARIANT * pvarArgIn, WCHAR * pchOptions, VARIANT * pvarArgOut, IUnknown * punkHost);
    END_INTERFACE
} IHostDialogHelperVtbl;

interface IHostDialogHelper {
    CONST_VTBL struct IHostDialogHelperVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IHostDialogHelper_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IHostDialogHelper_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IHostDialogHelper_Release(This)  (This)->lpVtbl->Release(This)
#define IHostDialogHelper_ShowHTMLDialog(This,hwndParent,pMk,pvarArgIn,pchOptions,pvarArgOut,punkHost)  (This)->lpVtbl->ShowHTMLDialog(This,hwndParent,pMk,pvarArgIn,pchOptions,pvarArgOut,punkHost)
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IHostDialogHelper_ShowHTMLDialog_Proxy(IHostDialogHelper * This, HWND hwndParent,IMoniker*, VARIANT * pvarArgIn, WCHAR * pchOptions, VARIANT * pvarArgOut, IUnknown * punkHost);
void __RPC_STUB IHostDialogHelper_ShowHTMLDialog_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __IHostDialogHelper_INTERFACE_DEFINED__ */

EXTERN_C const GUID CLSID_HostDialogHelper;

typedef enum tagDOCHOSTUITYPE {
    DOCHOSTUITYPE_BROWSE = 0,
    DOCHOSTUITYPE_AUTHOR = 1
} DOCHOSTUITYPE;

typedef enum tagDOCHOSTUIDBLCLK {
    DOCHOSTUIDBLCLK_DEFAULT = 0,
    DOCHOSTUIDBLCLK_SHOWPROPERTIES = 1,
    DOCHOSTUIDBLCLK_SHOWCODE = 2
} DOCHOSTUIDBLCLK;

typedef enum tagDOCHOSTUIFLAG {
    DOCHOSTUIFLAG_DIALOG = 0x1,
    DOCHOSTUIFLAG_DISABLE_HELP_MENU = 0x2,
    DOCHOSTUIFLAG_NO3DBORDER = 0x4,
    DOCHOSTUIFLAG_SCROLL_NO = 0x8,
    DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE = 0x10,
    DOCHOSTUIFLAG_OPENNEWWIN = 0x20,
    DOCHOSTUIFLAG_DISABLE_OFFSCREEN = 0x40,
    DOCHOSTUIFLAG_FLAT_SCROLLBAR = 0x80,
    DOCHOSTUIFLAG_DIV_BLOCKDEFAULT = 0x100,
    DOCHOSTUIFLAG_ACTIVATE_CLIENTHIT_ONLY = 0x200,
    DOCHOSTUIFLAG_OVERRIDEBEHAVIORFACTORY = 0x400,
    DOCHOSTUIFLAG_CODEPAGELINKEDFONTS = 0x800,
    DOCHOSTUIFLAG_URL_ENCODING_DISABLE_UTF8 = 0x1000,
    DOCHOSTUIFLAG_URL_ENCODING_ENABLE_UTF8 = 0x2000,
    DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE = 0x4000,
    DOCHOSTUIFLAG_ENABLE_INPLACE_NAVIGATION = 0x10000,
    DOCHOSTUIFLAG_IME_ENABLE_RECONVERSION = 0x20000,
    DOCHOSTUIFLAG_THEME = 0x40000,
    DOCHOSTUIFLAG_NOTHEME = 0x80000,
    DOCHOSTUIFLAG_NOPICS = 0x100000,
    DOCHOSTUIFLAG_NO3DOUTERBORDER = 0x200000,
    DOCHOSTUIFLAG_DISABLE_EDIT_NS_FIXUP = 0x400000,
    DOCHOSTUIFLAG_LOCAL_MACHINE_ACCESS_CHECK = 0x800000,
    DOCHOSTUIFLAG_DISABLE_UNTRUSTEDPROTOCOL = 0x1000000
} DOCHOSTUIFLAG;

#define DOCHOSTUIATOM_ENABLE_HIRES  _T("TridentEnableHiRes")

extern RPC_IF_HANDLE __MIDL_itf_mshtmhst_0273_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mshtmhst_0273_v0_0_s_ifspec;

#ifndef __IDocHostUIHandler_INTERFACE_DEFINED__
#define __IDocHostUIHandler_INTERFACE_DEFINED__

typedef struct _DOCHOSTUIINFO {
    ULONG cbSize;
    DWORD dwFlags;
    DWORD dwDoubleClick;
    OLECHAR *pchHostCss;
    OLECHAR *pchHostNS;
} DOCHOSTUIINFO;

EXTERN_C const IID IID_IDocHostUIHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("bd3f23c0-d43e-11cf-893b-00aa00bdce1a")IDocHostUIHandler:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE ShowContextMenu(DWORD dwID, POINT * ppt, IUnknown * pcmdtReserved, IDispatch * pdispReserved) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHostInfo(DOCHOSTUIINFO *pInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE ShowUI(DWORD dwID, IOleInPlaceActiveObject * pActiveObject, IOleCommandTarget * pCommandTarget, IOleInPlaceFrame * pFrame, IOleInPlaceUIWindow * pDoc) = 0;
    virtual HRESULT STDMETHODCALLTYPE HideUI(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE UpdateUI(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnDocWindowActivate(BOOL fActivate) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(BOOL fActivate) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow * pUIWindow, BOOL fRameWindow) = 0;
    virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpMsg, const GUID * pguidCmdGroup, DWORD nCmdID) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOptionKeyPath(LPOLESTR * pchKey, DWORD dw) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDropTarget(IDropTarget * pDropTarget, IDropTarget ** ppDropTarget) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetExternal(IDispatch ** ppDispatch) = 0;
    virtual HRESULT STDMETHODCALLTYPE TranslateUrl(DWORD dwTranslate, OLECHAR * pchURLIn, OLECHAR ** ppchURLOut) = 0;
    virtual HRESULT STDMETHODCALLTYPE FilterDataObject(IDataObject * pDO, IDataObject ** ppDORet) = 0;
};
#else /* C style interface */
typedef struct IDocHostUIHandlerVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IDocHostUIHandler * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IDocHostUIHandler * This);
    ULONG(STDMETHODCALLTYPE * Release) (IDocHostUIHandler * This);
    HRESULT(STDMETHODCALLTYPE * ShowContextMenu) (IDocHostUIHandler * This, DWORD dwID, POINT * ppt, IUnknown * pcmdtReserved, IDispatch * pdispReserved);
    HRESULT(STDMETHODCALLTYPE * GetHostInfo) (IDocHostUIHandler * This, DOCHOSTUIINFO * pInfo);
    HRESULT(STDMETHODCALLTYPE * ShowUI) (IDocHostUIHandler * This, DWORD dwID, IOleInPlaceActiveObject * pActiveObject, IOleCommandTarget * pCommandTarget, IOleInPlaceFrame * pFrame, IOleInPlaceUIWindow * pDoc);
    HRESULT(STDMETHODCALLTYPE * HideUI) (IDocHostUIHandler * This);
    HRESULT(STDMETHODCALLTYPE * UpdateUI) (IDocHostUIHandler * This);
    HRESULT(STDMETHODCALLTYPE * EnableModeless) (IDocHostUIHandler * This, BOOL fEnable);
    HRESULT(STDMETHODCALLTYPE * OnDocWindowActivate) (IDocHostUIHandler * This, BOOL fActivate);
    HRESULT(STDMETHODCALLTYPE * OnFrameWindowActivate) (IDocHostUIHandler * This, BOOL fActivate);
    HRESULT(STDMETHODCALLTYPE * ResizeBorder) (IDocHostUIHandler * This, LPCRECT prcBorder, IOleInPlaceUIWindow * pUIWindow, BOOL fRameWindow);
    HRESULT(STDMETHODCALLTYPE * TranslateAccelerator) (IDocHostUIHandler * This, LPMSG lpMsg, const GUID * pguidCmdGroup, DWORD nCmdID);
    HRESULT(STDMETHODCALLTYPE * GetOptionKeyPath) (IDocHostUIHandler * This, LPOLESTR * pchKey, DWORD dw);
    HRESULT(STDMETHODCALLTYPE * GetDropTarget) (IDocHostUIHandler * This, IDropTarget * pDropTarget, IDropTarget ** ppDropTarget);
    HRESULT(STDMETHODCALLTYPE * GetExternal) (IDocHostUIHandler * This, IDispatch ** ppDispatch);
    HRESULT(STDMETHODCALLTYPE * TranslateUrl) (IDocHostUIHandler * This, DWORD dwTranslate, OLECHAR * pchURLIn, OLECHAR ** ppchURLOut);
    HRESULT(STDMETHODCALLTYPE * FilterDataObject) (IDocHostUIHandler * This, IDataObject * pDO, IDataObject ** ppDORet);
    END_INTERFACE
} IDocHostUIHandlerVtbl;

interface IDocHostUIHandler {
    CONST_VTBL struct IDocHostUIHandlerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDocHostUIHandler_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IDocHostUIHandler_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IDocHostUIHandler_Release(This)  (This)->lpVtbl->Release(This)
#define IDocHostUIHandler_ShowContextMenu(This,dwID,ppt,pcmdtReserved,pdispReserved)  (This)->lpVtbl->ShowContextMenu(This,dwID,ppt,pcmdtReserved,pdispReserved)
#define IDocHostUIHandler_GetHostInfo(This,pInfo)  (This)->lpVtbl->GetHostInfo(This,pInfo)
#define IDocHostUIHandler_ShowUI(This,dwID,pActiveObject,pCommandTarget,pFrame,pDoc)  (This)->lpVtbl->ShowUI(This,dwID,pActiveObject,pCommandTarget,pFrame,pDoc)
#define IDocHostUIHandler_HideUI(This)  (This)->lpVtbl->HideUI(This)
#define IDocHostUIHandler_UpdateUI(This)  (This)->lpVtbl->UpdateUI(This)
#define IDocHostUIHandler_EnableModeless(This,fEnable)  (This)->lpVtbl->EnableModeless(This,fEnable)
#define IDocHostUIHandler_OnDocWindowActivate(This,fActivate)  (This)->lpVtbl->OnDocWindowActivate(This,fActivate)
#define IDocHostUIHandler_OnFrameWindowActivate(This,fActivate)  (This)->lpVtbl->OnFrameWindowActivate(This,fActivate)
#define IDocHostUIHandler_ResizeBorder(This,prcBorder,pUIWindow,fRameWindow)  (This)->lpVtbl->ResizeBorder(This,prcBorder,pUIWindow,fRameWindow)
#define IDocHostUIHandler_TranslateAccelerator(This,lpMsg,pguidCmdGroup,nCmdID)  (This)->lpVtbl->TranslateAccelerator(This,lpMsg,pguidCmdGroup,nCmdID)
#define IDocHostUIHandler_GetOptionKeyPath(This,pchKey,dw)  (This)->lpVtbl->GetOptionKeyPath(This,pchKey,dw)
#define IDocHostUIHandler_GetDropTarget(This,pDropTarget,ppDropTarget)  (This)->lpVtbl->GetDropTarget(This,pDropTarget,ppDropTarget)
#define IDocHostUIHandler_GetExternal(This,ppDispatch)  (This)->lpVtbl->GetExternal(This,ppDispatch)
#define IDocHostUIHandler_TranslateUrl(This,dwTranslate,pchURLIn,ppchURLOut)  (This)->lpVtbl->TranslateUrl(This,dwTranslate,pchURLIn,ppchURLOut)
#define IDocHostUIHandler_FilterDataObject(This,pDO,ppDORet)  (This)->lpVtbl->FilterDataObject(This,pDO,ppDORet)
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IDocHostUIHandler_ShowContextMenu_Proxy(IDocHostUIHandler * This, DWORD dwID, POINT * ppt, IUnknown * pcmdtReserved, IDispatch * pdispReserved);
void __RPC_STUB IDocHostUIHandler_ShowContextMenu_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDocHostUIHandler_GetHostInfo_Proxy(IDocHostUIHandler * This, DOCHOSTUIINFO * pInfo);
void __RPC_STUB IDocHostUIHandler_GetHostInfo_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDocHostUIHandler_ShowUI_Proxy(IDocHostUIHandler * This, DWORD dwID, IOleInPlaceActiveObject * pActiveObject, IOleCommandTarget * pCommandTarget, IOleInPlaceFrame * pFrame, IOleInPlaceUIWindow * pDoc);
void __RPC_STUB IDocHostUIHandler_ShowUI_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDocHostUIHandler_HideUI_Proxy(IDocHostUIHandler * This);
void __RPC_STUB IDocHostUIHandler_HideUI_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDocHostUIHandler_UpdateUI_Proxy(IDocHostUIHandler * This);
void __RPC_STUB IDocHostUIHandler_UpdateUI_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDocHostUIHandler_EnableModeless_Proxy(IDocHostUIHandler * This, BOOL fEnable);
void __RPC_STUB IDocHostUIHandler_EnableModeless_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDocHostUIHandler_OnDocWindowActivate_Proxy(IDocHostUIHandler * This, BOOL fActivate);
void __RPC_STUB IDocHostUIHandler_OnDocWindowActivate_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDocHostUIHandler_OnFrameWindowActivate_Proxy(IDocHostUIHandler * This, BOOL fActivate);
void __RPC_STUB IDocHostUIHandler_OnFrameWindowActivate_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDocHostUIHandler_ResizeBorder_Proxy(IDocHostUIHandler * This, LPCRECT prcBorder, IOleInPlaceUIWindow * pUIWindow, BOOL fRameWindow);
void __RPC_STUB IDocHostUIHandler_ResizeBorder_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDocHostUIHandler_TranslateAccelerator_Proxy(IDocHostUIHandler * This, LPMSG lpMsg, const GUID * pguidCmdGroup, DWORD nCmdID);
void __RPC_STUB IDocHostUIHandler_TranslateAccelerator_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDocHostUIHandler_GetOptionKeyPath_Proxy(IDocHostUIHandler * This, LPOLESTR * pchKey, DWORD dw);
void __RPC_STUB IDocHostUIHandler_GetOptionKeyPath_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDocHostUIHandler_GetDropTarget_Proxy(IDocHostUIHandler * This, IDropTarget * pDropTarget, IDropTarget ** ppDropTarget);
void __RPC_STUB IDocHostUIHandler_GetDropTarget_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDocHostUIHandler_GetExternal_Proxy(IDocHostUIHandler * This, IDispatch ** ppDispatch);
void __RPC_STUB IDocHostUIHandler_GetExternal_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDocHostUIHandler_TranslateUrl_Proxy(IDocHostUIHandler * This, DWORD dwTranslate, OLECHAR * pchURLIn, OLECHAR ** ppchURLOut);
void __RPC_STUB IDocHostUIHandler_TranslateUrl_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDocHostUIHandler_FilterDataObject_Proxy(IDocHostUIHandler * This, IDataObject * pDO, IDataObject ** ppDORet);
void __RPC_STUB IDocHostUIHandler_FilterDataObject_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __IDocHostUIHandler_INTERFACE_DEFINED__ */

#ifndef __IDocHostUIHandler2_INTERFACE_DEFINED__
#define __IDocHostUIHandler2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IDocHostUIHandler2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3050f6d0-98b5-11cf-bb82-00aa00bdce0b")IDocHostUIHandler2:public IDocHostUIHandler
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetOverrideKeyPath(LPOLESTR * pchKey, DWORD dw) = 0;
};
#else /* C style interface */
typedef struct IDocHostUIHandler2Vtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IDocHostUIHandler2 * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IDocHostUIHandler2 * This);
    ULONG(STDMETHODCALLTYPE * Release) (IDocHostUIHandler2 * This);
    HRESULT(STDMETHODCALLTYPE * ShowContextMenu) (IDocHostUIHandler2 * This, DWORD dwID, POINT * ppt, IUnknown * pcmdtReserved, IDispatch * pdispReserved);
    HRESULT(STDMETHODCALLTYPE * GetHostInfo) (IDocHostUIHandler2 * This, DOCHOSTUIINFO * pInfo);
    HRESULT(STDMETHODCALLTYPE * ShowUI) (IDocHostUIHandler2 * This, DWORD dwID, IOleInPlaceActiveObject * pActiveObject, IOleCommandTarget * pCommandTarget, IOleInPlaceFrame * pFrame, IOleInPlaceUIWindow * pDoc);
    HRESULT(STDMETHODCALLTYPE * HideUI) (IDocHostUIHandler2 * This);
    HRESULT(STDMETHODCALLTYPE * UpdateUI) (IDocHostUIHandler2 * This);
    HRESULT(STDMETHODCALLTYPE * EnableModeless) (IDocHostUIHandler2 * This, BOOL fEnable);
    HRESULT(STDMETHODCALLTYPE * OnDocWindowActivate) (IDocHostUIHandler2 * This, BOOL fActivate);
    HRESULT(STDMETHODCALLTYPE * OnFrameWindowActivate) (IDocHostUIHandler2 * This, BOOL fActivate);
    HRESULT(STDMETHODCALLTYPE * ResizeBorder) (IDocHostUIHandler2 * This, LPCRECT prcBorder, IOleInPlaceUIWindow * pUIWindow, BOOL fRameWindow);
    HRESULT(STDMETHODCALLTYPE * TranslateAccelerator) (IDocHostUIHandler2 * This, LPMSG lpMsg, const GUID * pguidCmdGroup, DWORD nCmdID);
    HRESULT(STDMETHODCALLTYPE * GetOptionKeyPath) (IDocHostUIHandler2 * This, LPOLESTR * pchKey, DWORD dw);
    HRESULT(STDMETHODCALLTYPE * GetDropTarget) (IDocHostUIHandler2 * This, IDropTarget * pDropTarget, IDropTarget ** ppDropTarget);
    HRESULT(STDMETHODCALLTYPE * GetExternal) (IDocHostUIHandler2 * This, IDispatch ** ppDispatch);
    HRESULT(STDMETHODCALLTYPE * TranslateUrl) (IDocHostUIHandler2 * This, DWORD dwTranslate, OLECHAR * pchURLIn, OLECHAR ** ppchURLOut);
    HRESULT(STDMETHODCALLTYPE * FilterDataObject) (IDocHostUIHandler2 * This, IDataObject * pDO, IDataObject ** ppDORet);
    HRESULT(STDMETHODCALLTYPE * GetOverrideKeyPath) (IDocHostUIHandler2 * This, LPOLESTR * pchKey, DWORD dw);
    END_INTERFACE
} IDocHostUIHandler2Vtbl;

interface IDocHostUIHandler2 {
    CONST_VTBL struct IDocHostUIHandler2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDocHostUIHandler2_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IDocHostUIHandler2_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IDocHostUIHandler2_Release(This)  (This)->lpVtbl->Release(This)
#define IDocHostUIHandler2_ShowContextMenu(This,dwID,ppt,pcmdtReserved,pdispReserved)  (This)->lpVtbl->ShowContextMenu(This,dwID,ppt,pcmdtReserved,pdispReserved)
#define IDocHostUIHandler2_GetHostInfo(This,pInfo)  (This)->lpVtbl->GetHostInfo(This,pInfo)
#define IDocHostUIHandler2_ShowUI(This,dwID,pActiveObject,pCommandTarget,pFrame,pDoc)  (This)->lpVtbl->ShowUI(This,dwID,pActiveObject,pCommandTarget,pFrame,pDoc)
#define IDocHostUIHandler2_HideUI(This)  (This)->lpVtbl->HideUI(This)
#define IDocHostUIHandler2_UpdateUI(This)  (This)->lpVtbl->UpdateUI(This)
#define IDocHostUIHandler2_EnableModeless(This,fEnable)  (This)->lpVtbl->EnableModeless(This,fEnable)
#define IDocHostUIHandler2_OnDocWindowActivate(This,fActivate)  (This)->lpVtbl->OnDocWindowActivate(This,fActivate)
#define IDocHostUIHandler2_OnFrameWindowActivate(This,fActivate)  (This)->lpVtbl->OnFrameWindowActivate(This,fActivate)
#define IDocHostUIHandler2_ResizeBorder(This,prcBorder,pUIWindow,fRameWindow)  (This)->lpVtbl->ResizeBorder(This,prcBorder,pUIWindow,fRameWindow)
#define IDocHostUIHandler2_TranslateAccelerator(This,lpMsg,pguidCmdGroup,nCmdID)  (This)->lpVtbl->TranslateAccelerator(This,lpMsg,pguidCmdGroup,nCmdID)
#define IDocHostUIHandler2_GetOptionKeyPath(This,pchKey,dw)  (This)->lpVtbl->GetOptionKeyPath(This,pchKey,dw)
#define IDocHostUIHandler2_GetDropTarget(This,pDropTarget,ppDropTarget)  (This)->lpVtbl->GetDropTarget(This,pDropTarget,ppDropTarget)
#define IDocHostUIHandler2_GetExternal(This,ppDispatch)  (This)->lpVtbl->GetExternal(This,ppDispatch)
#define IDocHostUIHandler2_TranslateUrl(This,dwTranslate,pchURLIn,ppchURLOut)  (This)->lpVtbl->TranslateUrl(This,dwTranslate,pchURLIn,ppchURLOut)
#define IDocHostUIHandler2_FilterDataObject(This,pDO,ppDORet)  (This)->lpVtbl->FilterDataObject(This,pDO,ppDORet)
#define IDocHostUIHandler2_GetOverrideKeyPath(This,pchKey,dw)  (This)->lpVtbl->GetOverrideKeyPath(This,pchKey,dw)
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IDocHostUIHandler2_GetOverrideKeyPath_Proxy(IDocHostUIHandler2 * This, LPOLESTR * pchKey, DWORD dw);
void __RPC_STUB IDocHostUIHandler2_GetOverrideKeyPath_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __IDocHostUIHandler2_INTERFACE_DEFINED__ */

DEFINE_GUID(CGID_DocHostCommandHandler, 0xf38bc242, 0xb950, 0x11d1, 0x89, 0x18, 0x00, 0xc0, 0x4f, 0xc2, 0xc8, 0x36);

extern RPC_IF_HANDLE __MIDL_itf_mshtmhst_0275_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mshtmhst_0275_v0_0_s_ifspec;

#ifndef __ICustomDoc_INTERFACE_DEFINED__
#define __ICustomDoc_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICustomDoc;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3050f3f0-98b5-11cf-bb82-00aa00bdce0b")ICustomDoc:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetUIHandler(IDocHostUIHandler * pUIHandler) = 0;
};
#else /* C style interface */
typedef struct ICustomDocVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (ICustomDoc * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (ICustomDoc * This);
    ULONG(STDMETHODCALLTYPE * Release) (ICustomDoc * This);
    HRESULT(STDMETHODCALLTYPE * SetUIHandler) (ICustomDoc * This, IDocHostUIHandler * pUIHandler);
    END_INTERFACE
} ICustomDocVtbl;

interface ICustomDoc {
    CONST_VTBL struct ICustomDocVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICustomDoc_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ICustomDoc_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ICustomDoc_Release(This)  (This)->lpVtbl->Release(This)
#define ICustomDoc_SetUIHandler(This,pUIHandler)  (This)->lpVtbl->SetUIHandler(This,pUIHandler)
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE ICustomDoc_SetUIHandler_Proxy(ICustomDoc * This, IDocHostUIHandler * pUIHandler);
void __RPC_STUB ICustomDoc_SetUIHandler_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __ICustomDoc_INTERFACE_DEFINED__ */

#ifndef __IDocHostShowUI_INTERFACE_DEFINED__
#define __IDocHostShowUI_INTERFACE_DEFINED__

EXTERN_C const IID IID_IDocHostShowUI;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("c4d244b0-d43e-11cf-893b-00aa00bdce1a")IDocHostShowUI:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE ShowMessage(HWND hwnd, LPOLESTR lpstrText, LPOLESTR lpstrCaption, DWORD dwType, LPOLESTR lpstrHelpFile, DWORD dwHelpContext, LRESULT * plResult) = 0;
    virtual HRESULT STDMETHODCALLTYPE ShowHelp(HWND hwnd, LPOLESTR pszHelpFile, UINT uCommand, DWORD dwData, POINT ptMouse, IDispatch * pDispatchObjectHit) = 0;
};
#else /* C style interface */
typedef struct IDocHostShowUIVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IDocHostShowUI * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IDocHostShowUI * This);
    ULONG(STDMETHODCALLTYPE * Release) (IDocHostShowUI * This);
    HRESULT(STDMETHODCALLTYPE * ShowMessage) (IDocHostShowUI * This, HWND hwnd, LPOLESTR lpstrText, LPOLESTR lpstrCaption, DWORD dwType, LPOLESTR lpstrHelpFile, DWORD dwHelpContext, LRESULT * plResult);
    HRESULT(STDMETHODCALLTYPE * ShowHelp) (IDocHostShowUI * This, HWND hwnd, LPOLESTR pszHelpFile, UINT uCommand, DWORD dwData, POINT ptMouse, IDispatch * pDispatchObjectHit);
    END_INTERFACE
} IDocHostShowUIVtbl;

interface IDocHostShowUI {
    CONST_VTBL struct IDocHostShowUIVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDocHostShowUI_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IDocHostShowUI_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IDocHostShowUI_Release(This)  (This)->lpVtbl->Release(This)
#define IDocHostShowUI_ShowMessage(This,hwnd,lpstrText,lpstrCaption,dwType,lpstrHelpFile,dwHelpContext,plResult)  (This)->lpVtbl->ShowMessage(This,hwnd,lpstrText,lpstrCaption,dwType,lpstrHelpFile,dwHelpContext,plResult)
#define IDocHostShowUI_ShowHelp(This,hwnd,pszHelpFile,uCommand,dwData,ptMouse,pDispatchObjectHit)  (This)->lpVtbl->ShowHelp(This,hwnd,pszHelpFile,uCommand,dwData,ptMouse,pDispatchObjectHit)
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IDocHostShowUI_ShowMessage_Proxy(IDocHostShowUI * This, HWND hwnd, LPOLESTR lpstrText, LPOLESTR lpstrCaption, DWORD dwType, LPOLESTR lpstrHelpFile, DWORD dwHelpContext, LRESULT * plResult);
void __RPC_STUB IDocHostShowUI_ShowMessage_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDocHostShowUI_ShowHelp_Proxy(IDocHostShowUI * This, HWND hwnd, LPOLESTR pszHelpFile, UINT uCommand, DWORD dwData, POINT ptMouse, IDispatch * pDispatchObjectHit);
void __RPC_STUB IDocHostShowUI_ShowHelp_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __IDocHostShowUI_INTERFACE_DEFINED__ */

#define IClassFactory3  IClassFactoryEx
#define IID_IClassFactory3  IID_IClassFactoryEx

extern RPC_IF_HANDLE __MIDL_itf_mshtmhst_0277_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mshtmhst_0277_v0_0_s_ifspec;

#ifndef __IClassFactoryEx_INTERFACE_DEFINED__
#define __IClassFactoryEx_INTERFACE_DEFINED__

EXTERN_C const IID IID_IClassFactoryEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("342D1EA0-AE25-11D1-89C5-006008C3FBFC")IClassFactoryEx:public IClassFactory
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateInstanceWithContext(IUnknown * punkContext, IUnknown * punkOuter, REFIID riid, void **ppv) = 0;
};
#else /* C style interface */
typedef struct IClassFactoryExVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IClassFactoryEx * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IClassFactoryEx * This);
    ULONG(STDMETHODCALLTYPE * Release) (IClassFactoryEx * This);
    HRESULT(STDMETHODCALLTYPE * CreateInstance) (IClassFactoryEx * This, IUnknown * pUnkOuter, REFIID riid, void **ppvObject);
    HRESULT(STDMETHODCALLTYPE * LockServer) (IClassFactoryEx * This, BOOL fLock);
    HRESULT(STDMETHODCALLTYPE * CreateInstanceWithContext) (IClassFactoryEx * This, IUnknown * punkContext, IUnknown * punkOuter, REFIID riid, void **ppv);
    END_INTERFACE
} IClassFactoryExVtbl;

interface IClassFactoryEx {
    CONST_VTBL struct IClassFactoryExVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IClassFactoryEx_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IClassFactoryEx_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IClassFactoryEx_Release(This)  (This)->lpVtbl->Release(This)
#define IClassFactoryEx_CreateInstance(This,pUnkOuter,riid,ppvObject)  (This)->lpVtbl->CreateInstance(This,pUnkOuter,riid,ppvObject)
#define IClassFactoryEx_LockServer(This,fLock)  (This)->lpVtbl->LockServer(This,fLock)
#define IClassFactoryEx_CreateInstanceWithContext(This,punkContext,punkOuter,riid,ppv)  (This)->lpVtbl->CreateInstanceWithContext(This,punkContext,punkOuter,riid,ppv)
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IClassFactoryEx_CreateInstanceWithContext_Proxy(IClassFactoryEx * This, IUnknown * punkContext, IUnknown * punkOuter, REFIID riid, void **ppv);
void __RPC_STUB IClassFactoryEx_CreateInstanceWithContext_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif /* __IClassFactoryEx_INTERFACE_DEFINED__ */

#endif

extern RPC_IF_HANDLE __MIDL_itf_mshtmhst_0278_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mshtmhst_0278_v0_0_s_ifspec;

#ifdef __cplusplus
}
#endif

#endif
