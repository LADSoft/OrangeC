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
#endif /* COM_NO_WINDOWS_H */

#ifndef _OLEIDL_H
#define _OLEIDL_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IOleAdviseHolder_FWD_DEFINED__
#define __IOleAdviseHolder_FWD_DEFINED__
typedef interface IOleAdviseHolder IOleAdviseHolder;
#endif
#ifndef __IOleCache_FWD_DEFINED__
#define __IOleCache_FWD_DEFINED__
typedef interface IOleCache IOleCache;
#endif
#ifndef __IOleCache2_FWD_DEFINED__
#define __IOleCache2_FWD_DEFINED__
typedef interface IOleCache2 IOleCache2;
#endif
#ifndef __IOleCacheControl_FWD_DEFINED__
#define __IOleCacheControl_FWD_DEFINED__
typedef interface IOleCacheControl IOleCacheControl;
#endif
#ifndef __IParseDisplayName_FWD_DEFINED__
#define __IParseDisplayName_FWD_DEFINED__
typedef interface IParseDisplayName IParseDisplayName;
#endif
#ifndef __IOleContainer_FWD_DEFINED__
#define __IOleContainer_FWD_DEFINED__
typedef interface IOleContainer IOleContainer;
#endif
#ifndef __IOleClientSite_FWD_DEFINED__
#define __IOleClientSite_FWD_DEFINED__
typedef interface IOleClientSite IOleClientSite;
#endif
#ifndef __IOleObject_FWD_DEFINED__
#define __IOleObject_FWD_DEFINED__
typedef interface IOleObject IOleObject;
#endif
#ifndef __IOleWindow_FWD_DEFINED__
#define __IOleWindow_FWD_DEFINED__
typedef interface IOleWindow IOleWindow;
#endif
#ifndef __IOleLink_FWD_DEFINED__
#define __IOleLink_FWD_DEFINED__
typedef interface IOleLink IOleLink;
#endif
#ifndef __IOleItemContainer_FWD_DEFINED__
#define __IOleItemContainer_FWD_DEFINED__
typedef interface IOleItemContainer IOleItemContainer;
#endif
#ifndef __IOleInPlaceUIWindow_FWD_DEFINED__
#define __IOleInPlaceUIWindow_FWD_DEFINED__
typedef interface IOleInPlaceUIWindow IOleInPlaceUIWindow;
#endif
#ifndef __IOleInPlaceActiveObject_FWD_DEFINED__
#define __IOleInPlaceActiveObject_FWD_DEFINED__
typedef interface IOleInPlaceActiveObject IOleInPlaceActiveObject;
#endif
#ifndef __IOleInPlaceFrame_FWD_DEFINED__
#define __IOleInPlaceFrame_FWD_DEFINED__
typedef interface IOleInPlaceFrame IOleInPlaceFrame;
#endif
#ifndef __IOleInPlaceObject_FWD_DEFINED__
#define __IOleInPlaceObject_FWD_DEFINED__
typedef interface IOleInPlaceObject IOleInPlaceObject;
#endif
#ifndef __IOleInPlaceSite_FWD_DEFINED__
#define __IOleInPlaceSite_FWD_DEFINED__
typedef interface IOleInPlaceSite IOleInPlaceSite;
#endif
#ifndef __IContinue_FWD_DEFINED__
#define __IContinue_FWD_DEFINED__
typedef interface IContinue IContinue;
#endif
#ifndef __IViewObject_FWD_DEFINED__
#define __IViewObject_FWD_DEFINED__
typedef interface IViewObject IViewObject;
#endif
#ifndef __IViewObject2_FWD_DEFINED__
#define __IViewObject2_FWD_DEFINED__
typedef interface IViewObject2 IViewObject2;
#endif
#ifndef __IDropSource_FWD_DEFINED__
#define __IDropSource_FWD_DEFINED__
typedef interface IDropSource IDropSource;
#endif
#ifndef __IDropTarget_FWD_DEFINED__
#define __IDropTarget_FWD_DEFINED__
typedef interface IDropTarget IDropTarget;
#endif
#ifndef __IEnumOLEVERB_FWD_DEFINED__
#define __IEnumOLEVERB_FWD_DEFINED__
typedef interface IEnumOLEVERB IEnumOLEVERB;
#endif

#include "objidl.h"

#ifdef __cplusplus
extern "C" {
#endif

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void*);

extern RPC_IF_HANDLE __MIDL_itf_oleidl_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_oleidl_0000_v0_0_s_ifspec;

#ifndef __IOleAdviseHolder_INTERFACE_DEFINED__
#define __IOleAdviseHolder_INTERFACE_DEFINED__

typedef IOleAdviseHolder *LPOLEADVISEHOLDER;
EXTERN_C const IID IID_IOleAdviseHolder;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000111-0000-0000-C000-000000000046")
IOleAdviseHolder:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Advise(IAdviseSink*,DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unadvise(DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumAdvise(IEnumSTATDATA**) = 0;
    virtual HRESULT STDMETHODCALLTYPE SendOnRename(IMoniker*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SendOnSave(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE SendOnClose(void) = 0;
};
#else
typedef struct IOleAdviseHolderVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IOleAdviseHolder*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IOleAdviseHolder*);
    ULONG(STDMETHODCALLTYPE *Release)(IOleAdviseHolder*);
    HRESULT(STDMETHODCALLTYPE *Advise)(IOleAdviseHolder*,IAdviseSink*,DWORD*);
    HRESULT(STDMETHODCALLTYPE *Unadvise)(IOleAdviseHolder*,DWORD);
    HRESULT(STDMETHODCALLTYPE *EnumAdvise)(IOleAdviseHolder*,IEnumSTATDATA**);
    HRESULT(STDMETHODCALLTYPE *SendOnRename)(IOleAdviseHolder*,IMoniker*);
    HRESULT(STDMETHODCALLTYPE *SendOnSave)(IOleAdviseHolder*);
    HRESULT(STDMETHODCALLTYPE *SendOnClose)(IOleAdviseHolder*);
    END_INTERFACE
} IOleAdviseHolderVtbl;

interface IOleAdviseHolder {
    CONST_VTBL struct IOleAdviseHolderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleAdviseHolder_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IOleAdviseHolder_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleAdviseHolder_Release(This)  (This)->lpVtbl->Release(This)
#define IOleAdviseHolder_Advise(This,pAdvise,pdwConnection)  (This)->lpVtbl->Advise(This,pAdvise,pdwConnection)
#define IOleAdviseHolder_Unadvise(This,dwConnection)  (This)->lpVtbl->Unadvise(This,dwConnection)
#define IOleAdviseHolder_EnumAdvise(This,ppenumAdvise)  (This)->lpVtbl->EnumAdvise(This,ppenumAdvise)
#define IOleAdviseHolder_SendOnRename(This,pmk)  (This)->lpVtbl->SendOnRename(This,pmk)
#define IOleAdviseHolder_SendOnSave(This)  (This)->lpVtbl->SendOnSave(This)
#define IOleAdviseHolder_SendOnClose(This)  (This)->lpVtbl->SendOnClose(This)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IOleAdviseHolder_Advise_Proxy(IOleAdviseHolder*,IAdviseSink*,DWORD*);
void __RPC_STUB IOleAdviseHolder_Advise_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleAdviseHolder_Unadvise_Proxy(IOleAdviseHolder*,DWORD);
void __RPC_STUB IOleAdviseHolder_Unadvise_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleAdviseHolder_EnumAdvise_Proxy(IOleAdviseHolder*,IEnumSTATDATA**);
void __RPC_STUB IOleAdviseHolder_EnumAdvise_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleAdviseHolder_SendOnRename_Proxy(IOleAdviseHolder*,IMoniker*);
void __RPC_STUB IOleAdviseHolder_SendOnRename_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleAdviseHolder_SendOnSave_Proxy(IOleAdviseHolder*);
void __RPC_STUB IOleAdviseHolder_SendOnSave_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleAdviseHolder_SendOnClose_Proxy(IOleAdviseHolder*);
void __RPC_STUB IOleAdviseHolder_SendOnClose_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IOleAdviseHolder_INTERFACE_DEFINED__ */


#ifndef __IOleCache_INTERFACE_DEFINED__
#define __IOleCache_INTERFACE_DEFINED__

typedef  IOleCache  *LPOLECACHE;
EXTERN_C const IID IID_IOleCache;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0000011e-0000-0000-C000-000000000046")
IOleCache:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Cache(FORMATETC*,DWORD,DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Uncache(DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumCache(IEnumSTATDATA**) = 0;
    virtual HRESULT STDMETHODCALLTYPE InitCache(IDataObject*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetData(FORMATETC*,STGMEDIUM*,BOOL) = 0;
};
#else
typedef struct IOleCacheVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IOleCache*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IOleCache*);
    ULONG(STDMETHODCALLTYPE *Release)(IOleCache*);
    HRESULT(STDMETHODCALLTYPE *Cache)(IOleCache*,FORMATETC*,DWORD,DWORD*);
    HRESULT(STDMETHODCALLTYPE *Uncache)(IOleCache*,DWORD);
    HRESULT(STDMETHODCALLTYPE *EnumCache)(IOleCache*,IEnumSTATDATA**);
    HRESULT(STDMETHODCALLTYPE *InitCache)(IOleCache*,IDataObject*);
    HRESULT(STDMETHODCALLTYPE *SetData)(IOleCache*,FORMATETC*,STGMEDIUM*,BOOL);
    END_INTERFACE
} IOleCacheVtbl;

interface IOleCache {
    CONST_VTBL struct IOleCacheVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleCache_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IOleCache_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleCache_Release(This)  (This)->lpVtbl->Release(This)
#define IOleCache_Cache(This,pformatetc,advf,pdwConnection)  (This)->lpVtbl->Cache(This,pformatetc,advf,pdwConnection)
#define IOleCache_Uncache(This,dwConnection)  (This)->lpVtbl->Uncache(This,dwConnection)
#define IOleCache_EnumCache(This,ppenumSTATDATA)  (This)->lpVtbl->EnumCache(This,ppenumSTATDATA)
#define IOleCache_InitCache(This,pDataObject)  (This)->lpVtbl->InitCache(This,pDataObject)
#define IOleCache_SetData(This,pformatetc,pmedium,fRelease)  (This)->lpVtbl->SetData(This,pformatetc,pmedium,fRelease)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IOleCache_Cache_Proxy(IOleCache*,FORMATETC*,DWORD,DWORD*);
void __RPC_STUB IOleCache_Cache_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleCache_Uncache_Proxy(IOleCache*,DWORD);
void __RPC_STUB IOleCache_Uncache_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleCache_EnumCache_Proxy(IOleCache*,IEnumSTATDATA**);
void __RPC_STUB IOleCache_EnumCache_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleCache_InitCache_Proxy(IOleCache*,IDataObject*);
void __RPC_STUB IOleCache_InitCache_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleCache_SetData_Proxy(IOleCache*,FORMATETC*,STGMEDIUM*,BOOL);
void __RPC_STUB IOleCache_SetData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IOleCache_INTERFACE_DEFINED__ */


#ifndef __IOleCache2_INTERFACE_DEFINED__
#define __IOleCache2_INTERFACE_DEFINED__

typedef  IOleCache2 *LPOLECACHE2;

#define UPDFCACHE_NODATACACHE  0x1
#define UPDFCACHE_ONSAVECACHE  0x2
#define UPDFCACHE_ONSTOPCACHE  0x4
#define UPDFCACHE_NORMALCACHE  0x8
#define UPDFCACHE_IFBLANK  0x10
#define UPDFCACHE_ONLYIFBLANK  0x80000000
#define UPDFCACHE_IFBLANKORONSAVECACHE  (UPDFCACHE_IFBLANK|UPDFCACHE_ONSAVECACHE)
#define UPDFCACHE_ALL  ((DWORD)~UPDFCACHE_ONLYIFBLANK)
#define UPDFCACHE_ALLBUTNODATACACHE  (UPDFCACHE_ALL&(DWORD)~UPDFCACHE_NODATACACHE)

typedef enum tagDISCARDCACHE {
    DISCARDCACHE_SAVEIFDIRTY = 0,
    DISCARDCACHE_NOSAVE = 1
} DISCARDCACHE;

EXTERN_C const IID IID_IOleCache2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000128-0000-0000-C000-000000000046")
IOleCache2:public IOleCache
{
    public:
    virtual HRESULT STDMETHODCALLTYPE UpdateCache(LPDATAOBJECT,DWORD,LPVOID) = 0;
    virtual HRESULT STDMETHODCALLTYPE DiscardCache(DWORD) = 0;
};
#else
typedef struct IOleCache2Vtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IOleCache2*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IOleCache2*);
    ULONG(STDMETHODCALLTYPE *Release)(IOleCache2*);
    HRESULT(STDMETHODCALLTYPE *Cache)(IOleCache2*,FORMATETC*,DWORD,DWORD*);
    HRESULT(STDMETHODCALLTYPE *Uncache)(IOleCache2*,DWORD);
    HRESULT(STDMETHODCALLTYPE *EnumCache)(IOleCache2*,IEnumSTATDATA**);
    HRESULT(STDMETHODCALLTYPE *InitCache)(IOleCache2*,IDataObject*);
    HRESULT(STDMETHODCALLTYPE *SetData)(IOleCache2*,FORMATETC*,STGMEDIUM*,BOOL);
    HRESULT(STDMETHODCALLTYPE *UpdateCache)(IOleCache2*,LPDATAOBJECT,DWORD,LPVOID);
    HRESULT(STDMETHODCALLTYPE *DiscardCache)(IOleCache2*,DWORD);
    END_INTERFACE
} IOleCache2Vtbl;

interface IOleCache2 {
    CONST_VTBL struct IOleCache2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleCache2_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IOleCache2_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleCache2_Release(This)  (This)->lpVtbl->Release(This)
#define IOleCache2_Cache(This,pformatetc,advf,pdwConnection)  (This)->lpVtbl->Cache(This,pformatetc,advf,pdwConnection)
#define IOleCache2_Uncache(This,dwConnection)  (This)->lpVtbl->Uncache(This,dwConnection)
#define IOleCache2_EnumCache(This,ppenumSTATDATA)  (This)->lpVtbl->EnumCache(This,ppenumSTATDATA)
#define IOleCache2_InitCache(This,pDataObject)  (This)->lpVtbl->InitCache(This,pDataObject)
#define IOleCache2_SetData(This,pformatetc,pmedium,fRelease)  (This)->lpVtbl->SetData(This,pformatetc,pmedium,fRelease)
#define IOleCache2_UpdateCache(This,pDataObject,grfUpdf,pReserved)  (This)->lpVtbl->UpdateCache(This,pDataObject,grfUpdf,pReserved)
#define IOleCache2_DiscardCache(This,dwDiscardOptions)  (This)->lpVtbl->DiscardCache(This,dwDiscardOptions)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IOleCache2_RemoteUpdateCache_Proxy(IOleCache2*,LPDATAOBJECT,DWORD,LONG_PTR);
void __RPC_STUB IOleCache2_RemoteUpdateCache_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleCache2_DiscardCache_Proxy(IOleCache2*,DWORD);
void __RPC_STUB IOleCache2_DiscardCache_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IOleCache2_INTERFACE_DEFINED__ */


#ifndef __IOleCacheControl_INTERFACE_DEFINED__
#define __IOleCacheControl_INTERFACE_DEFINED__

typedef IOleCacheControl  *LPOLECACHECONTROL;
EXTERN_C const IID IID_IOleCacheControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000129-0000-0000-C000-000000000046")
IOleCacheControl:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnRun(LPDATAOBJECT) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnStop(void) = 0;
};
#else
typedef struct IOleCacheControlVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IOleCacheControl*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IOleCacheControl*);
    ULONG(STDMETHODCALLTYPE *Release)(IOleCacheControl*);
    HRESULT(STDMETHODCALLTYPE *OnRun)(IOleCacheControl*,LPDATAOBJECT);
    HRESULT(STDMETHODCALLTYPE *OnStop)(IOleCacheControl*);
    END_INTERFACE
} IOleCacheControlVtbl;

interface IOleCacheControl {
    CONST_VTBL struct IOleCacheControlVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleCacheControl_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IOleCacheControl_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleCacheControl_Release(This)  (This)->lpVtbl->Release(This)
#define IOleCacheControl_OnRun(This,pDataObject)  (This)->lpVtbl->OnRun(This,pDataObject)
#define IOleCacheControl_OnStop(This)  (This)->lpVtbl->OnStop(This)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IOleCacheControl_OnRun_Proxy(IOleCacheControl*,LPDATAOBJECT);
void __RPC_STUB IOleCacheControl_OnRun_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleCacheControl_OnStop_Proxy(IOleCacheControl*);
void __RPC_STUB IOleCacheControl_OnStop_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IOleCacheControl_INTERFACE_DEFINED__ */


#ifndef __IParseDisplayName_INTERFACE_DEFINED__
#define __IParseDisplayName_INTERFACE_DEFINED__

typedef IParseDisplayName *LPPARSEDISPLAYNAME;
EXTERN_C const IID IID_IParseDisplayName;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0000011a-0000-0000-C000-000000000046")
IParseDisplayName:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE ParseDisplayName(IBindCtx*,LPOLESTR,ULONG*,IMoniker**) = 0;
};
#else
typedef struct IParseDisplayNameVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IParseDisplayName*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IParseDisplayName*);
    ULONG(STDMETHODCALLTYPE *Release)(IParseDisplayName*);
    HRESULT(STDMETHODCALLTYPE *ParseDisplayName)(IParseDisplayName*,IBindCtx*,LPOLESTR,ULONG*,IMoniker**);
    END_INTERFACE
} IParseDisplayNameVtbl;

interface IParseDisplayName {
    CONST_VTBL struct IParseDisplayNameVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IParseDisplayName_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IParseDisplayName_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IParseDisplayName_Release(This)  (This)->lpVtbl->Release(This)
#define IParseDisplayName_ParseDisplayName(This,pbc,pszDisplayName,pchEaten,ppmkOut)  (This)->lpVtbl->ParseDisplayName(This,pbc,pszDisplayName,pchEaten,ppmkOut)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IParseDisplayName_ParseDisplayName_Proxy(IParseDisplayName*,IBindCtx*,LPOLESTR,ULONG*,IMoniker**);
void __RPC_STUB IParseDisplayName_ParseDisplayName_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IParseDisplayName_INTERFACE_DEFINED__ */


#ifndef __IOleContainer_INTERFACE_DEFINED__
#define __IOleContainer_INTERFACE_DEFINED__

typedef IOleContainer *LPOLECONTAINER;
EXTERN_C const IID IID_IOleContainer;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0000011b-0000-0000-C000-000000000046")
IOleContainer:public IParseDisplayName
{
    public:
    virtual HRESULT STDMETHODCALLTYPE EnumObjects(DWORD,IEnumUnknown**) = 0;
    virtual HRESULT STDMETHODCALLTYPE LockContainer(BOOL) = 0;
};
#else
typedef struct IOleContainerVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IOleContainer*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IOleContainer*);
    ULONG(STDMETHODCALLTYPE *Release)(IOleContainer*);
    HRESULT(STDMETHODCALLTYPE *ParseDisplayName)(IOleContainer*,IBindCtx*,LPOLESTR,ULONG*,IMoniker**);
    HRESULT(STDMETHODCALLTYPE *EnumObjects)(IOleContainer*,DWORD,IEnumUnknown**);
    HRESULT(STDMETHODCALLTYPE *LockContainer)(IOleContainer*,BOOL);
    END_INTERFACE
} IOleContainerVtbl;

interface IOleContainer {
    CONST_VTBL struct IOleContainerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleContainer_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IOleContainer_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleContainer_Release(This)  (This)->lpVtbl->Release(This)
#define IOleContainer_ParseDisplayName(This,pbc,pszDisplayName,pchEaten,ppmkOut)  (This)->lpVtbl->ParseDisplayName(This,pbc,pszDisplayName,pchEaten,ppmkOut)
#define IOleContainer_EnumObjects(This,grfFlags,ppenum)  (This)->lpVtbl->EnumObjects(This,grfFlags,ppenum)
#define IOleContainer_LockContainer(This,fLock)  (This)->lpVtbl->LockContainer(This,fLock)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IOleContainer_EnumObjects_Proxy(IOleContainer*,DWORD,IEnumUnknown**);
void __RPC_STUB IOleContainer_EnumObjects_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleContainer_LockContainer_Proxy(IOleContainer*,BOOL);
void __RPC_STUB IOleContainer_LockContainer_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IOleContainer_INTERFACE_DEFINED__ */


#ifndef __IOleClientSite_INTERFACE_DEFINED__
#define __IOleClientSite_INTERFACE_DEFINED__

typedef IOleClientSite *LPOLECLIENTSITE;
EXTERN_C const IID IID_IOleClientSite;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000118-0000-0000-C000-000000000046")
IOleClientSite:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SaveObject(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMoniker(DWORD,DWORD,IMoniker**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ShowObject(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL) = 0;
    virtual HRESULT STDMETHODCALLTYPE RequestNewObjectLayout(void) = 0;
};
#else
typedef struct IOleClientSiteVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IOleClientSite*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IOleClientSite*);
    ULONG(STDMETHODCALLTYPE *Release)(IOleClientSite*);
    HRESULT(STDMETHODCALLTYPE *SaveObject)(IOleClientSite*);
    HRESULT(STDMETHODCALLTYPE *GetMoniker)(IOleClientSite*,DWORD,DWORD,IMoniker**);
    HRESULT(STDMETHODCALLTYPE *GetContainer)(IOleClientSite*,IOleContainer**);
    HRESULT(STDMETHODCALLTYPE *ShowObject)(IOleClientSite*);
    HRESULT(STDMETHODCALLTYPE *OnShowWindow)(IOleClientSite*,BOOL);
    HRESULT(STDMETHODCALLTYPE *RequestNewObjectLayout)(IOleClientSite*);
    END_INTERFACE
} IOleClientSiteVtbl;

interface IOleClientSite {
    CONST_VTBL struct IOleClientSiteVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleClientSite_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IOleClientSite_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleClientSite_Release(This)  (This)->lpVtbl->Release(This)
#define IOleClientSite_SaveObject(This)  (This)->lpVtbl->SaveObject(This)
#define IOleClientSite_GetMoniker(This,dwAssign,dwWhichMoniker,ppmk)  (This)->lpVtbl->GetMoniker(This,dwAssign,dwWhichMoniker,ppmk)
#define IOleClientSite_GetContainer(This,ppContainer)  (This)->lpVtbl->GetContainer(This,ppContainer)
#define IOleClientSite_ShowObject(This)  (This)->lpVtbl->ShowObject(This)
#define IOleClientSite_OnShowWindow(This,fShow)  (This)->lpVtbl->OnShowWindow(This,fShow)
#define IOleClientSite_RequestNewObjectLayout(This)  (This)->lpVtbl->RequestNewObjectLayout(This)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IOleClientSite_SaveObject_Proxy(IOleClientSite*);
void __RPC_STUB IOleClientSite_SaveObject_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleClientSite_GetMoniker_Proxy(IOleClientSite*,DWORD,DWORD,IMoniker**);
void __RPC_STUB IOleClientSite_GetMoniker_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleClientSite_GetContainer_Proxy(IOleClientSite*,IOleContainer**);
void __RPC_STUB IOleClientSite_GetContainer_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleClientSite_ShowObject_Proxy(IOleClientSite*);
void __RPC_STUB IOleClientSite_ShowObject_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleClientSite_OnShowWindow_Proxy(IOleClientSite*,BOOL);
void __RPC_STUB IOleClientSite_OnShowWindow_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleClientSite_RequestNewObjectLayout_Proxy(IOleClientSite*);
void __RPC_STUB IOleClientSite_RequestNewObjectLayout_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IOleClientSite_INTERFACE_DEFINED__ */


#ifndef __IOleObject_INTERFACE_DEFINED__
#define __IOleObject_INTERFACE_DEFINED__

typedef IOleObject *LPOLEOBJECT;

typedef enum tagOLEGETMONIKER {
    OLEGETMONIKER_ONLYIFTHERE = 1,
    OLEGETMONIKER_FORCEASSIGN = 2,
    OLEGETMONIKER_UNASSIGN = 3,
    OLEGETMONIKER_TEMPFORUSER = 4
} OLEGETMONIKER;

typedef enum tagOLEWHICHMK {
    OLEWHICHMK_CONTAINER = 1,
    OLEWHICHMK_OBJREL = 2,
    OLEWHICHMK_OBJFULL = 3
} OLEWHICHMK;

typedef enum tagUSERCLASSTYPE {
    USERCLASSTYPE_FULL = 1,
    USERCLASSTYPE_SHORT = 2,
    USERCLASSTYPE_APPNAME = 3
} USERCLASSTYPE;

typedef enum tagOLEMISC {
    OLEMISC_RECOMPOSEONRESIZE = 0x1,
    OLEMISC_ONLYICONIC = 0x2,
    OLEMISC_INSERTNOTREPLACE = 0x4,
    OLEMISC_STATIC = 0x8,
    OLEMISC_CANTLINKINSIDE = 0x10,
    OLEMISC_CANLINKBYOLE1 = 0x20,
    OLEMISC_ISLINKOBJECT = 0x40,
    OLEMISC_INSIDEOUT = 0x80,
    OLEMISC_ACTIVATEWHENVISIBLE = 0x100,
    OLEMISC_RENDERINGISDEVICEINDEPENDENT = 0x200,
    OLEMISC_INVISIBLEATRUNTIME = 0x400,
    OLEMISC_ALWAYSRUN = 0x800,
    OLEMISC_ACTSLIKEBUTTON = 0x1000,
    OLEMISC_ACTSLIKELABEL = 0x2000,
    OLEMISC_NOUIACTIVATE = 0x4000,
    OLEMISC_ALIGNABLE = 0x8000,
    OLEMISC_SIMPLEFRAME = 0x10000,
    OLEMISC_SETCLIENTSITEFIRST = 0x20000,
    OLEMISC_IMEMODE = 0x40000,
    OLEMISC_IGNOREACTIVATEWHENVISIBLE = 0x80000,
    OLEMISC_WANTSTOMENUMERGE = 0x100000,
    OLEMISC_SUPPORTSMULTILEVELUNDO = 0x200000
} OLEMISC;

typedef enum tagOLECLOSE {
    OLECLOSE_SAVEIFDIRTY = 0,
    OLECLOSE_NOSAVE = 1,
    OLECLOSE_PROMPTSAVE = 2
} OLECLOSE;

EXTERN_C const IID IID_IOleObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000112-0000-0000-C000-000000000046")
IOleObject:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetClientSite(IOleClientSite*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetClientSite(IOleClientSite**) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetHostNames(LPCOLESTR,LPCOLESTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE Close(DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMoniker(DWORD,IMoniker*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMoniker(DWORD,DWORD,IMoniker**) = 0;
    virtual HRESULT STDMETHODCALLTYPE InitFromData(IDataObject*,BOOL,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetClipboardData(DWORD,IDataObject**) = 0;
    virtual HRESULT STDMETHODCALLTYPE DoVerb(LONG,LPMSG,IOleClientSite*,LONG,HWND,LPCRECT) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumVerbs(IEnumOLEVERB**) = 0;
    virtual HRESULT STDMETHODCALLTYPE Update(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsUpToDate(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetUserClassID(CLSID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetUserType(DWORD,LPOLESTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetExtent(DWORD,SIZEL*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetExtent(DWORD,SIZEL*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Advise(IAdviseSink*,DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unadvise(DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumAdvise(IEnumSTATDATA**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMiscStatus(DWORD,DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetColorScheme(LOGPALETTE*) = 0;
};
#else
typedef struct IOleObjectVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IOleObject*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IOleObject*);
    ULONG(STDMETHODCALLTYPE *Release)(IOleObject*);
    HRESULT(STDMETHODCALLTYPE *SetClientSite)(IOleObject*,IOleClientSite*);
    HRESULT(STDMETHODCALLTYPE *GetClientSite)(IOleObject*,IOleClientSite**);
    HRESULT(STDMETHODCALLTYPE *SetHostNames)(IOleObject*,LPCOLESTR,LPCOLESTR);
    HRESULT(STDMETHODCALLTYPE *Close)(IOleObject*,DWORD);
    HRESULT(STDMETHODCALLTYPE *SetMoniker)(IOleObject*,DWORD,IMoniker*);
    HRESULT(STDMETHODCALLTYPE *GetMoniker)(IOleObject*,DWORD,DWORD,IMoniker**);
    HRESULT(STDMETHODCALLTYPE *InitFromData)(IOleObject*,IDataObject*,BOOL,DWORD);
    HRESULT(STDMETHODCALLTYPE *GetClipboardData)(IOleObject*,DWORD,IDataObject**);
    HRESULT(STDMETHODCALLTYPE *DoVerb)(IOleObject*,LONG,LPMSG,IOleClientSite*,LONG,HWND,LPCRECT);
    HRESULT(STDMETHODCALLTYPE *EnumVerbs)(IOleObject*,IEnumOLEVERB**);
    HRESULT(STDMETHODCALLTYPE *Update)(IOleObject*);
    HRESULT(STDMETHODCALLTYPE *IsUpToDate)(IOleObject*);
    HRESULT(STDMETHODCALLTYPE *GetUserClassID)(IOleObject*,CLSID*);
    HRESULT(STDMETHODCALLTYPE *GetUserType)(IOleObject*,DWORD,LPOLESTR*);
    HRESULT(STDMETHODCALLTYPE *SetExtent)(IOleObject*,DWORD,SIZEL*);
    HRESULT(STDMETHODCALLTYPE *GetExtent)(IOleObject*,DWORD,SIZEL*);
    HRESULT(STDMETHODCALLTYPE *Advise)(IOleObject*,IAdviseSink*,DWORD*);
    HRESULT(STDMETHODCALLTYPE *Unadvise)(IOleObject*,DWORD);
    HRESULT(STDMETHODCALLTYPE *EnumAdvise)(IOleObject*,IEnumSTATDATA**);
    HRESULT(STDMETHODCALLTYPE *GetMiscStatus)(IOleObject*,DWORD,DWORD*);
    HRESULT(STDMETHODCALLTYPE *SetColorScheme)(IOleObject*,LOGPALETTE*);
    END_INTERFACE
} IOleObjectVtbl;

interface IOleObject {
    CONST_VTBL struct IOleObjectVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleObject_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IOleObject_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleObject_Release(This)  (This)->lpVtbl->Release(This)
#define IOleObject_SetClientSite(This,pClientSite)  (This)->lpVtbl->SetClientSite(This,pClientSite)
#define IOleObject_GetClientSite(This,ppClientSite)  (This)->lpVtbl->GetClientSite(This,ppClientSite)
#define IOleObject_SetHostNames(This,szContainerApp,szContainerObj)  (This)->lpVtbl->SetHostNames(This,szContainerApp,szContainerObj)
#define IOleObject_Close(This,dwSaveOption)  (This)->lpVtbl->Close(This,dwSaveOption)
#define IOleObject_SetMoniker(This,dwWhichMoniker,pmk)  (This)->lpVtbl->SetMoniker(This,dwWhichMoniker,pmk)
#define IOleObject_GetMoniker(This,dwAssign,dwWhichMoniker,ppmk)  (This)->lpVtbl->GetMoniker(This,dwAssign,dwWhichMoniker,ppmk)
#define IOleObject_InitFromData(This,pDataObject,fCreation,dwReserved)  (This)->lpVtbl->InitFromData(This,pDataObject,fCreation,dwReserved)
#define IOleObject_GetClipboardData(This,dwReserved,ppDataObject)  (This)->lpVtbl->GetClipboardData(This,dwReserved,ppDataObject)
#define IOleObject_DoVerb(This,iVerb,lpmsg,pActiveSite,lindex,hwndParent,lprcPosRect)  (This)->lpVtbl->DoVerb(This,iVerb,lpmsg,pActiveSite,lindex,hwndParent,lprcPosRect)
#define IOleObject_EnumVerbs(This,ppEnumOleVerb)  (This)->lpVtbl->EnumVerbs(This,ppEnumOleVerb)
#define IOleObject_Update(This)  (This)->lpVtbl->Update(This)
#define IOleObject_IsUpToDate(This)  (This)->lpVtbl->IsUpToDate(This)
#define IOleObject_GetUserClassID(This,pClsid)  (This)->lpVtbl->GetUserClassID(This,pClsid)
#define IOleObject_GetUserType(This,dwFormOfType,pszUserType)  (This)->lpVtbl->GetUserType(This,dwFormOfType,pszUserType)
#define IOleObject_SetExtent(This,dwDrawAspect,psizel)  (This)->lpVtbl->SetExtent(This,dwDrawAspect,psizel)
#define IOleObject_GetExtent(This,dwDrawAspect,psizel)  (This)->lpVtbl->GetExtent(This,dwDrawAspect,psizel)
#define IOleObject_Advise(This,pAdvSink,pdwConnection)  (This)->lpVtbl->Advise(This,pAdvSink,pdwConnection)
#define IOleObject_Unadvise(This,dwConnection)  (This)->lpVtbl->Unadvise(This,dwConnection)
#define IOleObject_EnumAdvise(This,ppenumAdvise)  (This)->lpVtbl->EnumAdvise(This,ppenumAdvise)
#define IOleObject_GetMiscStatus(This,dwAspect,pdwStatus)  (This)->lpVtbl->GetMiscStatus(This,dwAspect,pdwStatus)
#define IOleObject_SetColorScheme(This,pLogpal)  (This)->lpVtbl->SetColorScheme(This,pLogpal)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IOleObject_SetClientSite_Proxy(IOleObject*,IOleClientSite*);
void __RPC_STUB IOleObject_SetClientSite_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_GetClientSite_Proxy(IOleObject*,IOleClientSite**);
void __RPC_STUB IOleObject_GetClientSite_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_SetHostNames_Proxy(IOleObject*,LPCOLESTR,LPCOLESTR);
void __RPC_STUB IOleObject_SetHostNames_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_Close_Proxy(IOleObject*,DWORD);
void __RPC_STUB IOleObject_Close_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_SetMoniker_Proxy(IOleObject*,DWORD,IMoniker*);
void __RPC_STUB IOleObject_SetMoniker_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_GetMoniker_Proxy(IOleObject*,DWORD,DWORD,IMoniker**);
void __RPC_STUB IOleObject_GetMoniker_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_InitFromData_Proxy(IOleObject*,IDataObject*,BOOL,DWORD);
void __RPC_STUB IOleObject_InitFromData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_GetClipboardData_Proxy(IOleObject*,DWORD,IDataObject**);
void __RPC_STUB IOleObject_GetClipboardData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_DoVerb_Proxy(IOleObject*,LONG,LPMSG,IOleClientSite*,LONG,HWND,LPCRECT);
void __RPC_STUB IOleObject_DoVerb_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_EnumVerbs_Proxy(IOleObject*,IEnumOLEVERB**);
void __RPC_STUB IOleObject_EnumVerbs_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_Update_Proxy(IOleObject*);
void __RPC_STUB IOleObject_Update_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_IsUpToDate_Proxy(IOleObject*);
void __RPC_STUB IOleObject_IsUpToDate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_GetUserClassID_Proxy(IOleObject*,CLSID*);
void __RPC_STUB IOleObject_GetUserClassID_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_GetUserType_Proxy(IOleObject*,DWORD,LPOLESTR*);
void __RPC_STUB IOleObject_GetUserType_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_SetExtent_Proxy(IOleObject*,DWORD,SIZEL*);
void __RPC_STUB IOleObject_SetExtent_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_GetExtent_Proxy(IOleObject*,DWORD,SIZEL*);
void __RPC_STUB IOleObject_GetExtent_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_Advise_Proxy(IOleObject*,IAdviseSink*,DWORD*);
void __RPC_STUB IOleObject_Advise_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_Unadvise_Proxy(IOleObject*,DWORD);
void __RPC_STUB IOleObject_Unadvise_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_EnumAdvise_Proxy(IOleObject*,IEnumSTATDATA**);
void __RPC_STUB IOleObject_EnumAdvise_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_GetMiscStatus_Proxy(IOleObject*,DWORD,DWORD*);
void __RPC_STUB IOleObject_GetMiscStatus_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleObject_SetColorScheme_Proxy(IOleObject*,LOGPALETTE*);
void __RPC_STUB IOleObject_SetColorScheme_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IOleObject_INTERFACE_DEFINED__ */


#ifndef __IOLETypes_INTERFACE_DEFINED__
#define __IOLETypes_INTERFACE_DEFINED__

typedef enum tagOLERENDER {
    OLERENDER_NONE = 0,
    OLERENDER_DRAW = 1,
    OLERENDER_FORMAT = 2,
    OLERENDER_ASIS = 3
} OLERENDER;

typedef OLERENDER *LPOLERENDER;

typedef struct tagOBJECTDESCRIPTOR {
    ULONG cbSize;
    CLSID clsid;
    DWORD dwDrawAspect;
    SIZEL sizel;
    POINTL pointl;
    DWORD dwStatus;
    DWORD dwFullUserTypeName;
    DWORD dwSrcOfCopy;
} OBJECTDESCRIPTOR;

typedef struct tagOBJECTDESCRIPTOR *POBJECTDESCRIPTOR;
typedef struct tagOBJECTDESCRIPTOR *LPOBJECTDESCRIPTOR;
typedef struct tagOBJECTDESCRIPTOR LINKSRCDESCRIPTOR;
typedef struct tagOBJECTDESCRIPTOR *PLINKSRCDESCRIPTOR;
typedef struct tagOBJECTDESCRIPTOR *LPLINKSRCDESCRIPTOR;

extern RPC_IF_HANDLE IOLETypes_v0_0_c_ifspec;
extern RPC_IF_HANDLE IOLETypes_v0_0_s_ifspec;

#endif /* __IOLETypes_INTERFACE_DEFINED__ */


#ifndef __IOleWindow_INTERFACE_DEFINED__
#define __IOleWindow_INTERFACE_DEFINED__

typedef IOleWindow *LPOLEWINDOW;
EXTERN_C const IID IID_IOleWindow;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000114-0000-0000-C000-000000000046")
IOleWindow:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetWindow(HWND*) = 0;
    virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL) = 0;
};
#else
typedef struct IOleWindowVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IOleWindow*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IOleWindow*);
    ULONG(STDMETHODCALLTYPE *Release)(IOleWindow*);
    HRESULT(STDMETHODCALLTYPE *GetWindow)(IOleWindow*,HWND*);
    HRESULT(STDMETHODCALLTYPE *ContextSensitiveHelp)(IOleWindow*,BOOL);
    END_INTERFACE
} IOleWindowVtbl;

interface IOleWindow {
    CONST_VTBL struct IOleWindowVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleWindow_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IOleWindow_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleWindow_Release(This)  (This)->lpVtbl->Release(This)
#define IOleWindow_GetWindow(This,phwnd)  (This)->lpVtbl->GetWindow(This,phwnd)
#define IOleWindow_ContextSensitiveHelp(This,fEnterMode)  (This)->lpVtbl->ContextSensitiveHelp(This,fEnterMode)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IOleWindow_GetWindow_Proxy(IOleWindow*,HWND*);
void __RPC_STUB IOleWindow_GetWindow_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleWindow_ContextSensitiveHelp_Proxy(IOleWindow*,BOOL);
void __RPC_STUB IOleWindow_ContextSensitiveHelp_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IOleWindow_INTERFACE_DEFINED__ */


#ifndef __IOleLink_INTERFACE_DEFINED__
#define __IOleLink_INTERFACE_DEFINED__

typedef IOleLink *LPOLELINK;

typedef enum tagOLEUPDATE {
    OLEUPDATE_ALWAYS = 1,
    OLEUPDATE_ONCALL = 3
} OLEUPDATE;

typedef OLEUPDATE *LPOLEUPDATE;
typedef OLEUPDATE *POLEUPDATE;

typedef enum tagOLELINKBIND {
    OLELINKBIND_EVENIFCLASSDIFF = 1
} OLELINKBIND;

EXTERN_C const IID IID_IOleLink;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0000011d-0000-0000-C000-000000000046")
IOleLink:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetUpdateOptions(DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetUpdateOptions(DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSourceMoniker(IMoniker*,REFCLSID) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSourceMoniker(IMoniker**) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSourceDisplayName(LPCOLESTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSourceDisplayName(LPOLESTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE BindToSource(DWORD,IBindCtx*) = 0;
    virtual HRESULT STDMETHODCALLTYPE BindIfRunning(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetBoundSource(IUnknown**) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnbindSource(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Update(IBindCtx*) = 0;
};
#else
typedef struct IOleLinkVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IOleLink*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IOleLink*);
    ULONG(STDMETHODCALLTYPE *Release)(IOleLink*);
    HRESULT(STDMETHODCALLTYPE *SetUpdateOptions)(IOleLink*,DWORD);
    HRESULT(STDMETHODCALLTYPE *GetUpdateOptions)(IOleLink*,DWORD*);
    HRESULT(STDMETHODCALLTYPE *SetSourceMoniker)(IOleLink*,IMoniker*,REFCLSID);
    HRESULT(STDMETHODCALLTYPE *GetSourceMoniker)(IOleLink*,IMoniker**);
    HRESULT(STDMETHODCALLTYPE *SetSourceDisplayName)(IOleLink*,LPCOLESTR);
    HRESULT(STDMETHODCALLTYPE *GetSourceDisplayName)(IOleLink*,LPOLESTR*);
    HRESULT(STDMETHODCALLTYPE *BindToSource)(IOleLink*,DWORD,IBindCtx*);
    HRESULT(STDMETHODCALLTYPE *BindIfRunning)(IOleLink*);
    HRESULT(STDMETHODCALLTYPE *GetBoundSource)(IOleLink*,IUnknown**);
    HRESULT(STDMETHODCALLTYPE *UnbindSource)(IOleLink*);
    HRESULT(STDMETHODCALLTYPE *Update)(IOleLink*,IBindCtx*);
    END_INTERFACE
} IOleLinkVtbl;

interface IOleLink {
    CONST_VTBL struct IOleLinkVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleLink_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IOleLink_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleLink_Release(This)  (This)->lpVtbl->Release(This)
#define IOleLink_SetUpdateOptions(This,dwUpdateOpt)  (This)->lpVtbl->SetUpdateOptions(This,dwUpdateOpt)
#define IOleLink_GetUpdateOptions(This,pdwUpdateOpt)  (This)->lpVtbl->GetUpdateOptions(This,pdwUpdateOpt)
#define IOleLink_SetSourceMoniker(This,pmk,rclsid)  (This)->lpVtbl->SetSourceMoniker(This,pmk,rclsid)
#define IOleLink_GetSourceMoniker(This,ppmk)  (This)->lpVtbl->GetSourceMoniker(This,ppmk)
#define IOleLink_SetSourceDisplayName(This,pszStatusText)  (This)->lpVtbl->SetSourceDisplayName(This,pszStatusText)
#define IOleLink_GetSourceDisplayName(This,ppszDisplayName)  (This)->lpVtbl->GetSourceDisplayName(This,ppszDisplayName)
#define IOleLink_BindToSource(This,bindflags,pbc)  (This)->lpVtbl->BindToSource(This,bindflags,pbc)
#define IOleLink_BindIfRunning(This)  (This)->lpVtbl->BindIfRunning(This)
#define IOleLink_GetBoundSource(This,ppunk)  (This)->lpVtbl->GetBoundSource(This,ppunk)
#define IOleLink_UnbindSource(This)  (This)->lpVtbl->UnbindSource(This)
#define IOleLink_Update(This,pbc)  (This)->lpVtbl->Update(This,pbc)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IOleLink_SetUpdateOptions_Proxy(IOleLink*,DWORD);
void __RPC_STUB IOleLink_SetUpdateOptions_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleLink_GetUpdateOptions_Proxy(IOleLink*,DWORD*);
void __RPC_STUB IOleLink_GetUpdateOptions_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleLink_SetSourceMoniker_Proxy(IOleLink*,IMoniker*,REFCLSID);
void __RPC_STUB IOleLink_SetSourceMoniker_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleLink_GetSourceMoniker_Proxy(IOleLink*,IMoniker**);
void __RPC_STUB IOleLink_GetSourceMoniker_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleLink_SetSourceDisplayName_Proxy(IOleLink*,LPCOLESTR);
void __RPC_STUB IOleLink_SetSourceDisplayName_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleLink_GetSourceDisplayName_Proxy(IOleLink*,LPOLESTR*);
void __RPC_STUB IOleLink_GetSourceDisplayName_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleLink_BindToSource_Proxy(IOleLink*,DWORD,IBindCtx*);
void __RPC_STUB IOleLink_BindToSource_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleLink_BindIfRunning_Proxy(IOleLink*);
void __RPC_STUB IOleLink_BindIfRunning_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleLink_GetBoundSource_Proxy(IOleLink*,IUnknown**);
void __RPC_STUB IOleLink_GetBoundSource_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleLink_UnbindSource_Proxy(IOleLink*);
void __RPC_STUB IOleLink_UnbindSource_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleLink_Update_Proxy(IOleLink*,IBindCtx*);
void __RPC_STUB IOleLink_Update_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IOleLink_INTERFACE_DEFINED__ */


#ifndef __IOleItemContainer_INTERFACE_DEFINED__
#define __IOleItemContainer_INTERFACE_DEFINED__

typedef  IOleItemContainer  *LPOLEITEMCONTAINER;

typedef enum tagBINDSPEED {
    BINDSPEED_INDEFINITE = 1,
    BINDSPEED_MODERATE = 2,
    BINDSPEED_IMMEDIATE = 3
} BINDSPEED;

typedef enum tagOLECONTF {
    OLECONTF_EMBEDDINGS = 1,
    OLECONTF_LINKS = 2,
    OLECONTF_OTHERS = 4,
    OLECONTF_ONLYUSER = 8,
    OLECONTF_ONLYIFRUNNING = 16
} OLECONTF;

EXTERN_C const IID IID_IOleItemContainer;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0000011c-0000-0000-C000-000000000046")
IOleItemContainer:public IOleContainer
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetObject(LPOLESTR,DWORD,IBindCtx*,REFIID,void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetObjectStorage(LPOLESTR,IBindCtx*,REFIID,void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsRunning(LPOLESTR) = 0;
};
#else
typedef struct IOleItemContainerVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IOleItemContainer*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IOleItemContainer*);
    ULONG(STDMETHODCALLTYPE *Release)(IOleItemContainer*);
    HRESULT(STDMETHODCALLTYPE *ParseDisplayName)(IOleItemContainer*,IBindCtx*,LPOLESTR,ULONG*,IMoniker**);
    HRESULT(STDMETHODCALLTYPE *EnumObjects)(IOleItemContainer*,DWORD,IEnumUnknown**);
    HRESULT(STDMETHODCALLTYPE *LockContainer)(IOleItemContainer*,BOOL);
    HRESULT(STDMETHODCALLTYPE *GetObject)(IOleItemContainer*,LPOLESTR,DWORD,IBindCtx*,REFIID,void**);
    HRESULT(STDMETHODCALLTYPE *GetObjectStorage)(IOleItemContainer*,LPOLESTR,IBindCtx*,REFIID,void**);
    HRESULT(STDMETHODCALLTYPE *IsRunning)(IOleItemContainer*,LPOLESTR);
    END_INTERFACE
} IOleItemContainerVtbl;

interface IOleItemContainer {
    CONST_VTBL struct IOleItemContainerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleItemContainer_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IOleItemContainer_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleItemContainer_Release(This)  (This)->lpVtbl->Release(This)
#define IOleItemContainer_ParseDisplayName(This,pbc,pszDisplayName,pchEaten,ppmkOut)  (This)->lpVtbl->ParseDisplayName(This,pbc,pszDisplayName,pchEaten,ppmkOut)
#define IOleItemContainer_EnumObjects(This,grfFlags,ppenum)  (This)->lpVtbl->EnumObjects(This,grfFlags,ppenum)
#define IOleItemContainer_LockContainer(This,fLock)  (This)->lpVtbl->LockContainer(This,fLock)
#define IOleItemContainer_GetObject(This,pszItem,dwSpeedNeeded,pbc,riid,ppvObject)  (This)->lpVtbl->GetObject(This,pszItem,dwSpeedNeeded,pbc,riid,ppvObject)
#define IOleItemContainer_GetObjectStorage(This,pszItem,pbc,riid,ppvStorage)  (This)->lpVtbl->GetObjectStorage(This,pszItem,pbc,riid,ppvStorage)
#define IOleItemContainer_IsRunning(This,pszItem)  (This)->lpVtbl->IsRunning(This,pszItem)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IOleItemContainer_GetObject_Proxy(IOleItemContainer*,LPOLESTR,DWORD,IBindCtx*,REFIID,void**);
void __RPC_STUB IOleItemContainer_GetObject_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleItemContainer_GetObjectStorage_Proxy(IOleItemContainer*,LPOLESTR,IBindCtx*,REFIID,void**);
void __RPC_STUB IOleItemContainer_GetObjectStorage_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleItemContainer_IsRunning_Proxy(IOleItemContainer*,LPOLESTR);
void __RPC_STUB IOleItemContainer_IsRunning_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IOleItemContainer_INTERFACE_DEFINED__ */


#ifndef __IOleInPlaceUIWindow_INTERFACE_DEFINED__
#define __IOleInPlaceUIWindow_INTERFACE_DEFINED__

typedef IOleInPlaceUIWindow *LPOLEINPLACEUIWINDOW;
EXTERN_C const IID IID_IOleInPlaceUIWindow;

typedef RECT BORDERWIDTHS;
typedef LPRECT LPBORDERWIDTHS;
typedef LPCRECT LPCBORDERWIDTHS;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000115-0000-0000-C000-000000000046")
IOleInPlaceUIWindow:public IOleWindow
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetBorder(LPRECT) = 0;
    virtual HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetActiveObject(IOleInPlaceActiveObject*,LPCOLESTR) = 0;
};
#else
typedef struct IOleInPlaceUIWindowVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IOleInPlaceUIWindow*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IOleInPlaceUIWindow*);
    ULONG(STDMETHODCALLTYPE *Release)(IOleInPlaceUIWindow*);
    HRESULT(STDMETHODCALLTYPE *GetWindow)(IOleInPlaceUIWindow*,HWND*);
    HRESULT(STDMETHODCALLTYPE *ContextSensitiveHelp)(IOleInPlaceUIWindow*,BOOL);
    HRESULT(STDMETHODCALLTYPE *GetBorder)(IOleInPlaceUIWindow*,LPRECT);
    HRESULT(STDMETHODCALLTYPE *RequestBorderSpace)(IOleInPlaceUIWindow*,LPCBORDERWIDTHS);
    HRESULT(STDMETHODCALLTYPE *SetBorderSpace)(IOleInPlaceUIWindow*,LPCBORDERWIDTHS);
    HRESULT(STDMETHODCALLTYPE *SetActiveObject)(IOleInPlaceUIWindow*,IOleInPlaceActiveObject*,LPCOLESTR);
    END_INTERFACE
} IOleInPlaceUIWindowVtbl;

interface IOleInPlaceUIWindow {
    CONST_VTBL struct IOleInPlaceUIWindowVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleInPlaceUIWindow_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IOleInPlaceUIWindow_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleInPlaceUIWindow_Release(This)  (This)->lpVtbl->Release(This)
#define IOleInPlaceUIWindow_GetWindow(This,phwnd)  (This)->lpVtbl->GetWindow(This,phwnd)
#define IOleInPlaceUIWindow_ContextSensitiveHelp(This,fEnterMode)  (This)->lpVtbl->ContextSensitiveHelp(This,fEnterMode)
#define IOleInPlaceUIWindow_GetBorder(This,lprectBorder)  (This)->lpVtbl->GetBorder(This,lprectBorder)
#define IOleInPlaceUIWindow_RequestBorderSpace(This,pborderwidths)  (This)->lpVtbl->RequestBorderSpace(This,pborderwidths)
#define IOleInPlaceUIWindow_SetBorderSpace(This,pborderwidths)  (This)->lpVtbl->SetBorderSpace(This,pborderwidths)
#define IOleInPlaceUIWindow_SetActiveObject(This,pActiveObject,pszObjName)  (This)->lpVtbl->SetActiveObject(This,pActiveObject,pszObjName)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IOleInPlaceUIWindow_GetBorder_Proxy(IOleInPlaceUIWindow*,LPRECT);
void __RPC_STUB IOleInPlaceUIWindow_GetBorder_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceUIWindow_RequestBorderSpace_Proxy(IOleInPlaceUIWindow*,LPCBORDERWIDTHS);
void __RPC_STUB IOleInPlaceUIWindow_RequestBorderSpace_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceUIWindow_SetBorderSpace_Proxy(IOleInPlaceUIWindow*,LPCBORDERWIDTHS);
void __RPC_STUB IOleInPlaceUIWindow_SetBorderSpace_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceUIWindow_SetActiveObject_Proxy(IOleInPlaceUIWindow*,IOleInPlaceActiveObject*,LPCOLESTR);
void __RPC_STUB IOleInPlaceUIWindow_SetActiveObject_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IOleInPlaceUIWindow_INTERFACE_DEFINED__ */


#ifndef __IOleInPlaceActiveObject_INTERFACE_DEFINED__
#define __IOleInPlaceActiveObject_INTERFACE_DEFINED__

typedef IOleInPlaceActiveObject *LPOLEINPLACEACTIVEOBJECT;
EXTERN_C const IID IID_IOleInPlaceActiveObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000117-0000-0000-C000-000000000046")
IOleInPlaceActiveObject:public IOleWindow
{
    public:
    virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(BOOL) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnDocWindowActivate(BOOL) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResizeBorder(LPCRECT,IOleInPlaceUIWindow*,BOOL) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnableModeless(BOOL) = 0;
};
#else
typedef struct IOleInPlaceActiveObjectVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IOleInPlaceActiveObject*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IOleInPlaceActiveObject*);
    ULONG(STDMETHODCALLTYPE *Release)(IOleInPlaceActiveObject*);
    HRESULT(STDMETHODCALLTYPE *GetWindow)(IOleInPlaceActiveObject*,HWND*);
    HRESULT(STDMETHODCALLTYPE *ContextSensitiveHelp)(IOleInPlaceActiveObject*,BOOL);
    HRESULT(STDMETHODCALLTYPE *TranslateAccelerator)(IOleInPlaceActiveObject*,LPMSG);
    HRESULT(STDMETHODCALLTYPE *OnFrameWindowActivate)(IOleInPlaceActiveObject*,BOOL);
    HRESULT(STDMETHODCALLTYPE *OnDocWindowActivate)(IOleInPlaceActiveObject*,BOOL);
    HRESULT(STDMETHODCALLTYPE *ResizeBorder)(IOleInPlaceActiveObject*,LPCRECT,IOleInPlaceUIWindow*,BOOL);
    HRESULT(STDMETHODCALLTYPE *EnableModeless)(IOleInPlaceActiveObject*,BOOL);
    END_INTERFACE
} IOleInPlaceActiveObjectVtbl;

interface IOleInPlaceActiveObject {
    CONST_VTBL struct IOleInPlaceActiveObjectVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleInPlaceActiveObject_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IOleInPlaceActiveObject_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleInPlaceActiveObject_Release(This)  (This)->lpVtbl->Release(This)
#define IOleInPlaceActiveObject_GetWindow(This,phwnd)  (This)->lpVtbl->GetWindow(This,phwnd)
#define IOleInPlaceActiveObject_ContextSensitiveHelp(This,fEnterMode)  (This)->lpVtbl->ContextSensitiveHelp(This,fEnterMode)
#define IOleInPlaceActiveObject_TranslateAccelerator(This,lpmsg)  (This)->lpVtbl->TranslateAccelerator(This,lpmsg)
#define IOleInPlaceActiveObject_OnFrameWindowActivate(This,fActivate)  (This)->lpVtbl->OnFrameWindowActivate(This,fActivate)
#define IOleInPlaceActiveObject_OnDocWindowActivate(This,fActivate)  (This)->lpVtbl->OnDocWindowActivate(This,fActivate)
#define IOleInPlaceActiveObject_ResizeBorder(This,prcBorder,pUIWindow,fFrameWindow)  (This)->lpVtbl->ResizeBorder(This,prcBorder,pUIWindow,fFrameWindow)
#define IOleInPlaceActiveObject_EnableModeless(This,fEnable)  (This)->lpVtbl->EnableModeless(This,fEnable)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_RemoteTranslateAccelerator_Proxy(IOleInPlaceActiveObject*);
void __RPC_STUB IOleInPlaceActiveObject_RemoteTranslateAccelerator_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_OnFrameWindowActivate_Proxy(IOleInPlaceActiveObject*,BOOL);
void __RPC_STUB IOleInPlaceActiveObject_OnFrameWindowActivate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_OnDocWindowActivate_Proxy(IOleInPlaceActiveObject*,BOOL);
void __RPC_STUB IOleInPlaceActiveObject_OnDocWindowActivate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_RemoteResizeBorder_Proxy(IOleInPlaceActiveObject*,LPCRECT,REFIID,IOleInPlaceUIWindow*,BOOL);
void __RPC_STUB IOleInPlaceActiveObject_RemoteResizeBorder_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_EnableModeless_Proxy(IOleInPlaceActiveObject*,BOOL);
void __RPC_STUB IOleInPlaceActiveObject_EnableModeless_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IOleInPlaceActiveObject_INTERFACE_DEFINED__ */


#ifndef __IOleInPlaceFrame_INTERFACE_DEFINED__
#define __IOleInPlaceFrame_INTERFACE_DEFINED__

typedef IOleInPlaceFrame *LPOLEINPLACEFRAME;
EXTERN_C const IID IID_IOleInPlaceFrame;

typedef struct tagOIFI {
    UINT cb;
    BOOL fMDIApp;
    HWND hwndFrame;
    HACCEL haccel;
    UINT cAccelEntries;
} OLEINPLACEFRAMEINFO, *LPOLEINPLACEFRAMEINFO;

typedef struct tagOleMenuGroupWidths {
    LONG width[6];
} OLEMENUGROUPWIDTHS, *LPOLEMENUGROUPWIDTHS;

typedef HGLOBAL HOLEMENU;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000116-0000-0000-C000-000000000046")
IOleInPlaceFrame:public IOleInPlaceUIWindow
{
    public:
    virtual HRESULT STDMETHODCALLTYPE InsertMenus(HMENU,LPOLEMENUGROUPWIDTHS) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMenu(HMENU,HOLEMENU,HWND) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveMenus(HMENU) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetStatusText(LPCOLESTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnableModeless(BOOL) = 0;
    virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG,WORD) = 0;
};
#else
typedef struct IOleInPlaceFrameVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IOleInPlaceFrame*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IOleInPlaceFrame*);
    ULONG(STDMETHODCALLTYPE *Release)(IOleInPlaceFrame*);
    HRESULT(STDMETHODCALLTYPE *GetWindow)(IOleInPlaceFrame*,HWND*);
    HRESULT(STDMETHODCALLTYPE *ContextSensitiveHelp)(IOleInPlaceFrame*,BOOL);
    HRESULT(STDMETHODCALLTYPE *GetBorder)(IOleInPlaceFrame*,LPRECT);
    HRESULT(STDMETHODCALLTYPE *RequestBorderSpace)(IOleInPlaceFrame*,LPCBORDERWIDTHS);
    HRESULT(STDMETHODCALLTYPE *SetBorderSpace)(IOleInPlaceFrame*,LPCBORDERWIDTHS);
    HRESULT(STDMETHODCALLTYPE *SetActiveObject)(IOleInPlaceFrame*,IOleInPlaceActiveObject*,LPCOLESTR);
    HRESULT(STDMETHODCALLTYPE *InsertMenus)(IOleInPlaceFrame*,HMENU,LPOLEMENUGROUPWIDTHS);
    HRESULT(STDMETHODCALLTYPE *SetMenu)(IOleInPlaceFrame*,HMENU,HOLEMENU,HWND);
    HRESULT(STDMETHODCALLTYPE *RemoveMenus)(IOleInPlaceFrame*,HMENU);
    HRESULT(STDMETHODCALLTYPE *SetStatusText)(IOleInPlaceFrame*,LPCOLESTR);
    HRESULT(STDMETHODCALLTYPE *EnableModeless)(IOleInPlaceFrame*,BOOL);
    HRESULT(STDMETHODCALLTYPE *TranslateAccelerator)(IOleInPlaceFrame*,LPMSG,WORD);
    END_INTERFACE
} IOleInPlaceFrameVtbl;

interface IOleInPlaceFrame {
    CONST_VTBL struct IOleInPlaceFrameVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleInPlaceFrame_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IOleInPlaceFrame_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleInPlaceFrame_Release(This)  (This)->lpVtbl->Release(This)
#define IOleInPlaceFrame_GetWindow(This,phwnd)  (This)->lpVtbl->GetWindow(This,phwnd)
#define IOleInPlaceFrame_ContextSensitiveHelp(This,fEnterMode)  (This)->lpVtbl->ContextSensitiveHelp(This,fEnterMode)
#define IOleInPlaceFrame_GetBorder(This,lprectBorder)  (This)->lpVtbl->GetBorder(This,lprectBorder)
#define IOleInPlaceFrame_RequestBorderSpace(This,pborderwidths)  (This)->lpVtbl->RequestBorderSpace(This,pborderwidths)
#define IOleInPlaceFrame_SetBorderSpace(This,pborderwidths)  (This)->lpVtbl->SetBorderSpace(This,pborderwidths)
#define IOleInPlaceFrame_SetActiveObject(This,pActiveObject,pszObjName)  (This)->lpVtbl->SetActiveObject(This,pActiveObject,pszObjName)
#define IOleInPlaceFrame_InsertMenus(This,hmenuShared,lpMenuWidths)  (This)->lpVtbl->InsertMenus(This,hmenuShared,lpMenuWidths)
#define IOleInPlaceFrame_SetMenu(This,hmenuShared,holemenu,hwndActiveObject)  (This)->lpVtbl->SetMenu(This,hmenuShared,holemenu,hwndActiveObject)
#define IOleInPlaceFrame_RemoveMenus(This,hmenuShared)  (This)->lpVtbl->RemoveMenus(This,hmenuShared)
#define IOleInPlaceFrame_SetStatusText(This,pszStatusText)  (This)->lpVtbl->SetStatusText(This,pszStatusText)
#define IOleInPlaceFrame_EnableModeless(This,fEnable)  (This)->lpVtbl->EnableModeless(This,fEnable)
#define IOleInPlaceFrame_TranslateAccelerator(This,lpmsg,wID)  (This)->lpVtbl->TranslateAccelerator(This,lpmsg,wID)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IOleInPlaceFrame_InsertMenus_Proxy(IOleInPlaceFrame*,HMENU,LPOLEMENUGROUPWIDTHS);
void __RPC_STUB IOleInPlaceFrame_InsertMenus_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceFrame_SetMenu_Proxy(IOleInPlaceFrame*,HMENU,HOLEMENU,HWND);
void __RPC_STUB IOleInPlaceFrame_SetMenu_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceFrame_RemoveMenus_Proxy(IOleInPlaceFrame*,HMENU);
void __RPC_STUB IOleInPlaceFrame_RemoveMenus_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceFrame_SetStatusText_Proxy(IOleInPlaceFrame*,LPCOLESTR);
void __RPC_STUB IOleInPlaceFrame_SetStatusText_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceFrame_EnableModeless_Proxy(IOleInPlaceFrame*,BOOL);
void __RPC_STUB IOleInPlaceFrame_EnableModeless_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceFrame_TranslateAccelerator_Proxy(IOleInPlaceFrame*,LPMSG,WORD);
void __RPC_STUB IOleInPlaceFrame_TranslateAccelerator_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IOleInPlaceFrame_INTERFACE_DEFINED__ */


#ifndef __IOleInPlaceObject_INTERFACE_DEFINED__
#define __IOleInPlaceObject_INTERFACE_DEFINED__

typedef IOleInPlaceObject *LPOLEINPLACEOBJECT;
EXTERN_C const IID IID_IOleInPlaceObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000113-0000-0000-C000-000000000046")
IOleInPlaceObject:public IOleWindow
{
    public:
    virtual HRESULT STDMETHODCALLTYPE InPlaceDeactivate(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE UIDeactivate(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetObjectRects(LPCRECT,LPCRECT) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReactivateAndUndo(void) = 0;
};
#else
typedef struct IOleInPlaceObjectVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IOleInPlaceObject*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IOleInPlaceObject*);
    ULONG(STDMETHODCALLTYPE *Release)(IOleInPlaceObject*);
    HRESULT(STDMETHODCALLTYPE *GetWindow)(IOleInPlaceObject*,HWND*);
    HRESULT(STDMETHODCALLTYPE *ContextSensitiveHelp)(IOleInPlaceObject*,BOOL);
    HRESULT(STDMETHODCALLTYPE *InPlaceDeactivate)(IOleInPlaceObject*);
    HRESULT(STDMETHODCALLTYPE *UIDeactivate)(IOleInPlaceObject*);
    HRESULT(STDMETHODCALLTYPE *SetObjectRects)(IOleInPlaceObject*,LPCRECT,LPCRECT);
    HRESULT(STDMETHODCALLTYPE *ReactivateAndUndo)(IOleInPlaceObject*);
    END_INTERFACE
} IOleInPlaceObjectVtbl;

interface IOleInPlaceObject {
    CONST_VTBL struct IOleInPlaceObjectVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleInPlaceObject_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IOleInPlaceObject_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleInPlaceObject_Release(This)  (This)->lpVtbl->Release(This)
#define IOleInPlaceObject_GetWindow(This,phwnd)  (This)->lpVtbl->GetWindow(This,phwnd)
#define IOleInPlaceObject_ContextSensitiveHelp(This,fEnterMode)  (This)->lpVtbl->ContextSensitiveHelp(This,fEnterMode)
#define IOleInPlaceObject_InPlaceDeactivate(This)  (This)->lpVtbl->InPlaceDeactivate(This)
#define IOleInPlaceObject_UIDeactivate(This)  (This)->lpVtbl->UIDeactivate(This)
#define IOleInPlaceObject_SetObjectRects(This,lprcPosRect,lprcClipRect)  (This)->lpVtbl->SetObjectRects(This,lprcPosRect,lprcClipRect)
#define IOleInPlaceObject_ReactivateAndUndo(This)  (This)->lpVtbl->ReactivateAndUndo(This)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IOleInPlaceObject_InPlaceDeactivate_Proxy(IOleInPlaceObject*);
void __RPC_STUB IOleInPlaceObject_InPlaceDeactivate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceObject_UIDeactivate_Proxy(IOleInPlaceObject*);
void __RPC_STUB IOleInPlaceObject_UIDeactivate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceObject_SetObjectRects_Proxy(IOleInPlaceObject*,LPCRECT,LPCRECT);
void __RPC_STUB IOleInPlaceObject_SetObjectRects_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceObject_ReactivateAndUndo_Proxy(IOleInPlaceObject*);
void __RPC_STUB IOleInPlaceObject_ReactivateAndUndo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IOleInPlaceObject_INTERFACE_DEFINED__ */


#ifndef __IOleInPlaceSite_INTERFACE_DEFINED__
#define __IOleInPlaceSite_INTERFACE_DEFINED__

typedef IOleInPlaceSite *LPOLEINPLACESITE;
EXTERN_C const IID IID_IOleInPlaceSite;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000119-0000-0000-C000-000000000046")
IOleInPlaceSite:public IOleWindow
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnUIActivate(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetWindowContext(IOleInPlaceFrame**,IOleInPlaceUIWindow**,LPRECT,LPRECT,LPOLEINPLACEFRAMEINFO) = 0;
    virtual HRESULT STDMETHODCALLTYPE Scroll(SIZE) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE DiscardUndoState(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT) = 0;
};
#else
typedef struct IOleInPlaceSiteVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IOleInPlaceSite*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IOleInPlaceSite*);
    ULONG(STDMETHODCALLTYPE *Release)(IOleInPlaceSite*);
    HRESULT(STDMETHODCALLTYPE *GetWindow)(IOleInPlaceSite*,HWND*);
    HRESULT(STDMETHODCALLTYPE *ContextSensitiveHelp)(IOleInPlaceSite*,BOOL);
    HRESULT(STDMETHODCALLTYPE *CanInPlaceActivate)(IOleInPlaceSite*);
    HRESULT(STDMETHODCALLTYPE *OnInPlaceActivate)(IOleInPlaceSite*);
    HRESULT(STDMETHODCALLTYPE *OnUIActivate)(IOleInPlaceSite*);
    HRESULT(STDMETHODCALLTYPE *GetWindowContext)(IOleInPlaceSite*,IOleInPlaceFrame**,IOleInPlaceUIWindow**,LPRECT,LPRECT,LPOLEINPLACEFRAMEINFO);
    HRESULT(STDMETHODCALLTYPE *Scroll)(IOleInPlaceSite*,SIZE);
    HRESULT(STDMETHODCALLTYPE *OnUIDeactivate)(IOleInPlaceSite*,BOOL);
    HRESULT(STDMETHODCALLTYPE *OnInPlaceDeactivate)(IOleInPlaceSite*);
    HRESULT(STDMETHODCALLTYPE *DiscardUndoState)(IOleInPlaceSite*);
    HRESULT(STDMETHODCALLTYPE *DeactivateAndUndo)(IOleInPlaceSite*);
    HRESULT(STDMETHODCALLTYPE *OnPosRectChange)(IOleInPlaceSite*,LPCRECT);
    END_INTERFACE
} IOleInPlaceSiteVtbl;

interface IOleInPlaceSite {
    CONST_VTBL struct IOleInPlaceSiteVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IOleInPlaceSite_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IOleInPlaceSite_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IOleInPlaceSite_Release(This)  (This)->lpVtbl->Release(This)
#define IOleInPlaceSite_GetWindow(This,phwnd)  (This)->lpVtbl->GetWindow(This,phwnd)
#define IOleInPlaceSite_ContextSensitiveHelp(This,fEnterMode)  (This)->lpVtbl->ContextSensitiveHelp(This,fEnterMode)
#define IOleInPlaceSite_CanInPlaceActivate(This)  (This)->lpVtbl->CanInPlaceActivate(This)
#define IOleInPlaceSite_OnInPlaceActivate(This)  (This)->lpVtbl->OnInPlaceActivate(This)
#define IOleInPlaceSite_OnUIActivate(This)  (This)->lpVtbl->OnUIActivate(This)
#define IOleInPlaceSite_GetWindowContext(This,ppFrame,ppDoc,lprcPosRect,lprcClipRect,lpFrameInfo)  (This)->lpVtbl->GetWindowContext(This,ppFrame,ppDoc,lprcPosRect,lprcClipRect,lpFrameInfo)
#define IOleInPlaceSite_Scroll(This,scrollExtant)  (This)->lpVtbl->Scroll(This,scrollExtant)
#define IOleInPlaceSite_OnUIDeactivate(This,fUndoable)  (This)->lpVtbl->OnUIDeactivate(This,fUndoable)
#define IOleInPlaceSite_OnInPlaceDeactivate(This)  (This)->lpVtbl->OnInPlaceDeactivate(This)
#define IOleInPlaceSite_DiscardUndoState(This)  (This)->lpVtbl->DiscardUndoState(This)
#define IOleInPlaceSite_DeactivateAndUndo(This)  (This)->lpVtbl->DeactivateAndUndo(This)
#define IOleInPlaceSite_OnPosRectChange(This,lprcPosRect)  (This)->lpVtbl->OnPosRectChange(This,lprcPosRect)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IOleInPlaceSite_CanInPlaceActivate_Proxy(IOleInPlaceSite*);
void __RPC_STUB IOleInPlaceSite_CanInPlaceActivate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceSite_OnInPlaceActivate_Proxy(IOleInPlaceSite*);
void __RPC_STUB IOleInPlaceSite_OnInPlaceActivate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceSite_OnUIActivate_Proxy(IOleInPlaceSite*);
void __RPC_STUB IOleInPlaceSite_OnUIActivate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceSite_GetWindowContext_Proxy(IOleInPlaceSite*,IOleInPlaceFrame**,IOleInPlaceUIWindow**,LPRECT,LPRECT,LPOLEINPLACEFRAMEINFO);
void __RPC_STUB IOleInPlaceSite_GetWindowContext_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceSite_Scroll_Proxy(IOleInPlaceSite*,SIZE);
void __RPC_STUB IOleInPlaceSite_Scroll_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceSite_OnUIDeactivate_Proxy(IOleInPlaceSite*,BOOL);
void __RPC_STUB IOleInPlaceSite_OnUIDeactivate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceSite_OnInPlaceDeactivate_Proxy(IOleInPlaceSite*);
void __RPC_STUB IOleInPlaceSite_OnInPlaceDeactivate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceSite_DiscardUndoState_Proxy(IOleInPlaceSite*);
void __RPC_STUB IOleInPlaceSite_DiscardUndoState_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceSite_DeactivateAndUndo_Proxy(IOleInPlaceSite*);
void __RPC_STUB IOleInPlaceSite_DeactivateAndUndo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IOleInPlaceSite_OnPosRectChange_Proxy(IOleInPlaceSite*,LPCRECT);
void __RPC_STUB IOleInPlaceSite_OnPosRectChange_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IOleInPlaceSite_INTERFACE_DEFINED__ */


#ifndef __IContinue_INTERFACE_DEFINED__
#define __IContinue_INTERFACE_DEFINED__

EXTERN_C const IID IID_IContinue;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0000012a-0000-0000-C000-000000000046")
IContinue:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE FContinue(void) = 0;
};
#else
typedef struct IContinueVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IContinue*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IContinue*);
    ULONG(STDMETHODCALLTYPE *Release)(IContinue*);
    HRESULT(STDMETHODCALLTYPE *FContinue)(IContinue*);
    END_INTERFACE
} IContinueVtbl;

interface IContinue {
    CONST_VTBL struct IContinueVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IContinue_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IContinue_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IContinue_Release(This)  (This)->lpVtbl->Release(This)
#define IContinue_FContinue(This)  (This)->lpVtbl->FContinue(This)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IContinue_FContinue_Proxy(IContinue*);
void __RPC_STUB IContinue_FContinue_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IContinue_INTERFACE_DEFINED__ */


#ifndef __IViewObject_INTERFACE_DEFINED__
#define __IViewObject_INTERFACE_DEFINED__

typedef IViewObject *LPVIEWOBJECT;
EXTERN_C const IID IID_IViewObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0000010d-0000-0000-C000-000000000046")
IViewObject:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Draw(DWORD,LONG,void*,DVTARGETDEVICE*,HDC,HDC,LPCRECTL,LPCRECTL,BOOL(STDMETHODCALLTYPE*)(ULONG_PTR),ULONG_PTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetColorSet(DWORD,LONG,void*,DVTARGETDEVICE*,HDC,LOGPALETTE**) = 0;
    virtual HRESULT STDMETHODCALLTYPE Freeze(DWORD,LONG,void*,DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unfreeze(DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetAdvise(DWORD,DWORD,IAdviseSink*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAdvise(DWORD*,DWORD*,IAdviseSink**) = 0;
};
#else
typedef struct IViewObjectVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IViewObject*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IViewObject*);
    ULONG(STDMETHODCALLTYPE *Release)(IViewObject*);
    HRESULT(STDMETHODCALLTYPE *Draw)(IViewObject*,DWORD,LONG,void*,DVTARGETDEVICE*,HDC,HDC,LPCRECTL,LPCRECTL,BOOL(STDMETHODCALLTYPE*)(ULONG_PTR),ULONG_PTR);
    HRESULT(STDMETHODCALLTYPE *GetColorSet)(IViewObject*,DWORD,LONG,void*,DVTARGETDEVICE*,HDC,LOGPALETTE**);
    HRESULT(STDMETHODCALLTYPE *Freeze)(IViewObject*,DWORD,LONG,void*,DWORD*);
    HRESULT(STDMETHODCALLTYPE *Unfreeze)(IViewObject*,DWORD);
    HRESULT(STDMETHODCALLTYPE *SetAdvise)(IViewObject*,DWORD,DWORD,IAdviseSink*);
    HRESULT(STDMETHODCALLTYPE *GetAdvise)(IViewObject*,DWORD*,DWORD*,IAdviseSink**);
    END_INTERFACE
} IViewObjectVtbl;

interface IViewObject {
    CONST_VTBL struct IViewObjectVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IViewObject_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IViewObject_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IViewObject_Release(This)  (This)->lpVtbl->Release(This)
#define IViewObject_Draw(This,dwDrawAspect,lindex,pvAspect,ptd,hdcTargetDev,hdcDraw,lprcBounds,lprcWBounds,pfnContinue,dwContinue)  (This)->lpVtbl->Draw(This,dwDrawAspect,lindex,pvAspect,ptd,hdcTargetDev,hdcDraw,lprcBounds,lprcWBounds,pfnContinue,dwContinue)
#define IViewObject_GetColorSet(This,dwDrawAspect,lindex,pvAspect,ptd,hicTargetDev,ppColorSet)  (This)->lpVtbl->GetColorSet(This,dwDrawAspect,lindex,pvAspect,ptd,hicTargetDev,ppColorSet)
#define IViewObject_Freeze(This,dwDrawAspect,lindex,pvAspect,pdwFreeze)  (This)->lpVtbl->Freeze(This,dwDrawAspect,lindex,pvAspect,pdwFreeze)
#define IViewObject_Unfreeze(This,dwFreeze)  (This)->lpVtbl->Unfreeze(This,dwFreeze)
#define IViewObject_SetAdvise(This,aspects,advf,pAdvSink)  (This)->lpVtbl->SetAdvise(This,aspects,advf,pAdvSink)
#define IViewObject_GetAdvise(This,pAspects,pAdvf,ppAdvSink)  (This)->lpVtbl->GetAdvise(This,pAspects,pAdvf,ppAdvSink)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IViewObject_RemoteDraw_Proxy(IViewObject*,DWORD,LONG,ULONG_PTR,DVTARGETDEVICE*,ULONG_PTR,ULONG_PTR,LPCRECTL,LPCRECTL,IContinue*);
void __RPC_STUB IViewObject_RemoteDraw_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IViewObject_RemoteGetColorSet_Proxy(IViewObject*,DWORD,LONG,ULONG_PTR,DVTARGETDEVICE*,ULONG_PTR,LOGPALETTE**);
void __RPC_STUB IViewObject_RemoteGetColorSet_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IViewObject_RemoteFreeze_Proxy(IViewObject*,DWORD,LONG,ULONG_PTR,DWORD*);
void __RPC_STUB IViewObject_RemoteFreeze_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IViewObject_Unfreeze_Proxy(IViewObject*,DWORD);
void __RPC_STUB IViewObject_Unfreeze_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IViewObject_SetAdvise_Proxy(IViewObject*,DWORD,DWORD,IAdviseSink*);
void __RPC_STUB IViewObject_SetAdvise_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IViewObject_RemoteGetAdvise_Proxy(IViewObject*,DWORD*,DWORD*,IAdviseSink**);
void __RPC_STUB IViewObject_RemoteGetAdvise_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IViewObject_INTERFACE_DEFINED__ */


#ifndef __IViewObject2_INTERFACE_DEFINED__
#define __IViewObject2_INTERFACE_DEFINED__

typedef IViewObject2 *LPVIEWOBJECT2;
EXTERN_C const IID IID_IViewObject2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000127-0000-0000-C000-000000000046")
IViewObject2:public IViewObject
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetExtent(DWORD,LONG,DVTARGETDEVICE*,LPSIZEL) = 0;
};
#else
typedef struct IViewObject2Vtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IViewObject2*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IViewObject2*);
    ULONG(STDMETHODCALLTYPE *Release)(IViewObject2*);
    HRESULT(STDMETHODCALLTYPE *Draw)(IViewObject2*,DWORD,LONG,void*,DVTARGETDEVICE*,HDC,HDC,LPCRECTL,LPCRECTL,BOOL(STDMETHODCALLTYPE*)(ULONG_PTR),ULONG_PTR);
    HRESULT(STDMETHODCALLTYPE *GetColorSet)(IViewObject2*,DWORD,LONG,void*,DVTARGETDEVICE*,HDC,LOGPALETTE**);
    HRESULT(STDMETHODCALLTYPE *Freeze)(IViewObject2*,DWORD,LONG,void*,DWORD*);
    HRESULT(STDMETHODCALLTYPE *Unfreeze)(IViewObject2*,DWORD);
    HRESULT(STDMETHODCALLTYPE *SetAdvise)(IViewObject2*,DWORD,DWORD,IAdviseSink*);
    HRESULT(STDMETHODCALLTYPE *GetAdvise)(IViewObject2*,DWORD*,DWORD*,IAdviseSink**);
    HRESULT(STDMETHODCALLTYPE *GetExtent)(IViewObject2*,DWORD,LONG,DVTARGETDEVICE*,LPSIZEL);
    END_INTERFACE
} IViewObject2Vtbl;

interface IViewObject2 {
    CONST_VTBL struct IViewObject2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IViewObject2_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IViewObject2_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IViewObject2_Release(This)  (This)->lpVtbl->Release(This)
#define IViewObject2_Draw(This,dwDrawAspect,lindex,pvAspect,ptd,hdcTargetDev,hdcDraw,lprcBounds,lprcWBounds,pfnContinue,dwContinue)  (This)->lpVtbl->Draw(This,dwDrawAspect,lindex,pvAspect,ptd,hdcTargetDev,hdcDraw,lprcBounds,lprcWBounds,pfnContinue,dwContinue)
#define IViewObject2_GetColorSet(This,dwDrawAspect,lindex,pvAspect,ptd,hicTargetDev,ppColorSet)  (This)->lpVtbl->GetColorSet(This,dwDrawAspect,lindex,pvAspect,ptd,hicTargetDev,ppColorSet)
#define IViewObject2_Freeze(This,dwDrawAspect,lindex,pvAspect,pdwFreeze)  (This)->lpVtbl->Freeze(This,dwDrawAspect,lindex,pvAspect,pdwFreeze)
#define IViewObject2_Unfreeze(This,dwFreeze)  (This)->lpVtbl->Unfreeze(This,dwFreeze)
#define IViewObject2_SetAdvise(This,aspects,advf,pAdvSink)  (This)->lpVtbl->SetAdvise(This,aspects,advf,pAdvSink)
#define IViewObject2_GetAdvise(This,pAspects,pAdvf,ppAdvSink)  (This)->lpVtbl->GetAdvise(This,pAspects,pAdvf,ppAdvSink)
#define IViewObject2_GetExtent(This,dwDrawAspect,lindex,ptd,lpsizel)  (This)->lpVtbl->GetExtent(This,dwDrawAspect,lindex,ptd,lpsizel)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IViewObject2_GetExtent_Proxy(IViewObject2*,DWORD,LONG,DVTARGETDEVICE*,LPSIZEL);
void __RPC_STUB IViewObject2_GetExtent_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IViewObject2_INTERFACE_DEFINED__ */


#ifndef __IDropSource_INTERFACE_DEFINED__
#define __IDropSource_INTERFACE_DEFINED__

typedef IDropSource *LPDROPSOURCE;
EXTERN_C const IID IID_IDropSource;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000121-0000-0000-C000-000000000046")
IDropSource:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD) = 0;
};
#else
typedef struct IDropSourceVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IDropSource*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IDropSource*);
    ULONG(STDMETHODCALLTYPE *Release)(IDropSource*);
    HRESULT(STDMETHODCALLTYPE *QueryContinueDrag)(IDropSource*,BOOL,DWORD);
    HRESULT(STDMETHODCALLTYPE *GiveFeedback)(IDropSource*,DWORD);
    END_INTERFACE
} IDropSourceVtbl;

interface IDropSource {
    CONST_VTBL struct IDropSourceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDropSource_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IDropSource_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IDropSource_Release(This)  (This)->lpVtbl->Release(This)
#define IDropSource_QueryContinueDrag(This,fEscapePressed,grfKeyState)  (This)->lpVtbl->QueryContinueDrag(This,fEscapePressed,grfKeyState)
#define IDropSource_GiveFeedback(This,dwEffect)  (This)->lpVtbl->GiveFeedback(This,dwEffect)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IDropSource_QueryContinueDrag_Proxy(IDropSource*,BOOL,DWORD);
void __RPC_STUB IDropSource_QueryContinueDrag_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDropSource_GiveFeedback_Proxy(IDropSource*,DWORD);
void __RPC_STUB IDropSource_GiveFeedback_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IDropSource_INTERFACE_DEFINED__ */


#ifndef __IDropTarget_INTERFACE_DEFINED__
#define __IDropTarget_INTERFACE_DEFINED__

typedef IDropTarget *LPDROPTARGET;
EXTERN_C const IID IID_IDropTarget;

#define MK_ALT  0x20

#define DROPEFFECT_NONE  0
#define DROPEFFECT_COPY  1
#define DROPEFFECT_MOVE  2
#define DROPEFFECT_LINK  4
#define DROPEFFECT_SCROLL  0x80000000

#define DD_DEFSCROLLINSET  11
#define DD_DEFSCROLLDELAY  50
#define DD_DEFSCROLLINTERVAL  50
#define DD_DEFDRAGDELAY  200
#define DD_DEFDRAGMINDIST  2

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000122-0000-0000-C000-000000000046")
IDropTarget:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject*,DWORD,POINTL,DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD,POINTL,DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE DragLeave(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject*,DWORD,POINTL,DWORD*) = 0;
};
#else
typedef struct IDropTargetVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IDropTarget*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IDropTarget*);
    ULONG(STDMETHODCALLTYPE *Release)(IDropTarget*);
    HRESULT(STDMETHODCALLTYPE *DragEnter)(IDropTarget*,IDataObject*,DWORD,POINTL,DWORD*);
    HRESULT(STDMETHODCALLTYPE *DragOver)(IDropTarget*,DWORD,POINTL,DWORD*);
    HRESULT(STDMETHODCALLTYPE *DragLeave)(IDropTarget*);
    HRESULT(STDMETHODCALLTYPE *Drop)(IDropTarget*,IDataObject*,DWORD,POINTL,DWORD*);
    END_INTERFACE
} IDropTargetVtbl;

interface IDropTarget {
    CONST_VTBL struct IDropTargetVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDropTarget_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IDropTarget_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IDropTarget_Release(This)  (This)->lpVtbl->Release(This)
#define IDropTarget_DragEnter(This,pDataObj,grfKeyState,pt,pdwEffect)  (This)->lpVtbl->DragEnter(This,pDataObj,grfKeyState,pt,pdwEffect)
#define IDropTarget_DragOver(This,grfKeyState,pt,pdwEffect)  (This)->lpVtbl->DragOver(This,grfKeyState,pt,pdwEffect)
#define IDropTarget_DragLeave(This)  (This)->lpVtbl->DragLeave(This)
#define IDropTarget_Drop(This,pDataObj,grfKeyState,pt,pdwEffect)  (This)->lpVtbl->Drop(This,pDataObj,grfKeyState,pt,pdwEffect)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IDropTarget_DragEnter_Proxy(IDropTarget*,IDataObject*,DWORD,POINTL,DWORD*);
void __RPC_STUB IDropTarget_DragEnter_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDropTarget_DragOver_Proxy(IDropTarget*,DWORD,POINTL,DWORD*);
void __RPC_STUB IDropTarget_DragOver_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDropTarget_DragLeave_Proxy(IDropTarget*);
void __RPC_STUB IDropTarget_DragLeave_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDropTarget_Drop_Proxy(IDropTarget*,IDataObject*,DWORD,POINTL,DWORD*);
void __RPC_STUB IDropTarget_Drop_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IDropTarget_INTERFACE_DEFINED__ */


#ifndef __IEnumOLEVERB_INTERFACE_DEFINED__
#define __IEnumOLEVERB_INTERFACE_DEFINED__

typedef IEnumOLEVERB *LPENUMOLEVERB;

typedef struct tagOLEVERB {
    LONG lVerb;
    LPOLESTR lpszVerbName;
    DWORD fuFlags;
    DWORD grfAttribs;
} OLEVERB;

typedef struct tagOLEVERB *LPOLEVERB;

typedef enum tagOLEVERBATTRIB {
    OLEVERBATTRIB_NEVERDIRTIES = 1,
    OLEVERBATTRIB_ONCONTAINERMENU = 2
} OLEVERBATTRIB;

EXTERN_C const IID IID_IEnumOLEVERB;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000104-0000-0000-C000-000000000046")
IEnumOLEVERB:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Next(ULONG,LPOLEVERB,ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip(ULONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IEnumOLEVERB**) = 0;
};
#else
typedef struct IEnumOLEVERBVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IEnumOLEVERB*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IEnumOLEVERB*);
    ULONG(STDMETHODCALLTYPE *Release)(IEnumOLEVERB*);
    HRESULT(STDMETHODCALLTYPE *Next)(IEnumOLEVERB*,ULONG,LPOLEVERB,ULONG*);
    HRESULT(STDMETHODCALLTYPE *Skip)(IEnumOLEVERB*,ULONG);
    HRESULT(STDMETHODCALLTYPE *Reset)(IEnumOLEVERB*);
    HRESULT(STDMETHODCALLTYPE *Clone)(IEnumOLEVERB*,IEnumOLEVERB**);
    END_INTERFACE
} IEnumOLEVERBVtbl;

interface IEnumOLEVERB {
    CONST_VTBL struct IEnumOLEVERBVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEnumOLEVERB_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IEnumOLEVERB_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IEnumOLEVERB_Release(This)  (This)->lpVtbl->Release(This)
#define IEnumOLEVERB_Next(This,celt,rgelt,pceltFetched)  (This)->lpVtbl->Next(This,celt,rgelt,pceltFetched)
#define IEnumOLEVERB_Skip(This,celt)  (This)->lpVtbl->Skip(This,celt)
#define IEnumOLEVERB_Reset(This)  (This)->lpVtbl->Reset(This)
#define IEnumOLEVERB_Clone(This,ppenum)  (This)->lpVtbl->Clone(This,ppenum)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IEnumOLEVERB_RemoteNext_Proxy(IEnumOLEVERB*,ULONG,LPOLEVERB,ULONG*);
void __RPC_STUB IEnumOLEVERB_RemoteNext_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumOLEVERB_Skip_Proxy(IEnumOLEVERB*,ULONG);
void __RPC_STUB IEnumOLEVERB_Skip_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumOLEVERB_Reset_Proxy(IEnumOLEVERB*);
void __RPC_STUB IEnumOLEVERB_Reset_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEnumOLEVERB_Clone_Proxy(IEnumOLEVERB*,IEnumOLEVERB**);
void __RPC_STUB IEnumOLEVERB_Clone_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IEnumOLEVERB_INTERFACE_DEFINED__ */


unsigned long __RPC_USER CLIPFORMAT_UserSize(unsigned long*,unsigned long, CLIPFORMAT*);
unsigned char *__RPC_USER CLIPFORMAT_UserMarshal(unsigned long*,unsigned char*,CLIPFORMAT*);
unsigned char *__RPC_USER CLIPFORMAT_UserUnmarshal(unsigned long*,unsigned char*,CLIPFORMAT*);
void __RPC_USER CLIPFORMAT_UserFree(unsigned long*,CLIPFORMAT*);
unsigned long __RPC_USER HACCEL_UserSize(unsigned long*,unsigned long,HACCEL*);
unsigned char  *__RPC_USER HACCEL_UserMarshal(unsigned long*, unsigned char*,HACCEL*);
unsigned char  *__RPC_USER HACCEL_UserUnmarshal(unsigned long*,unsigned char*,HACCEL*);
void __RPC_USER HACCEL_UserFree(unsigned long*,HACCEL*);
unsigned long __RPC_USER HGLOBAL_UserSize(unsigned long*,unsigned long,HGLOBAL*);
unsigned char  *__RPC_USER HGLOBAL_UserMarshal(unsigned long*,unsigned char*,HGLOBAL*);
unsigned char  *__RPC_USER HGLOBAL_UserUnmarshal(unsigned long*,unsigned char*,HGLOBAL*);
void __RPC_USER HGLOBAL_UserFree(unsigned long*,HGLOBAL*);
unsigned long __RPC_USER HMENU_UserSize(unsigned long*,unsigned long,HMENU*);
unsigned char  *__RPC_USER HMENU_UserMarshal(unsigned long*,unsigned char*,HMENU*);
unsigned char  *__RPC_USER HMENU_UserUnmarshal(unsigned long*,unsigned char*,HMENU*);
void __RPC_USER HMENU_UserFree(unsigned long*,HMENU*);
unsigned long __RPC_USER HWND_UserSize(unsigned long*,unsigned long,HWND*);
unsigned char  *__RPC_USER HWND_UserMarshal(unsigned long*,unsigned char*,HWND*);
unsigned char  *__RPC_USER HWND_UserUnmarshal(unsigned long*,unsigned char*,HWND*);
void __RPC_USER HWND_UserFree(unsigned long*,HWND*);
unsigned long __RPC_USER STGMEDIUM_UserSize(unsigned long*,unsigned long,STGMEDIUM*);
unsigned char  *__RPC_USER STGMEDIUM_UserMarshal(unsigned long*,unsigned char*,STGMEDIUM*);
unsigned char  *__RPC_USER STGMEDIUM_UserUnmarshal(unsigned long*,unsigned char*,STGMEDIUM*);
void __RPC_USER STGMEDIUM_UserFree(unsigned long*,STGMEDIUM*);

HRESULT STDMETHODCALLTYPE IOleCache2_UpdateCache_Proxy(IOleCache2*,LPDATAOBJECT,DWORD,LPVOID);
HRESULT STDMETHODCALLTYPE IOleCache2_UpdateCache_Stub(IOleCache2*,LPDATAOBJECT,DWORD,LONG_PTR);
HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_TranslateAccelerator_Proxy(IOleInPlaceActiveObject*,LPMSG);
HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_TranslateAccelerator_Stub(IOleInPlaceActiveObject*);
HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_ResizeBorder_Proxy(IOleInPlaceActiveObject*,LPCRECT,IOleInPlaceUIWindow*,BOOL);
HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_ResizeBorder_Stub(IOleInPlaceActiveObject*,LPCRECT,REFIID,IOleInPlaceUIWindow*,BOOL);
HRESULT STDMETHODCALLTYPE IViewObject_Draw_Proxy(IViewObject*,DWORD,LONG,void*,DVTARGETDEVICE*,HDC,HDC,LPCRECTL,LPCRECTL,BOOL(STDMETHODCALLTYPE*)(ULONG_PTR),ULONG_PTR);
HRESULT STDMETHODCALLTYPE IViewObject_Draw_Stub(IViewObject*,DWORD,LONG,ULONG_PTR,DVTARGETDEVICE*,ULONG_PTR,ULONG_PTR,LPCRECTL,LPCRECTL,IContinue*);
HRESULT STDMETHODCALLTYPE IViewObject_GetColorSet_Proxy(IViewObject*,DWORD,LONG,void*,DVTARGETDEVICE*,HDC,LOGPALETTE**);
HRESULT STDMETHODCALLTYPE IViewObject_GetColorSet_Stub(IViewObject*,DWORD,LONG,ULONG_PTR,DVTARGETDEVICE*,ULONG_PTR,LOGPALETTE**);
HRESULT STDMETHODCALLTYPE IViewObject_Freeze_Proxy(IViewObject*,DWORD,LONG,void*,DWORD*);
HRESULT STDMETHODCALLTYPE IViewObject_Freeze_Stub(IViewObject*,DWORD,LONG,ULONG_PTR,DWORD*);
HRESULT STDMETHODCALLTYPE IViewObject_GetAdvise_Proxy(IViewObject*,DWORD*,DWORD*,IAdviseSink**);
HRESULT STDMETHODCALLTYPE IViewObject_GetAdvise_Stub(IViewObject*,DWORD*,DWORD*,IAdviseSink**);
HRESULT STDMETHODCALLTYPE IEnumOLEVERB_Next_Proxy(IEnumOLEVERB*,ULONG,LPOLEVERB,ULONG*);
HRESULT STDMETHODCALLTYPE IEnumOLEVERB_Next_Stub(IEnumOLEVERB*,ULONG,LPOLEVERB,ULONG*);

#ifdef __cplusplus
}
#endif

#endif /* _OLEIDL_H */
